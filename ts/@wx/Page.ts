import { App, NavigationBarTextStyle, NavigationStyle, PageOrientation, Recycle, QueryObject, AppObject } from './App';
import * as wx from "./wx";
import { Component } from './Component';
import { PageInterface } from './PageInterface';
import { ViewContext } from './ViewContext';
import { dirname } from './func';

export enum BackgroundTextStyle {
    Dark = 'dark',
    Light = 'light'
}

export interface PageOptions {
    /**
    navigationBarBackgroundColor	HexColor	#000000	导航栏背景颜色，如 #000000	
    navigationBarTextStyle	String	white	导航栏标题颜色，仅支持 black / white	
    navigationBarTitleText	String		导航栏标题文字内容	
    navigationStyle	String	default	导航栏样式，仅支持以下值：
    default 默认样式
    custom 自定义导航栏，只保留右上角胶囊按钮	微信客户端 7.0.0
    backgroundColor	HexColor	#ffffff	窗口的背景色	
    backgroundTextStyle	String	dark	下拉 loading 的样式，仅支持 dark / light	
    backgroundColorTop	String	#ffffff	顶部窗口的背景色，仅 iOS 支持	微信客户端 6.5.16
    backgroundColorBottom	String	#ffffff	底部窗口的背景色，仅 iOS 支持	微信客户端 6.5.16
    enablePullDownRefresh	Boolean	false	是否全局开启下拉刷新。
    详见 Page.onPullDownRefresh	
    onReachBottomDistance	Number	50	页面上拉触底事件触发时距页面底部距离，单位为px。
    详见 Page.onReachBottom	
    pageOrientation	String	portrait	屏幕旋转设置，支持 auto / portrait / landscape 
    详见 响应显示区域变化	2.4.0 (auto) / 2.5.0 (landscape)
    disableScroll	Boolean	false	设置为 true 则页面整体不能上下滚动。
    只在页面配置中有效，无法在 app.json 中设置	
    disableSwipeBack	Boolean	false	禁止页面右滑手势返回	微信客户端 7.0.0
    usingComponents	Object	否	页面自定义组件配置	1.6.3
     */
    readonly navigationBarBackgroundColor?: string
    readonly navigationBarTextStyle?: NavigationBarTextStyle
    readonly navigationBarTitleText?: string
    readonly navigationStyle?: NavigationStyle
    readonly backgroundColor?: string
    readonly backgroundTextStyle?: BackgroundTextStyle
    readonly backgroundColorTop?: string
    readonly backgroundColorBottom?: string
    readonly enablePullDownRefresh?: boolean
    readonly onReachBottomDistance?: number
    readonly pageOrientation?: PageOrientation
    readonly disableScroll?: boolean
    readonly disableSwipeBack?: boolean
    readonly usingComponents?: any
}

export interface PageShareAppMessageObject {
    from: string
    target: any
    webviewUrl: string
}

export interface PageShareAppMessageResult {
    title: string
    path: string
    imageUrl?: string
}

export interface PageObject {
    data?: ker.DataObject
    setData?: (object: ker.DataObject) => void
    onLoad?: (query: QueryObject) => void
    onShow?: () => void
    onReady?: () => void
    onHide?: () => void
    onUnload?: () => void
    onPullDownRefresh?: () => void
    onReachBottom?: () => void
    onPageScroll?: () => void
    onShareAppMessage?: (object: PageShareAppMessageObject) => PageShareAppMessageResult
}

export interface PageViewInterface {
    ondata?: (object: any) => void
    setData(object: ker.DataObject, id?: string): void
    sendEvent(id: string, name: string, data: any): void
    addSubview(view: UIView): void
    setHTMLContent(content: string, basePath: string): void
}

interface ComponentSet {
    [key: string]: Component
}

export class Page implements Recycle {

    private _object: PageObject
    private _options: PageOptions

    readonly app: App;
    get object(): PageObject {
        return this._object
    }

    get options(): PageOptions {
        return this._options
    }

    private _components: ComponentSet;

    constructor(app: App, viewInterface: PageViewInterface) {
        this.app = app;
        this._components = {};
    }

