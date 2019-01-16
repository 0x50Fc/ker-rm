

exports.create = function (page) {

    var document = new Document();
    var context = new UIViewContext(app);
    var layouting = false;
    var element = document.createElement("view");

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

        if (typeof screen == 'object') {
            context.setSize(screen.width, screen.height);
        } else {
            context.setSize(page.width, page.height);
        }

        element.setFrame(0, 0, page.width, page.height);

        setLayout();

    });

    page.on("ready", function () {

        context.view = page.view;

        element.setFrame(0, 0, page.width, page.height);

        layout();
    });

    document.on("layout",function(){
        setLayout();
    });

    document.layout = function () {
        setLayout();
    };

    return document;
};

