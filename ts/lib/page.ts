

namespace ker {

    export function Page(object: UIPageObject): void {

        var basename = object.path;

        let i = basename.lastIndexOf(".");
        if (i >= 0) {
            basename = basename.substr(0, i);
        }

        if (object.view === undefined) {
            object.view = "view";
        }

        var document = new Document();
        var context = new UIViewContext(app);
        var layouting = false;
        var element = document.createElement("view") as UIViewElement;
        var data = new Data(page);

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
            element: element,
            data: data,
            V : (element: Element, data: Data, name: string): void => {

            },
            E : (func:EvaluateScript,keys:string[]):Evaluate=>{
                return new Evaluate(func,keys);
            }
        });

        object.document = document;
        object.data = data;
        if (object.onload !== undefined) {
            object.onload(document, data);
        }
    }

}
