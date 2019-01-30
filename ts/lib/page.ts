
namespace ker {

    export interface UIPageObject extends ViewObject {
        [key: string]: any
        path: string
        view?: string
        document?: Document
        data?: DataObject
        setData?: (object: DataObject) => void
        onload?: (document: Document) => void
        onunload?: () => void
    }

    export function Page(object: UIPageObject, page: UIPage, setTimeout: any): void {

        arguments.callee.caller
        var basename = object.path;

        let i = basename.lastIndexOf(".");
        if (i >= 0) {
            basename = basename.substr(0, i);
        }

        if (object.view === undefined) {
            object.view = "view";
        }

        let document = new Document();
        let context = new UIViewContext(app);
        let layouting = false;
        let element = document.createElement("view") as UIViewElement;
        let data = new Data(object);

        if (object.data !== undefined) {
            data.object = object.data
        }

        document.rootElement = element;

        let layout = function () {
            layouting = false;
            if (page.view) {
                context.layout(element);
                context.obtainView(element);
            }
        };

        let setLayout = function () {

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

            element.recycle();

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

        page.on("data", (e: Event, name: string): void => {
            let v = e.data;
            if (typeof v == 'object') {
                data.setData(v);
                setLayout();
            }
        });

        document.on("layout", function () {
            setLayout();
        });

        View(document, object, (V: ViewElementFuntion, E: ViewEvaluateFuntion): void => {

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
        object.setData = (object: DataObject): void => {
            data.setData(object);
            setLayout();
        };

        if (object.onload !== undefined) {
            object.onload(document);
        }
    }

}
