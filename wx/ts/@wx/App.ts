import * as wx from "./wx";

export enum NavigationBarTextStyle {
    White = "white",
    Black = "black"
}

export enum NavigationStyle {
    Default = "default",
    Custom = "custom"
}

export enum BackgroundTextStyle {
    Dark = 'dark',
    Light = 'light'
}

export enum PageOrientation {
    Portrait = 'portrait',
    Auto = 'auto',
    Landscape = 'landscape'
}

export interface WindowStyle {
    /**
    navigationBarBackgroundColor	HexColor	#000000	导航栏背景颜色，如 #000000	
    navigationBarTextStyle	String	white	导航栏标题颜色，仅支持 black / white	
    navigationBarTitleText	String		导航栏标题文字内容	
    navigationStyle	String	default	导航栏样式，仅支持以下值：
    default 默认样式
    custom 自定义导航栏，只保留右上角胶囊按钮。参见注2。	微信客户端 6.6.0
    backgroundColor	HexColor	#ffffff	窗口的背景色	
    backgroundTextStyle	String	dark	下拉 loading 的样式，仅支持 dark / light	
    backgroundColorTop	String	#ffffff	顶部窗口的背景色，仅 iOS 支持	微信客户端 6.5.16
    backgroundColorBottom	String	#ffffff	底部窗口的背景色，仅 iOS 支持	微信客户端 6.5.16
    enablePullDownRefresh	Boolean	false	是否开启当前页面的下拉刷新。
    详见 Page.onPullDownRefresh	
    onReachBottomDistance	Number	50	页面上拉触底事件触发时距页面底部距离，单位为px。
    详见 Page.onReachBottom	
    pageOrientation	String	portrait	屏幕旋转设置，支持 auto / portrait / landscape 
    详见 响应显示区域变化	2.4.0 (auto) / 2.5.0 (landscape)
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
}

export interface TabBarItem {
    readonly pagePath: string
    readonly iconPath: string
    readonly selectedIconPath: string
    readonly text: string
}

export enum TabBarBorderStyle {
    Black = 'black',
    White = 'white'
}

export enum TabBarPosition {
    Bottom = 'bottom',
    Top = 'top'
}

export interface TabBar {
    /**
    color	HexColor	是		tab 上的文字默认颜色，仅支持十六进制颜色	
    selectedColor	HexColor	是		tab 上的文字选中时的颜色，仅支持十六进制颜色	
    backgroundColor	HexColor	是		tab 的背景色，仅支持十六进制颜色	
    borderStyle	String	否	black	tabbar上边框的颜色， 仅支持 black / white	
    list	Array	是		tab 的列表，详见 list 属性说明，最少2个、最多5个 tab	
    position	String	否	bottom	tabBar的位置，仅支持 bottom / top	
    custom	Boolean	否	false	自定义 tabBar，见详情
     */
    readonly color: string
    readonly selectedColor: string
    readonly backgroundColor: string
    readonly borderStyle?: TabBarBorderStyle
    readonly list: TabBarItem[]
    readonly position?: TabBarPosition
    readonly custom: number
}

export interface NetworkTimeout {
    readonly request?: number
    readonly connectSocket?: number
    readonly uploadFile?: number
    readonly downloadFile?: number
}

export interface AppOptions {
    readonly pages: string[]
    readonly window?: WindowStyle
    readonly tabBar?: TabBar
    readonly networkTimeout?: NetworkTimeout
    readonly debug?: boolean
}

export interface QueryObject {
    [key: string]: string
}

export interface AppLaunchOptions {
    readonly path: string
    readonly scene?: number
    readonly query: QueryObject
    readonly shareTicket?: string
    readonly referrerInfo?: any
}

export interface AppShowOptions extends AppLaunchOptions {

}

export interface AppPageNotFoundRes {
    readonly path: string
    readonly query: QueryObject
    readonly isEntryPage: boolean
}

export interface AppObject {
    [key: string]: any
    readonly onLaunch?: (options: AppLaunchOptions) => void
    readonly onShow?: (options: AppShowOptions) => void
    readonly onHide?: () => void
    readonly onError?: (error: string) => void
    readonly onPageNotFound?: (res: AppPageNotFoundRes) => void
}

export interface LibrarySet {
    [key: string]: any
}

export interface Recycle {
    recycle(): void
}

class RefCount<T extends Recycle> {
    readonly object: T;
    private _retainCount: number;
    constructor(object: T) {
        this.object = object;
        this._retainCount = 0;
    }
    retain(): void {
        this._retainCount++;
    }
    release(): void {
        this._retainCount--;
        if (this._retainCount == 0) {
            this.object.recycle();
        }
    }
}

interface AppSet {
    [id: number]: RefCount<App>
}

var autoId: number = 0;
var appSet: AppSet = {}

export class App implements Recycle {

    private _object: AppObject

    readonly appid: number

    readonly basePath: String

    readonly options: AppOptions

    get object():AppObject{
        return this._object
    }
    
    constructor(basePath: string) {
        this.appid = ++autoId;
        this.basePath = basePath;
        {
            let v = app.getTextContent(basePath + "/app.json");
            if (v !== undefined) {
                this.options = JSON.parse(v);
            }
        }
    }

    run(path?: string, query?: QueryObject): void {

        if (appSet[this.appid] !== undefined) {
            throw "小程序已运行"
        }

        let librarySet = {
            getApp: (): AppObject => {
                return this._object;
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

        let keys: string[] = ["App"];
        let values: any[] = [
            (object: AppObject): void => {
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
            let v = app.getTextContent(this.basePath + "/app.js");
            if (v !== undefined) {
                code.push(v);
            }
        }

        code.push("\n})");

        let fn = compile(code.join(''), "app.js")();

        fn.apply(undefined, values);

        appSet[this.appid] = new RefCount(this);

        if (this.options.tabBar !== undefined) {

            app.open("@wx/page/tabbar.js?appid=" + this.appid, path === undefined);

        } else {

            if (path === undefined) {
                if (this.options.pages.length > 0) {
                    path = this.options.pages[0];
                }
            }

            if (query === undefined) {
                query = {}
            }

            if (path === undefined) {
                delete appSet[this.appid];
                throw "未找到小程序页面"
            }

            app.open("@wx/page/page.js?appid=" + this.appid
                + "&path=" + encodeURIComponent(path)
                + "&query=" + encodeURIComponent(JSON.stringify(query)), true);

        }
    }

    recycle(): void {
        delete appSet[this.appid];
    }
}

export function getApp(id: number | string): RefCount<App> | undefined {
    return appSet[id];
}
