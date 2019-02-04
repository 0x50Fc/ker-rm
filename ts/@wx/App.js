"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var wx = require("./wx");
var NavigationBarTextStyle;
(function (NavigationBarTextStyle) {
    NavigationBarTextStyle["White"] = "white";
    NavigationBarTextStyle["Black"] = "black";
})(NavigationBarTextStyle = exports.NavigationBarTextStyle || (exports.NavigationBarTextStyle = {}));
var NavigationStyle;
(function (NavigationStyle) {
    NavigationStyle["Default"] = "default";
    NavigationStyle["Custom"] = "custom";
})(NavigationStyle = exports.NavigationStyle || (exports.NavigationStyle = {}));
var BackgroundTextStyle;
(function (BackgroundTextStyle) {
    BackgroundTextStyle["Dark"] = "dark";
    BackgroundTextStyle["Light"] = "light";
})(BackgroundTextStyle = exports.BackgroundTextStyle || (exports.BackgroundTextStyle = {}));
var PageOrientation;
(function (PageOrientation) {
    PageOrientation["Portrait"] = "portrait";
    PageOrientation["Auto"] = "auto";
    PageOrientation["Landscape"] = "landscape";
})(PageOrientation = exports.PageOrientation || (exports.PageOrientation = {}));
var TabBarBorderStyle;
(function (TabBarBorderStyle) {
    TabBarBorderStyle["Black"] = "black";
    TabBarBorderStyle["White"] = "white";
})(TabBarBorderStyle = exports.TabBarBorderStyle || (exports.TabBarBorderStyle = {}));
var TabBarPosition;
(function (TabBarPosition) {
    TabBarPosition["Bottom"] = "bottom";
    TabBarPosition["Top"] = "top";
})(TabBarPosition = exports.TabBarPosition || (exports.TabBarPosition = {}));
var RefCount = /** @class */ (function () {
    function RefCount(object) {
        this.object = object;
        this._retainCount = 0;
    }
    RefCount.prototype.retain = function () {
        this._retainCount++;
    };
    RefCount.prototype.release = function () {
        this._retainCount--;
        if (this._retainCount == 0) {
            this.object.recycle();
        }
    };
    return RefCount;
}());
var autoId = 0;
var appSet = {};
var App = /** @class */ (function () {
    function App(basePath) {
        this.appid = ++autoId;
        this.basePath = basePath;
        {
            var v = app.getTextContent(basePath + "/app.json");
            if (v !== undefined) {
                this.options = JSON.parse(v);
            }
        }
        {
            this.cssContent = app.getTextContent(basePath + "/app.css") || '';
        }
    }
    Object.defineProperty(App.prototype, "object", {
        get: function () {
            return this._object;
        },
        enumerable: true,
        configurable: true
    });
    App.prototype.open = function (path, query) {
        app.open("@wx/page/page.js?appid=" + this.appid + "&path=" + encodeURIComponent(path) + "&query=" + encodeURIComponent(JSON.stringify(query)), true);
    };
    App.prototype.run = function (path, query) {
        var _this = this;
        if (appSet[this.appid] !== undefined) {
            throw "小程序已运行";
        }
        var librarySet = {
            getApp: function () {
                return _this._object;
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
            require: function (path) {
                return require(_this.basePath + "/" + path);
            },
            wx: wx
        };
        var keys = ["App"];
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
            var v = app.getTextContent(this.basePath + "/app.js");
            if (v !== undefined) {
                code.push(v);
            }
        }
        code.push("\n})");
        var fn = compile(code.join(''), "app.js")();
        fn.apply(undefined, values);
        appSet[this.appid] = new RefCount(this);
        if (this.options.tabBar !== undefined) {
            app.open("@wx/page/tabbar.js?appid=" + this.appid, path === undefined);
        }
        else {
            if (path === undefined) {
                if (this.options.pages.length > 0) {
                    path = this.options.pages[0];
                }
            }
            if (query === undefined) {
                query = {};
            }
            if (path === undefined) {
                delete appSet[this.appid];
                throw "未找到小程序页面";
            }
            app.open("@wx/page/page.js?appid=" + this.appid
                + "&path=" + encodeURIComponent(path)
                + "&query=" + encodeURIComponent(JSON.stringify(query)), true);
        }
    };
    App.prototype.recycle = function () {
        delete appSet[this.appid];
    };
    return App;
}());
exports.App = App;
function getApp(id) {
    return appSet[id];
}
exports.getApp = getApp;
