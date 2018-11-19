
function diranme(path) {
    var i = path.lastIndexOf("/");
    if (i >= 0) {
        return path.substr(0, i);
    }
    return path;
}

var V = require("wx/wx.view.js");
var wx = require("wx/wx.js");

module.exports = function (options, path, page, padding) {

    print("[PAGE]");

    var view = page.view;

    page.setOptions(options);

    if (padding === undefined) {
        padding = { top: 64, left: 0, bottom: 0, right: 0 };
        view.set("background-color", "#fff");
    }

    var config = new UIWebViewConfiguration();

    var basePath = diranme(options.path);

    config.addUserScript(app.getTextContent("wx/wx.web.js"));
    config.addUserScript("__basePath=" + JSON.stringify(basePath) + ";\n");

    var webview = app.createView("WKWebView", config);

    var context = new V.Context(webview);

    wx.setContext(context);

    view.addSubview(webview);

    webview.setFrame(padding.left, padding.top, page.width - padding.left - padding.right, page.height - padding.top - padding.bottom);
    webview.set("background-color", "#fff");

    (function () {

        page.object = {};

        var readying = function () {

            var fn = compile("(function(Page, getApp, app, page, wx){" + app.getTextContent(path + ".js") + "})", path + ".wx.js")();

            fn(
                function (object) {
                    page.object = object;
                },
                function () {
                    return app.object;
                },
                undefined,
                undefined,
                wx
            );
            Object.defineProperty(page.object, "setData", {
                value: function (data) {
                    webview.evaluateJavaScript("kk.setData(" + JSON.stringify(data) + ");");
                },
                writable: false,
                configurable: false,
                enumerable: true
            });
            if (page.object.data === undefined) {
                page.object.data = {};
            }
            webview.evaluateJavaScript("kk.setData(" + JSON.stringify(page.object.data) + ");");
            page.on("willAppera", function () {
                var fn = page.object['onShow'];
                if (typeof fn == 'function') {
                    fn.call(page.object);
                }
            });
            page.on("didDisappera", function () {
                var fn = page.object['onHide'];
                if (typeof fn == 'function') {
                    fn.call(page.object);
                }
            });
            page.on("close", function () {
                var fn = page.object['onUnload'];
                if (typeof fn == 'function') {
                    fn.call(page.object);
                }
            });
        }

        webview.on("data", function (event) {
            var data = event.data;
            if (data.page == 'readying') {
                readying();
                var fn = page.object['onLoad'];
                if (typeof fn == 'function') {
                    fn.call(page.object, options.query);
                }
                fn = page.object['onShow'];
                if (typeof fn == 'function') {
                    fn.call(page.object);
                }
            } else if (data.page == 'ready') {
                var fn = page.object['onReady'];
                if (typeof fn == 'function') {
                    fn.call(page.object);
                }
            } else if (data.event) {
                var fn = page.object[data.event];
                if (typeof fn == 'function') {
                    fn.call(page.object, data.data);
                }
            } else if (data.view == 'create') {
                context.create(data.name, data.id);
            } else if (data.view == 'set') {
                context.set(data.id, data.name, data.value);
            } else if (data.view == 'setFrame') {
                context.setFrame(data.id, data.x, data.y, data.width, data.height);
            } else if (data.view == 'setContentSize') {
                context.setContentSize(data.id, data.width, data.height);
            } else if (data.view == 'add') {
                context.add(data.id, data.pid);
            } else if (data.view == 'remove') {
                context.remove(data.id);
            } else if (data.view == 'on') {

                context.on(data.id, data.name, function (e) {
                    console.info(typeof e.data);
                    webview.evaluateJavaScript("kk.sendEvent(" + data.id + "," + JSON.stringify(data.name) + "," + JSON.stringify(e.data) + ");");
                });

            } else if (data.view == 'off') {

                context.on(data.id, data.name);

            }
        });

    })();

    var content = app.getTextContent(path + ".wx.html");
    var rem = page.width * 20 / 750.0;

    webview.set("#text", '<html style="font-size: ' + rem + 'px">' + content + '</html>');

};
