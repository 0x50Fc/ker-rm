

exports.create = function (page) {

    var document = new Document();
    var context = new UIViewContext(app);
    var layouting = false;
    document.rootElement = document.createElement("view");

    var layout = function() {
        var e = document.rootElement;
        if (e !== undefined && page.view) {
            context.layout(e);
        }
        layouting = false;
    };

    var setLayout = function() {
        
        if(layouting) {
            return;
        }
        layouting = true;
        setTimeout(layout,0);
    };

    page.on("resize", function () {

        if (typeof screen == 'object') {
            context.setSize(screen.width, screen.height);
        } else {
            context.setSize(page.width, page.height);
        }

        var e = document.rootElement;

        if (e !== undefined) {
            print(typeof e.setFrame);
            e.setFrame(0, 0, page.width, page.height);
        }

        setLayout();

    });

    page.on("ready", function () {

        context.view = page.view;

        var e = document.rootElement;

        if (e !== undefined) {
            e.setFrame(0, 0, page.width, page.height);
        }

        layout();
    });

    document.layout = function() {
        setLayout();
    };

    return document;
};

