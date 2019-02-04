import { ViewContext } from "./ViewContext";
import { App } from "./App";
import { dirname, relativePath } from "./func";
import { PageInterface } from './PageInterface';
import { PageViewInterface } from './Page';


export interface ComponentObject {
    [key: string]: any
    data?: ker.DataObject
    setData?: (object: ker.DataObject) => void
    triggerEvent?: (name: string, detail: any, option: any) => void
}

export class Component {

    private _viewContext: ViewContext
    private _id: string
    private _path: string
    private _viewInterface: PageViewInterface
    private _basePath: string
    private _app: App
    private _object: ComponentObject

    get object(): ComponentObject {
        return this._object;
    }

    constructor(a: App, id: string, path: string, viewContext: ViewContext, viewInterface: PageViewInterface, wx: PageInterface) {
        this._viewContext = viewContext;
        this._app = a;
        this._id = id;
        this._path = path;
        this._viewInterface = viewInterface
        this._basePath = a.basePath + "/" + dirname(path);

        (function (component: Component, basePath: string, appObject: any) {

            console.info("[COMPONENT] [READYING]");
            let filePath = a.basePath + "/" + path + ".js";
            var fn = compile("(function(Component, getApp, app, page, wx,setInterval,setTimeout,require){" + app.getTextContent(filePath) + "})", filePath)();

            fn(
                function (object) {
                    component._object = object;
                },
                function () {
                    return a.object;
                },
                undefined,
                undefined,
                wx,
                function (fn: any, v: number) {
                    setInterval(function () {
                        fn.call(component.object);
                    }, v);
                },
                function (fn: any, v: number) {
                    setTimeout(function () {
                        fn.call(component.object);
                    }, v);
                },
                function (path: string) {
                    var p = relativePath(path, basePath);
                    if (!p.endsWith(".js")) {
                        p = p + ".js";
                    }
                    return require(p);
                }
            );

            component.object.triggerEvent = function (name: string, detail: any, option: any) {
                viewInterface.sendEvent(id, name, {
                    detail: detail,
                    option: option
                });
            };

            component.object.setData = function (data: any) {
                for (var key in data) {
                    this.data[key] = data[key];
                }
                viewInterface.setData(data, id)
            };

            if (component.object.data === undefined) {
                component.object.data = {};
            }

            viewInterface.setData(component.object.data, id)

        })(this, dirname(path), this._app.object);

        var fn: any;
        var lifetimes = this.object['lifetimes'];

        if (lifetimes) {

            fn = lifetimes.object['created'];
            if (typeof fn == 'function') {
                fn.call(this.object);
            }

            fn = lifetimes.object['attached'];
            if (typeof fn == 'function') {
                fn.call(this.object);
            }

        }

        fn = this.object['created'];
        if (typeof fn == 'function') {
            fn.call(this.object);
        }

        fn = this.object['attached'];
        if (typeof fn == 'function') {
            fn.call(this.object);
        }

        fn = this.object['show'];
        if (typeof fn == 'function') {
            fn.call(this.object);
        }
    }

    remove(): void {
        this.onHide();
        this.onUnload();
    }

    set(name: string, value: string): void {

    }

    onUnload(): void {

        var fn: any;
        var lifetimes = this.object['lifetimes'];

        if (lifetimes) {

            fn = lifetimes.object['detached'];
            if (typeof fn == 'function') {
                fn.call(this.object);
            }

        }

        fn = this.object['detached'];
        if (typeof fn == 'function') {
            fn.call(this.object);
        }

    }

    doEvent(name: string, data: any): void {
        var methods = this.object.methods;
        if (methods) {
            var fn = methods[name];
            if (typeof fn == 'function') {
                fn.call(this.object, data);
            }
        }
    }

    onShow(): void {

        var fn = this.object['show'];
        if (typeof fn == 'function') {
            fn.call(this.object);
        }

    }

    onHide(): void {
        var fn = this.object['hide'];
        if (typeof fn == 'function') {
            fn.call(this.object);
        }
    }

}