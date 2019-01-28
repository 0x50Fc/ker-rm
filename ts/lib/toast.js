var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    }
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
var ker;
(function (ker) {
    var ToastView = /** @class */ (function (_super) {
        __extends(ToastView, _super);
        function ToastView() {
            return _super !== null && _super.apply(this, arguments) || this;
        }
        ToastView.prototype.create = function (object) {
            _super.prototype.create.call(this, object, function (element, data, V, E) {
                V(element, data, "body", {
                    'max-width': '400rpx',
                    'padding': '20rpx',
                    'background-color': 'rgba(0,0,0,0.75)',
                    'border-radius': '8rpx'
                }, function (element, data) {
                    V(element, data, "text", {
                        '#text': E(function (title) { return title; }, ['title']),
                        'font': '28rpx',
                        'color': '#fff'
                    }, function (element, data) { });
                });
            });
        };
        return ToastView;
    }(ker.Dialog));
    var audoId = 0;
    var viewSet = {};
    function showToast(object) {
        if (object.duration === undefined) {
            object.duration = 1500;
        }
        var id = (++audoId) + '';
        var view = new ToastView();
        view.create({});
        view.setData({
            title: object.title
        });
        view.show();
        viewSet[id] = view;
        setTimeout(function () {
            view.hide();
            view.recycle();
            delete viewSet[id];
        }, object.duration);
        if (object.success !== undefined) {
            object.success();
        }
        if (object.complete !== undefined) {
            object.complete();
        }
    }
    ker.showToast = showToast;
    function hideToast(object) {
        var view;
        var id = 0;
        for (var key in viewSet) {
            if (parseInt(key) > id) {
                id = parseInt(key);
                view = viewSet[key];
            }
        }
        if (view !== undefined) {
            view.hide();
            view.recycle();
            delete viewSet[id + ''];
        }
        if (object.success !== undefined) {
            object.success();
        }
        if (object.complete !== undefined) {
            object.complete();
        }
    }
    ker.hideToast = hideToast;
})(ker || (ker = {}));
