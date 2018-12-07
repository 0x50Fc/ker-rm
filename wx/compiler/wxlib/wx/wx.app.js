
__appObject = {};
__appOptions = {};
__cssContext = '';

module.exports = function (options) {

    print("[APP]");

    __appOptions = options;
    __cssContext = app.getTextContent("app.css");

    (function (__CODE__, App, app) {
        eval(__CODE__);
    })(
        app.getTextContent("app.wx.js"),
        function (object) {
            __appObject = object;
        },
        undefined
    );

    app.on("foreground", function () {
        var fn = __appObject['onShow'];
        if (typeof fn == 'function') {
            fn.call(__appObject);
        }
    });

    app.on("background", function () {
        var fn = __appObject['onHide'];
        if (typeof fn == 'function') {
            fn.call(__appObject);
        }
    });

    var fn = __appObject['onLaunch'];
    if (typeof fn == 'function') {
        fn.call(__appObject, options);
    }

    if (options && options.tabBar) {
        app.open("wx/tabbar.js", true);
    } else {
        app.open(options.pages[0] + '.page.js', true);
    }

};
