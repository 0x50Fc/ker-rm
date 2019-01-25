var ker;
(function (ker) {
    var ToastView = /** @class */ (function () {
        function ToastView() {
            var _this = this;
            this._data = new ker.Data(global);
            this._view = app.createView("view");
            this._viewContext = new UIViewContext(app);
            this._viewContext.view = this._view;
            this._document = new Document();
            this._element = this._document.createElement("layout");
            this._document.rootElement = this._element;
            ker.View(this._document, {}, function (V, E) {
                V(_this._element, _this._data, "view", {
                    'max-width': '400rpx',
                    'padding': '20rpx',
                    'background-color': 'rgba(0,0,0,0.75)',
                    'border-radius': '8rpx'
                }, function (element, data) {
                    _this._viewElement = element;
                    V(element, data, "text", {
                        '#text': E(function (title) { return title; }, ['title']),
                        'font': '28rpx',
                        'color': '#fff'
                    }, function (element, data) { });
                });
            });
        }
        ToastView.prototype.layout = function () {
            this._viewContext.setSize(screen.width, screen.height);
            this._viewContext.setUnit("px", screen.density, 0);
            this._element.setFrame(0, 0, screen.width, screen.height);
            this._viewContext.layout(this._element);
            this._viewContext.obtainView(this._viewElement);
        };
        ToastView.prototype.setData = function (data) {
            this._data.setData(data);
            this.layout();
        };
        Object.defineProperty(ToastView.prototype, "view", {
            get: function () {
                return this._view;
            },
            enumerable: true,
            configurable: true
        });
        return ToastView;
    }());
    var views = [];
    screen.on("change", function (event) {
        for (var _i = 0, views_1 = views; _i < views_1.length; _i++) {
            var v = views_1[_i];
            v.layout();
        }
    });
    function showToast(object) {
    }
    ker.showToast = showToast;
    function hideToast(object) {
        if (views.length > 0) {
            var view = views.pop();
            app.hideView;
        }
    }
    ker.hideToast = hideToast;
})(ker || (ker = {}));
