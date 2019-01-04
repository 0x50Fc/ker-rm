

function WX(viewContext, page, app, wx, webview) {
    this.viewContext = viewContext;
    this.page = page;
    this.app = app;
    this.wx = wx;
    this.webview = webview;
    for (var key in wx) {
        var v = wx[key];
        if (typeof v == 'function') {
            (function (object, fn, key) {
                object[key] = function () {
                    fn.apply(wx, arguments);
                };
            })(this, v, key);
        }
    }
};

WX.prototype = Object.create(Object.prototype, {
    createCanvasContext: {
        value: function (canvasId) {
            return this.viewContext.getCanvasContext(canvasId);
        },
        writable: false
    },
    showModal: {
        value: function (object) {

        },
        writable: false
    },
    showLoading: {
        value: function (object) {

        },
        writable: false
    },
    showActionSheet: {
        value: function (object) {

        },
        writable: false
    },
    hideLoading: {
        value: function (object) {

        },
        writable: false
    },
    showToast: {
        value: function (object) {
            var vs = [];
            for (var key in object) {
                var v = object[key];
                switch (typeof v) {
                    case 'string':
                    case 'number':
                    case 'boolean':
                        vs.push(key + '=' + encodeURIComponent(v + ''));
                        break;
                }
            }
            this.app.open("window:///wx/toast.js?" + vs.join('&'));
            if (typeof object.success == 'function') {
                object.success();
            }
            if (typeof object.complete == 'function') {
                object.complete();
            }
        },
        writable: false
    },
    hideToast: {
        value: function (object) {
            this.app.emit("toast.hide", new Event());
        },
        writable: false
    },
    setNavigationBarTitle: {
        value: function (object) {
            this.page.setOptions({ title: object.title });
            if (typeof object.success == 'function') {
                object.success();
            }
            if (typeof object.complete == 'function') {
                object.complete();
            }
        },
        writable: false
    },
    pageScrollTo: {
        value: function (object) {
            this.webview.setContentOffset(0, object.scrollTop, object.duration === undefined || object.duration > 0);
        },
        writable: false
    },

    previewImage: {
        value: function (object) {
            this.showToast({ title: '[wx.previewImage] 接口未实现' });
            if (typeof object.fail == 'function') {
                object.fail('[wx.previewImage] 接口未实现');
            }
            if (typeof object.complete == 'function') {
                object.complete();
            }
        }, writable: false
    }

});

module.exports = WX;
