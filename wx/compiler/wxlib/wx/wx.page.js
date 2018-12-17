
var V = require("wx/wx.view.js");
var WX = require("wx/wx.js");
var Component = require("wx/wx.component.js");
var func = require("wx/func.js");

module.exports = function (options, path, page, app, padding) {

    console.info("[PAGE]");

    var view = page.view;

    if (padding === undefined) {
        if (options.title === undefined) {
            options.title = __appOptions.window.navigationBarTitleText;
        }
        if (options.backgroundColor === undefined) {
            options.backgroundColor = __appOptions.window.backgroundColor;
        }
        page.setOptions(options);
        padding = { top: 64, left: 0, bottom: 0, right: 0 };

        if (platform == 'Android') {
            padding.top = 44;
        }

        if (typeof screen == 'object' && screen.density !== undefined) {
            padding.top = padding.top * screen.density;
            padding.left = padding.left * screen.density;
            padding.bottom = padding.bottom * screen.density;
            padding.right = padding.right * screen.density;
        }

        view.set("background-color", options.backgroundColor || "#fff");
    }

    var config = new UIWebViewConfiguration();

    var basePath = func.dirname(path);

    console.info("[PAGE] [basePath]", basePath);

    config.addUserScript("__basePath='';\n");
    config.addUserScript(app.getTextContent("wx/wx.web.js"));

    var webview = app.createView("WKWebView", config);

    var context = new V.Context(webview);

    var components = {};

    view.addSubview(webview);

    var resize = function(){
        webview.setFrame(padding.left, padding.top, page.width - padding.left - padding.right, page.height - padding.top - padding.bottom);
    };
    
    page.on("resize",resize);

    resize();

    webview.set("background-color", "#fff");

    (function () {

        var pageObject = {};

        var readying = function () {

            console.info("[PAGE] [READYING]");

            var fn = compile("(function(Page, getApp, app, page, wx,setInterval,setTimeout,require){" + app.getTextContent(path + ".js") + "})", path + ".js")();

            fn(
                function (object) {
                    pageObject = object;
                },
                function () {
                    return __appObject;
                },
                undefined,
                undefined,
                new WX(context, page, app, wx, webview),
                function (fn, v) {
                    setInterval(function () {
                        fn.call(pageObject);
                    }, v);
                },
                function (fn, v) {
                    setTimeout(function () {
                        fn.call(pageObject);
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
                for (var id in components) {
                    var v = components[id];
                    v.onShow();
                }
            });
            page.on("didDisappera", function () {
                var fn = pageObject['onHide'];
                if (typeof fn == 'function') {
                    fn.call(pageObject);
                }
                for (var id in components) {
                    var v = components[id];
                    v.onHide();
                }
            });
            page.on("close", function () {
                var fn = pageObject['onUnload'];
                if (typeof fn == 'function') {
                    fn.call(pageObject);
                }
                for (var id in components) {
                    var v = components[id];
                    v.onUnload();
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
                if (data.componentId) {
                    var v = components[data.componentId];
                    if (v) {
                        v.doEvent(data.event, data.data);
                    }
                } else {
                    var fn = pageObject[data.event];
                    if (typeof fn == 'function') {
                        fn.call(pageObject, data.data);
                    }
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
                    webview.evaluateJavaScript("kk.sendEvent(" + data.id + "," + JSON.stringify(data.name) + "," + JSON.stringify(e.data) + ");");
                });

            } else if (data.view == 'off') {

                context.on(data.id, data.name);

            } else if (data.component == 'add') {
                components[data.id] = new Component(data.id, data.path, page, webview, context, wx);
            } else if (data.component == 'remove') {
                var v = components[data.id];
                if (v) {
                    v.remove();
                    delete components[data.id];
                }
            } else if (data.component == 'set') {
                var v = components[data.id];
                if (v) {
                    v.set(data.name, data.value);
                }
            }
        });

    })();

    var content = app.getTextContent(path + ".wx.html");
    var rem = page.width * 20 / 750.0;

    if (typeof screen == 'object' && screen.density !== undefined) {
        rem = (screen.width / screen.density) * 20 / 750.0;
        console.info("[REM]", rem, screen);
    }

    webview.setContent('<html style="font-size: ' + rem + 'px"><head><meta charset="utf-8"/><meta name="viewport" content="width=device-width,minimum-scale=1,maximum-scale=1" /><style type="text/css">'
        + __cssContext + '</style>'
        + content + '</html>', undefined, basePath);

    return webview;
};
