var ker;
(function (ker) {
    function Page(object, page, setTimeout) {
        arguments.callee.caller;
        var basename = object.path;
        var i = basename.lastIndexOf(".");
        if (i >= 0) {
            basename = basename.substr(0, i);
        }
        if (object.view === undefined) {
            object.view = "view";
        }
        var document = new Document();
        var context = new UIViewContext(app);
        var layouting = false;
        var element = document.createElement("view");
        var data = new ker.Data(object);
        if (object.data !== undefined) {
            data.object = object.data;
        }
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
            context.page = page;
            context.view = page.view;
            element.setFrame(0, 0, page.width, page.height);
            layout();
        });
        page.on("data", function (e, name) {
            var v = e.data;
            if (typeof v == 'object') {
                data.setData(v);
                setLayout();
            }
        });
        document.on("layout", function () {
            setLayout();
        });
        ker.View(document, object, function (V, E) {
            app.exec(basename + "_" + object.view + ".js", {
                element: element,
                data: data,
                V: V,
                E: E
            });
        });
        data.changeKeys();
        setLayout();
        console.info(element.toString());
        object.document = document;
        object.data = data.object;
        object.setData = function (object) {
            data.setData(object);
            setLayout();
        };
        if (object.onload !== undefined) {
            object.onload(document);
        }
    }
    ker.Page = Page;
})(ker || (ker = {}));
