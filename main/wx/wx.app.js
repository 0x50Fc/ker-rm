
module.exports = function (options) {

    print("[APP]");

    app.object = {};

    (function (__CODE__, App, app) {
        eval(__CODE__);
    })(
        app.getTextContent("app.wx.js"),
        function (object) {
            app.object = object;
        },
        undefined
    );

    app.on("foreground", function () {
        var fn = app.object['onShow'];
        if (typeof fn == 'function') {
            fn.call(app.object);
        }
    });

    app.on("background", function () {
        var fn = app.object['onHide'];
        if (typeof fn == 'function') {
            fn.call(app.object);
        }
    });

    var fn = app.object['onLaunch'];
    if (typeof fn == 'function') {
        fn.call(app.object, options);
    }

};
