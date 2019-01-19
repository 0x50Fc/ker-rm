"use strict";
var ker;
(function (ker) {
    function Page(object) {
        var basename = object.path;
        var i = basename.lastIndexOf(".");
        if (i >= 0) {
            basename = basename.substr(0, i);
        }
        if (object.view === undefined) {
            object.view = "view";
        }
        var document = new ker.Document();
        var context = new ker.UIViewContext(app);
        var layouting = false;
        var element = document.createElement("view");
        var data = new ker.Data(page);
        document.rootElement = element;
        var layout = function () {
            if (page.view) {
                context.layout(element);
                context.obtainView(element);
            }
            layouting = false;
        };
        var setLayout = function () {
            if (layouting) {
                return;
            }
            layouting = true;
            setTimeout(layout, 0);
        };
        page.on("resize", function () {
            context.setSize(page.width, page.height);
            element.setFrame(0, 0, page.width, page.height);
            setLayout();
        });
        page.on("unload", function () {
            element.recycleView();
            if (object.onunload !== undefined) {
                object.onunload();
            }
        });
        page.on("ready", function () {
            context.view = page.view;
            element.setFrame(0, 0, page.width, page.height);
            layout();
        });
        document.on("layout", function () {
            setLayout();
        });
        app.exec(basename + "_" + object.view + ".js", {
            document: document,
            element: element,
            data: data
        });
        object.document = document;
        object.data = data;
        if (object.onload !== undefined) {
            object.onload(document, data);
        }
    }
    ker.Page = Page;
})(ker || (ker = {}));
