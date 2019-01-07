

function WX(viewContext, page, app, wx, webview, basePath) {
    this.viewContext = viewContext;
    this.page = page;
    this.app = app;
    this.wx = wx;
    this.webview = webview;
    this._basePath = basePath;
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
    },

    getStorageInfoSync: {
        value: function () {
            return {
                keys: app.storage.keys()
            }
        }, writable: false
    },

    getStorageInfo: {
        value: function (object) {
            app.storage.loadKeys(function (keys) {
                if (typeof object.success == 'function') {
                    object.success({ keys: keys });
                }
                if (typeof object.complete == 'function') {
                    object.complete();
                }
            });
        }, writable: false
    },

    clearStorageSync: {
        value: function () {
            app.storage.clear();
        }, writable: false
    },

    clearStorage: {
        value: function (object) {
            app.storage.clear();
            setTimeout(function () {
                if (typeof object.success == 'function') {
                    object.success({});
                }
                if (typeof object.complete == 'function') {
                    object.complete();
                }
            }, 0);
        }, writable: false
    },

    removeStorageSync: {
        value: function (key) {
            app.storage.set(key, undefined);
        }, writable: false
    },
    removeStorage: {
        value: function (object) {
            app.storage.set(object.key, undefined);
            setTimeout(function () {
                if (typeof object.success == 'function') {
                    object.success({});
                }
                if (typeof object.complete == 'function') {
                    object.complete();
                }
            }, 0);
        }, writable: false
    },

    setStorageSync: {
        value: function (key, value) {
            app.storage.set(key, JSON.stringify(value));
        }, writable: false
    },

    setStorage: {
        value: function (object) {
            app.storage.set(object.key, JSON.stringify(object.data));
            setTimeout(function () {
                if (typeof object.success == 'function') {
                    object.success({});
                }
                if (typeof object.complete == 'function') {
                    object.complete();
                }
            }, 0);
        }, writable: false
    },

    getStorageSync: {
        value: function (key) {
            var v = app.storage.get(key);
            if (typeof v == 'string' && v != '') {
                return JSON.parse(v);
            }
        }, writable: false
    },

    getStorage: {
        value: function (object) {
            app.storage.load(key, function (value) {
                if (typeof object.success == 'function') {
                    if (typeof value == 'string' && value != '') {
                        object.success({ data: JSON.parse(value) });
                    } else {
                        object.success({});
                    }

                }
                if (typeof object.complete == 'function') {
                    object.complete();
                }
            });
        }, writable: false
    },

    navigateTo: {
        value: function (object) {
            if (object.url) {
                app.open(this._basePath + "/" + object.url + ".page.js", true);
                setTimeout(function () {
                    if (typeof object.success == 'function') {
                        object.success({});
                    }
                    if (typeof object.complete == 'function') {
                        object.complete();
                    }
                }, 0);
            }
        }, writable: false
    },

    createWorker : {
        value: function (path) {
            return app.createWorker(path);
        }, writable: false
    }


});

module.exports = WX;
