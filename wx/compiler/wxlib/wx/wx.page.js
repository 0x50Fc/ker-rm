
function diranme(path) {
    var i = path.lastIndexOf("/");
    if (i >= 0) {
        return path.substr(0, i);
    }
    return path;
}

var V = require("wx/wx.view.js");

require("wx/wx.js");

module.exports = function (options, path, page, padding) {

    console.info("[PAGE]");

    var view = page.view;

    if (padding === undefined) {
        if (options.title === undefined) {
            options.title = app.options.window.navigationBarTitleText;
        }
        if (options.backgroundColor === undefined) {
            options.backgroundColor = app.options.window.backgroundColor;
        }
        page.setOptions(options);
        padding = { top: 64, left: 0, bottom: 0, right: 0 };
        view.set("background-color", options.backgroundColor || "#fff");
    }

    var config = new UIWebViewConfiguration();

    var basePath = diranme(path);

    console.info("[PAGE] [basePath]", basePath);

    config.addUserScript(app.getTextContent("wx/wx.web.js"));
    config.addUserScript("__basePath=" + JSON.stringify(basePath) + ";\n");

    var webview = app.createView("WKWebView", config);

    var context = new V.Context(webview);

    wx.setContext(context);

    view.addSubview(webview);

    webview.setFrame(padding.left, padding.top, page.width - padding.left - padding.right, page.height - padding.top - padding.bottom);
    webview.set("background-color", "#fff");

    (function () {

        var pageObject = {};

        var readying = function () {

            console.info("[PAGE] [READYING]");

            var fn = compile("(function(Page, getApp, app, page, wx,setInterval,setTimeout){" + app.getTextContent(path + ".js") + "})", path + ".wx.js")();

            fn(
                function (object) {
                    pageObject = object;
                },
                function () {
                    return pageObject;
                },
                undefined,
                undefined,
                wx,
                function (fn, v) {
                    setInterval(function () {
                        fn.call(pageObject);
                    }, v);
                },
                function (fn, v) {
                    setTimeout(function () {
                        fn.call(pageObject);
                    }, v);
                }
            );
            pageObject.setData = function (data) {
                for (var key in data) {
                    this.data[key] = data[key];
                }
                webview.evaluateJavaScript("kk.setData(" + JSON.stringify(data) + ");");
            };
            if (pageObject.data === undefined) {
                pageObject.data = {};
            }
            webview.evaluateJavaScript("kk.setData(" + JSON.stringify(pageObject.data) + ");");
            page.on("willAppera", function () {
                var fn = pageObject['onShow'];
                if (typeof fn == 'function') {
                    fn.call(pageObject);
                }
            });
            page.on("didDisappera", function () {
                var fn = pageObject['onHide'];
                if (typeof fn == 'function') {
                    fn.call(pageObject);
                }
            });
            page.on("close", function () {
                var fn = pageObject['onUnload'];
                if (typeof fn == 'function') {
                    fn.call(pageObject);
                }
            });
        }

        webview.on("data", function (event) {
            var data = event.data;
            if (data.page == 'readying') {
                readying();
                var fn = pageObject['onLoad'];
                if (typeof fn == 'function') {
                    fn.call(pageObject, options.query);
                }
                fn = pageObject['onShow'];
                if (typeof fn == 'function') {
                    fn.call(pageObject);
                }
            } else if (data.page == 'ready') {
                var fn = pageObject['onReady'];
                if (typeof fn == 'function') {
                    fn.call(pageObject);
                }
            } else if (data.page == 'open') {
                app.open(basePath + "/" + data.url + ".page.js", true);
            } else if (data.event) {
                var fn = pageObject[data.event];
                if (typeof fn == 'function') {
                    fn.call(pageObject, data.data);
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

    webview.set("#text", '<html style="font-size: ' + rem + 'px"><head><meta charset="utf-8"/><meta name="viewport" content="width=device-width,minimum-scale=1,maximum-scale=1" /><style type="text/css">'
        + app.cssContent + '</style>'
        + content + '</html>');

    return webview;
};
