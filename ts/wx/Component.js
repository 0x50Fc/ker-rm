var wx;
(function (wx) {
    var Component = /** @class */ (function () {
        function Component(a, id, path, page, viewContext) {
            this._viewContext = viewContext;
            this._app = a;
            this._id = id;
            this._path = path;
            this._page = page;
            this._view = viewContext.view;
            this._basePath = wx.dirname(path);
            this.object = { data: {} };
            (function (component, basePath, webview, appObject) {
                console.info("[COMPONENT] [READYING]");
                var fn = compile("(function(Component, getApp, app, page, wx,setInterval,setTimeout,require){" + app.getTextContent(path + ".js") + "})", path + ".js")();
                fn(function (object) {
                    component.object = object;
                }, function () {
                    return a.object;
                }, undefined, undefined, new wx.API(), function (fn, v) {
                    setInterval(function () {
                        fn.call(component.object);
                    }, v);
                }, function (fn, v) {
                    setTimeout(function () {
                        fn.call(component.object);
                    }, v);
                }, function (path) {
                    var p = wx.relativePath(path, basePath);
                    if (!p.endsWith(".js")) {
                        p = p + ".js";
                    }
                    return require(p);
                });
                component.object.triggerEvent = function (name, detail, option) {
                    webview.evaluateJavaScript("kk.sendEvent(" + id + ',' + JSON.stringify(name) + "," + JSON.stringify({
                        detail: detail,
                        option: option
                    }) + ");");
                };
                component.object.setData = function (data) {
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
            var fn;
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
        Component.prototype.onUnload = function () {
            var fn;
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
        };
        Component.prototype.doEvent = function (name, data) {
            var methods = this.object.methods;
            if (methods) {
                var fn = methods[name];
                if (typeof fn == 'function') {
                    fn.call(this.object, data);
                }
            }
        };
        Component.prototype.onShow = function () {
            var fn = this.object['show'];
            if (typeof fn == 'function') {
                fn.call(this.object);
            }
        };
        Component.prototype.onHide = function () {
            var fn = this.object['hide'];
            if (typeof fn == 'function') {
                fn.call(this.object);
            }
        };
        return Component;
    }());
    wx.Component = Component;
})(wx || (wx = {}));