    run(viewInterface: PageViewInterface, path: string, query: QueryObject) {

        {
            let v = app.getTextContent(this.app.basePath + "/" + path + ".json");
            if (v !== undefined) {
                this._options = JSON.parse(v);
            }
        }

        let wx = new PageInterface(this.app, dirname(path));

        let librarySet = {
            getApp: (): AppObject => {
                return this.app.object;
            },
            setTimeout: (fn: () => void, tv: number): any => {
                return setTimeout((): void => {
                    fn.apply(this);
                }, tv);
            },
            setInterval: (fn: () => void, tv: number): any => {
                return setInterval((): void => {
                    fn.apply(this);
                }, tv);
            },
            clearTimeout: (id: any): void => {
                clearTimeout(id);
            },
            clearInterval: (id: any): void => {
                clearInterval(id);
            },
            wx: wx
        }

        let keys: string[] = ["Page"];
        let values: any[] = [
            (object: PageObject): void => {
                this._object = object;
            }
        ];

        for (let key in librarySet) {
            keys.push(key);
            values.push(librarySet[key]);
        }

        let code = ["(function("];

        code.push(keys.join(","));

        code.push("){\n");

        {
            let v = app.getTextContent(this.app.basePath + "/" + path + ".js");
            if (v !== undefined) {
                code.push(v);
            }
        }

        code.push("\n})");

        let fn = compile(code.join(''), this.app.basePath + "/" + path + ".js")();

        fn.apply(undefined, values);

        if (this._object.data === undefined) {
            this._object.data = {};
        }

        this._object.setData = (object: ker.DataObject): void => {
            viewInterface.setData(object);
        };


        let viewContext: ViewContext = new ViewContext(viewInterface);

        viewInterface.ondata = (data: any): void => {
            if (data.page == 'readying') {

                if (this._object.onLoad !== undefined) {
                    this._object.onLoad(query);
                }

                if (this._object.onShow !== undefined) {
                    this._object.onShow();
                }

                viewInterface.setData(this._object.data);

            } else if (data.page == 'ready') {
                if (this._object.onReady !== undefined) {
                    this._object.onReady();
                }

            } else if (data.page == 'open') {
                wx.navigateTo({
                    url: data.url
                })
            } else if (data.event) {
                if (data.componentId) {
                    var v = this._components[data.componentId];
                    if (v) {
                        v.doEvent(data.event, data.data);
                    }
                } else {
                    var fn = this._object[data.event];
                    if (typeof fn == 'function') {
                        fn.call(this._object, data.data);
                    }
                }
            } else if (data.view == 'create') {
                viewContext.create(data.name, data.id);
            } else if (data.view == 'set') {
                viewContext.set(data.id, data.name, data.value);
            } else if (data.view == 'setFrame') {
                viewContext.setFrame(data.id, data.x, data.y, data.width, data.height);
            } else if (data.view == 'setContentSize') {
                viewContext.setContentSize(data.id, data.width, data.height);
            } else if (data.view == 'add') {
                viewContext.add(data.id, data.pid);
            } else if (data.view == 'remove') {
                viewContext.remove(data.id);
            } else if (data.view == 'on') {

                viewContext.on(data.id, data.name, function (e) {
                    viewInterface.sendEvent(data.id, data.name, e.data);
                });

            } else if (data.view == 'off') {
                viewContext.off(data.id, data.name);
            } else if (data.component == 'add') {
                this._components[data.id] = new Component(this.app, data.id, data.path, viewContext, viewInterface, wx);
            } else if (data.component == 'remove') {
                var v = this._components[data.id];
                if (v) {
                    v.remove();
                    delete this._components[data.id];
                }
            } else if (data.component == 'set') {
                var v = this._components[data.id];
                if (v) {
                    v.set(data.name, data.value);
                }
            }
        };


        let p = this.app.basePath + "/" + path + ".wx.html";
        var content = app.getTextContent(p);
        var rem = (screen.width / screen.density) * 20 / 750.0;
        console.info("[REM]", rem, screen);

        let vs: any[] = [
            '<html style="font-size: ',
            rem,
            'px">',
            '<head><meta charset="utf-8"/><meta name="viewport" content="width=device-width,minimum-scale=1,maximum-scale=1,initial-scale=1,user-scalable=no" />',
            '<style type="text/css">',
            this.app.cssContent,
            '</style>',
            content,
            '</html>'

        ];

        viewInterface.setHTMLContent(vs.join(''), dirname(p) + '/');

    }

    recycle(): void {

        for (let id in this._components) {
            let v = this._components[id];
            v.onUnload();
        }

        if (this._object.onUnload !== undefined) {
            this._object.onUnload();
        }

    }

    show(): void {


        if (this._object.onShow !== undefined) {
            this._object.onShow();
        }

        for (let id in this._components) {
            let v = this._components[id];
            v.onShow();
        }

    }

    hide(): void {

        for (let id in this._components) {
            let v = this._components[id];
            v.onHide();
        }

        if (this._object.onHide !== undefined) {
            this._object.onHide();
        }
    }

}
