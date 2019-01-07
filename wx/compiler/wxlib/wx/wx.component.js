
var WX = require("wx/wx.js");
var func = require("wx/func.js");

var Component = function (id, path, page, webview, viewContext, wx) {
    this.id = id;
    this.path = path;
    this.page = page;
    this.webview = webview;
    this.object = { data: {} };

    (function (component, basePath) {

        console.info("[PAGE] [READYING]");

        var fn = compile("(function(Component, getApp, app, page, wx,setInterval,setTimeout,require){" + app.getTextContent(path + ".js") + "})", path + ".js")();

        fn(
            function (object) {
                component.object = object;
            },
            function () {
                return __appObject;
            },
            undefined,
            undefined,
            new WX(viewContext, page, app, wx, webview, basePath),
            function (fn, v) {
                setInterval(function () {
                    fn.call(component.object);
                }, v);
            },
            function (fn, v) {
                setTimeout(function () {
                    fn.call(component.object);
                }, v);
            },
            function (path) {
                var p = func.relativePath(path, basePath);
                if (!p.endsWith(".js")) {
                    p = p + ".js";
                }
                return require(p);
            }
        );

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

    })(this, func.dirname(path));

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

};

Component.prototype = Object.create(Object.prototype, {

    onUnload: {
        value: function () {

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
        },
        writable: false
    },

    doEvent: {
        value: function (name, data) {
            var methods = this.object.methods;
            if (methods) {
                var fn = methods[name];
                if (typeof fn == 'function') {
                    fn.call(this.object, data);
                }
            }
        }
    },

    onShow: {
        value: function () {

            var fn = this.object['show'];
            if (typeof fn == 'function') {
                fn.call(this.object);
            }

        }
    },

    onHide: {
        value: function () {
            var fn = this.object['hide'];
            if (typeof fn == 'function') {
                fn.call(this.object);
            }
        }
    }
});

module.exports = Component;
