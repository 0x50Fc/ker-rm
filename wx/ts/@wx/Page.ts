import { App, NavigationBarTextStyle, NavigationStyle, PageOrientation, Recycle, QueryObject, AppObject } from './App';
import * as wx from "./wx";

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

export interface PageObject {

}

export class Page implements Recycle {

    private _object:PageObject
    private _options:PageOptions

    readonly app: App;

    get object():PageObject {
        return this._object
    }

    get options():PageOptions {
        return this._options
    }

    constructor(app: App) {
        this.app = app
    }

    run(path: string, query: QueryObject) {

        {
            let v = app.getTextContent(this.app.basePath + "/" + path + ".json");
            if(v !== undefined) {
                this._options = JSON.parse(v);
            }
        }
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
    }

    recycle(): void {

    }
}
