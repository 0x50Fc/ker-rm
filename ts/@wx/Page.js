"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var Component_1 = require("./Component");
var PageInterface_1 = require("./PageInterface");
var ViewContext_1 = require("./ViewContext");
var func_1 = require("./func");
var BackgroundTextStyle;
(function (BackgroundTextStyle) {
    BackgroundTextStyle["Dark"] = "dark";
    BackgroundTextStyle["Light"] = "light";
})(BackgroundTextStyle = exports.BackgroundTextStyle || (exports.BackgroundTextStyle = {}));
var Page = /** @class */ (function () {
    function Page(app, viewInterface) {
        this.app = app;
        this._components = {};
    }
    Object.defineProperty(Page.prototype, "object", {
        get: function () {
            return this._object;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Page.prototype, "options", {
        get: function () {
            return this._options;
        },
        enumerable: true,
        configurable: true
    });
    Page.prototype.run = function (viewInterface, path, query) {
        var _this = this;
        {
            var v = app.getTextContent(this.app.basePath + "/" + path + ".json");
            if (v !== undefined) {
                this._options = JSON.parse(v);
            }
        }
        var wx = new PageInterface_1.PageInterface(this.app, func_1.dirname(path));
        var librarySet = {
            getApp: function () {
                return _this.app.object;
            },
            setTimeout: function (fn, tv) {
                return setTimeout(function () {
                    fn.apply(_this);
                }, tv);
            },
            setInterval: function (fn, tv) {
                return setInterval(function () {
                    fn.apply(_this);
                }, tv);
            },
            clearTimeout: function (id) {
                clearTimeout(id);
            },
            clearInterval: function (id) {
                clearInterval(id);
            },
            wx: wx
        };
        var keys = ["Page"];
        var values = [
            function (object) {
                _this._object = object;
            }
        ];
        for (var key in librarySet) {
            keys.push(key);
            values.push(librarySet[key]);
        }
        var code = ["(function("];
        code.push(keys.join(","));
        code.push("){\n");
        {
            var v = app.getTextContent(this.app.basePath + "/" + path + ".js");
            if (v !== undefined) {
                code.push(v);
            }
        }
        code.push("\n})");
        var fn = compile(code.join(''), this.app.basePath + "/" + path + ".js")();
        fn.apply(undefined, values);
        if (this._object.data === undefined) {
            this._object.data = {};
        }
        this._object.setData = function (object) {
            viewInterface.setData(object);
        };
        var viewContext = new ViewContext_1.ViewContext(viewInterface);
        viewInterface.ondata = function (data) {
            if (data.page == 'readying') {
                if (_this._object.onLoad !== undefined) {
                    _this._object.onLoad(query);
                }
                if (_this._object.onShow !== undefined) {
                    _this._object.onShow();
                }
                viewInterface.setData(_this._object.data);
            }
            else if (data.page == 'ready') {
                if (_this._object.onReady !== undefined) {
                    _this._object.onReady();
                }
            }
            else if (data.page == 'open') {
                wx.navigateTo({
                    url: data.url
                });
            }
            else if (data.event) {
                if (data.componentId) {
                    var v = _this._components[data.componentId];
                    if (v) {
                        v.doEvent(data.event, data.data);
                    }
                }
                else {
                    var fn = _this._object[data.event];
                    if (typeof fn == 'function') {
                        fn.call(_this._object, data.data);
                    }
                }
            }
            else if (data.view == 'create') {
                viewContext.create(data.name, data.id);
            }
            else if (data.view == 'set') {
                viewContext.set(data.id, data.name, data.value);
            }
            else if (data.view == 'setFrame') {
                viewContext.setFrame(data.id, data.x, data.y, data.width, data.height);
            }
            else if (data.view == 'setContentSize') {
                viewContext.setContentSize(data.id, data.width, data.height);
            }
            else if (data.view == 'add') {
                viewContext.add(data.id, data.pid);
            }
            else if (data.view == 'remove') {
                viewContext.remove(data.id);
            }
            else if (data.view == 'on') {
                viewContext.on(data.id, data.name, function (e) {
                    viewInterface.sendEvent(data.id, data.name, e.data);
                });
            }
            else if (data.view == 'off') {
                viewContext.off(data.id, data.name);
            }
            else if (data.component == 'add') {
                _this._components[data.id] = new Component_1.Component(_this.app, data.id, data.path, viewContext, viewInterface, wx);
            }
            else if (data.component == 'remove') {
                var v = _this._components[data.id];
                if (v) {
                    v.remove();
                    delete _this._components[data.id];
                }
            }
            else if (data.component == 'set') {
                var v = _this._components[data.id];
                if (v) {
                    v.set(data.name, data.value);
                }
            }
        };
        var p = this.app.basePath + "/" + path + ".wx.html";
        var content = app.getTextContent(p);
        var rem = (screen.width / screen.density) * 20 / 750.0;
        console.info("[REM]", rem, screen);
        var vs = [
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
        viewInterface.setHTMLContent(vs.join(''), func_1.dirname(p) + '/');
    };
    Page.prototype.recycle = function () {
        for (var id in this._components) {
            var v = this._components[id];
            v.onUnload();
        }
        if (this._object.onUnload !== undefined) {
            this._object.onUnload();
        }
    };
    Page.prototype.show = function () {
        if (this._object.onShow !== undefined) {
            this._object.onShow();
        }
        for (var id in this._components) {
            var v = this._components[id];
            v.onShow();
        }
    };
    Page.prototype.hide = function () {
        for (var id in this._components) {
            var v = this._components[id];
            v.onHide();
        }
        if (this._object.onHide !== undefined) {
            this._object.onHide();
        }
    };
    return Page;
}());
exports.Page = Page;
