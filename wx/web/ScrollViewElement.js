"use strict";
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
Object.defineProperty(exports, "__esModule", { value: true });
var ViewElement_1 = require("./ViewElement");
var V_1 = require("./V");
var Anim_1 = require("./Anim");
var ScrollViewElement = /** @class */ (function (_super) {
    __extends(ScrollViewElement, _super);
    function ScrollViewElement(document, name, id) {
        var _this = _super.call(this, document, name, id) || this;
        _this._scrollAnimateId = 0;
        _this._upperThreshold = 50;
        _this._lowerThreshold = 50;
        _this._enabledScrollX = false;
        _this._enabledScrollY = false;
        _this._thgresholdin = false;
        _this._scrollWithAnimation = false;
        var element = _this;
        var delta = {
            x: 0,
            y: 0,
            deltaX: 0,
            deltaY: 0
        };
        _this._view.addEventListener("scroll", function (event) {
            element.onScroll(element.scrollLeft, element.scrollTop, event);
            delta.deltaX = element.scrollLeft - delta.x;
            delta.deltaY = element.scrollTop - delta.y;
            delta.x = element.scrollLeft;
            delta.y = element.scrollTop;
            element.doEvent(event, "scroll", {
                scrollLeft: element.scrollLeft,
                scrollTop: element.scrollTop,
                scrollWidth: element.scrollWidth,
                scrollHeight: element.scrollHeight,
                deltaX: delta.deltaX,
                deltaY: delta.deltaY
            });
            event.stopPropagation();
        });
        return _this;
    }
    ScrollViewElement.prototype.doEvent = function (event, name, detail) {
        _super.prototype.doEvent.call(this, event, name, detail);
    };
    ScrollViewElement.prototype.scrollTo = function (x, y, animated) {
        if (this._scrollAnimateId) {
            Anim_1.clearAnimation(this._scrollAnimateId);
            this._scrollAnimateId = 0;
        }
        if (animated) {
            var view_1 = this._view;
            var x0_1 = view_1.scrollLeft;
            var y0_1 = view_1.scrollTop;
            var dx_1 = x - x0_1;
            var dy_1 = y - y0_1;
            this._scrollAnimateId = Anim_1.startAnimation(300, function (v) {
                view_1.scrollTo(x0_1 + dx_1 * v, y0_1 + dy_1 * v);
            });
        }
        else {
            this._view.scrollTo(x, y);
        }
    };
    ScrollViewElement.prototype.onScroll = function (x, y, event) {
        var sBottom = this._view.scrollHeight - this._view.clientHeight;
        var sRight = this._view.scrollWidth - this._view.clientWidth;
        if (this._enabledScrollY) {
            if (y < this._upperThreshold) {
                if (!this._thgresholdin) {
                    this._thgresholdin = true;
                    this.doEvent(event, "scrolltoupper", {});
                    ;
                }
            }
            else if (y > sBottom - this._lowerThreshold) {
                if (!this._thgresholdin) {
                    this._thgresholdin = true;
                    this.doEvent(event, "scrolltolower", {});
                    ;
                }
            }
            else {
                this._thgresholdin = false;
            }
        }
        else if (this._enabledScrollX) {
            if (x < this._upperThreshold) {
                if (!this._thgresholdin) {
                    this._thgresholdin = true;
                    this.doEvent(event, "scrolltoupper", {});
                    ;
                }
            }
            else if (x > sRight - this._lowerThreshold) {
                if (!this._thgresholdin) {
                    this._thgresholdin = true;
                    this.doEvent(event, "scrolltolower", {});
                    ;
                }
            }
            else {
                this._thgresholdin = false;
            }
        }
    };
    Object.defineProperty(ScrollViewElement.prototype, "scrollLeft", {
        get: function () {
            return this._view.scrollLeft;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ScrollViewElement.prototype, "scrollTop", {
        get: function () {
            return this._view.scrollTop;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ScrollViewElement.prototype, "scrollWidth", {
        get: function () {
            return this._view.scrollWidth;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ScrollViewElement.prototype, "scrollHeight", {
        get: function () {
            return this._view.scrollHeight;
        },
        enumerable: true,
        configurable: true
    });
    ScrollViewElement.prototype.set = function (key, value) {
        _super.prototype.set.call(this, key, value);
        if (key == 'scroll-x') {
            if (V_1.booleanValue(value)) {
                this._view.style.overflowX = 'auto';
                this._enabledScrollX = true;
            }
            else {
                this._view.style.overflowX = 'hidden';
                this._enabledScrollX = false;
            }
        }
        else if (key == 'scroll-y') {
            if (V_1.booleanValue(value)) {
                this._view.style.overflowY = 'auto';
                this._enabledScrollY = true;
            }
            else {
                this._view.style.overflowY = 'hidden';
                this._enabledScrollY = false;
            }
        }
        else if (key == 'upper-threshold') {
            this._upperThreshold = V_1.pixelValue(value);
        }
        else if (key == 'lower-threshold') {
            this._lowerThreshold = V_1.pixelValue(value);
        }
        else if (key == 'scroll-into-view') {
            if (value) {
                var v = document.getElementById(value);
                if (v && v.parentElement == this._view) {
                    if (this._enabledScrollY) {
                        this.scrollTo(this._view.scrollLeft, v.offsetTop, this._scrollWithAnimation);
                    }
                    else if (this._enabledScrollX) {
                        this.scrollTo(v.offsetLeft, this._view.scrollTop, this._scrollWithAnimation);
                    }
                }
            }
        }
        else if (key == 'scroll-top') {
            if (this._enabledScrollY) {
                this.scrollTo(this._view.scrollLeft, V_1.pixelValue(value), this._scrollWithAnimation);
            }
        }
        else if (key == 'scroll-left') {
            if (this._enabledScrollX) {
                this.scrollTo(V_1.pixelValue(value), this._view.scrollTop, this._scrollWithAnimation);
            }
        }
    };
    return ScrollViewElement;
}(ViewElement_1.ViewElement));
exports.ScrollViewElement = ScrollViewElement;
