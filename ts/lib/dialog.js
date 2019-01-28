var ker;
(function (ker) {
    var Dialog = /** @class */ (function () {
        function Dialog() {
            var _this = this;
            this._data = new ker.Data(global);
            this._view = app.createView("view");
            this._viewContext = new UIViewContext(app);
            this._viewContext.view = this._view;
            this._document = new Document();
            this._element = this._document.createElement("layout");
            this._document.rootElement = this._element;
            this._viewElement = this._document.createElement("view");
            this._element.append(this._viewElement);
            this._layouting = false;
            this._onLayout = function (event) {
                _this.setLayout();
            };
            screen.on("change", this._onLayout);
        }
        Dialog.prototype.recycle = function () {
            screen.off("change", this._onLayout);
            this._data.off([]);
            this._viewElement.recycleView();
            this._view.removeView();
        };
        Dialog.prototype.layout = function () {
            this._viewContext.setSize(screen.width, screen.height);
            this._viewContext.setUnit("px", screen.density, 0);
            this._element.setFrame(0, 0, screen.width, screen.height);
            this._viewContext.layout(this._element);
            this._viewContext.obtainView(this._viewElement);
            this._layouting = false;
        };
        Dialog.prototype.setLayout = function () {
            var _this = this;
            if (this._layouting) {
                return;
            }
            this._layouting = true;
            setTimeout(function () {
                _this.layout();
            }, 0);
        };
        Dialog.prototype.setData = function (data) {
            this._data.setData(data);
            this.setLayout();
        };
        Object.defineProperty(Dialog.prototype, "view", {
            get: function () {
                return this._view;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(Dialog.prototype, "document", {
            get: function () {
                return this._document;
            },
            enumerable: true,
            configurable: true
        });
        Dialog.prototype.create = function (object, func) {
            var _this = this;
            ker.View(this._document, object, function (V, E) {
                func(_this._viewElement, _this._data, V, E);
            });
            this.setLayout();
        };
        Dialog.prototype.open = function (path, object) {
            var _this = this;
            ker.View(this._document, object, function (V, E) {
                app.exec(path, {
                    element: _this._viewElement,
                    data: _this._data,
                    V: V,
                    E: E
                });
            });
            this.setLayout();
        };
        Dialog.prototype.show = function () {
            this._view.showToScreen();
        };
        Dialog.prototype.hide = function () {
            this._view.removeView();
        };
        return Dialog;
    }());
    ker.Dialog = Dialog;
})(ker || (ker = {}));
