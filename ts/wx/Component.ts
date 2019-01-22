import { ViewContext } from "./ViewContext";
import { App } from "./App";
import { dirname } from "./func";

export class Component {
    private _viewContext: ViewContext
    private _id: string
    private _path: string
    private _page: UIPage
    private _view: UIView
    private _basePath: string
    private _app: App
    public object: any

    constructor(a: App, id: string, path: string, page: UIPage, viewContext: ViewContext) {
        this._viewContext = viewContext;
        this._app = a;
        this._id = id;
        this._path = path;
        this._page = page;
        this._view = viewContext.view;
        this._basePath = dirname(path);
        this.object = { data: {} };

        (function (component: Component, basePath: string, webview: UIView, appObject: any) {

            console.info("[COMPONENT] [READYING]");

            var fn = compile("(function(Component, getApp, app, page, wx,setInterval,setTimeout,require){" + app.getTextContent(path + ".js") + "})", path + ".js")();

            fn(
                function (object) {
                    component.object = object;
                },
                function () {
                    return a.object;
                },
                undefined,
                undefined,
                new wx.API(),
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
                    var p = wx.relativePath(path, basePath);
                    if (!p.endsWith(".js")) {
                        p = p + ".js";
                    }
                    return require(p);
                }
            );

            component.object.triggerEvent = function (name: string, detail: any, option: any) {
                webview.evaluateJavaScript("kk.sendEvent(" + id + ',' + JSON.stringify(name) + "," + JSON.stringify({
                    detail: detail,
                    option: option
                }) + ");");
            };

            component.object.setData = function (data: any) {
                for (var key in data) {
                    this.data[key] = data[key];
                }
                webview.evaluateJavaScript("kk.setData(" + JSON.stringify(data) + "," + id + ");");
            };

            if (component.object.data === undefined) {
                component.object.data = {};
            }

            webview.evaluateJavaScript("kk.setData(" + JSON.stringify(component.object.data) + "," + id + ");");

        })(this, wx.dirname(path), this._view, this._app.object);

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