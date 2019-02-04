"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var func_1 = require("./func");
var Component = /** @class */ (function () {
    function Component(a, id, path, viewContext, viewInterface, wx) {
        this._viewContext = viewContext;
        this._app = a;
        this._id = id;
        this._path = path;
        this._viewInterface = viewInterface;
        this._basePath = a.basePath + "/" + func_1.dirname(path);
        (function (component, basePath, appObject) {
            console.info("[COMPONENT] [READYING]");
            var filePath = a.basePath + "/" + path + ".js";
            var fn = compile("(function(Component, getApp, app, page, wx,setInterval,setTimeout,require){" + app.getTextContent(filePath) + "})", filePath)();
            fn(function (object) {
                component._object = object;
            }, function () {
                return a.object;
            }, undefined, undefined, wx, function (fn, v) {
                setInterval(function () {
                    fn.call(component.object);
                }, v);
            }, function (fn, v) {
                setTimeout(function () {
                    fn.call(component.object);
                }, v);
            }, function (path) {
                var p = func_1.relativePath(path, basePath);
                if (!p.endsWith(".js")) {
                    p = p + ".js";
                }
                return require(p);
            });
            component.object.triggerEvent = function (name, detail, option) {
                viewInterface.sendEvent(id, name, {
                    detail: detail,
                    option: option
                });
            };
            component.object.setData = function (data) {
                for (var key in data) {
                    this.data[key] = data[key];
                }
                viewInterface.setData(data, id);
            };
            if (component.object.data === undefined) {
                component.object.data = {};
            }
            viewInterface.setData(component.object.data, id);
        })(this, func_1.dirname(path), this._app.object);
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
    Object.defineProperty(Component.prototype, "object", {
        get: function () {
            return this._object;
        },
        enumerable: true,
        configurable: true
    });
    Component.prototype.remove = function () {
        this.onHide();
        this.onUnload();
    };
    Component.prototype.set = function (name, value) {
    };
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
exports.Component = Component;
