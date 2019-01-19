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
var once_1 = require("./once");
var BlockElement_1 = require("./BlockElement");
var Anim_1 = require("./Anim");
var SwiperItemElement = /** @class */ (function (_super) {
    __extends(SwiperItemElement, _super);
    function SwiperItemElement() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    return SwiperItemElement;
}(ViewElement_1.ViewElement));
exports.SwiperItemElement = SwiperItemElement;
var SwiperElement = /** @class */ (function (_super) {
    __extends(SwiperElement, _super);
    function SwiperElement(document, name, id) {
        var _this = _super.call(this, document, name, id) || this;
        _this._scrollAnimationId = 0;
        _this._index = 0;
        _this._touching = false;
        _this._preScrollX = 0;
        _this._preScrollY = 0;
        _this._vScrollX = 0;
        _this._vScrollY = 0;
        _this._vertical = false;
        _this._items = [];
        _this._autoplaying = false;
        _this._updateItemsing = false;
        _this._interval = 5000;
        _this._duration = 500;
        _this._autoplay = false;
        var element = _this;
        _this.scrollView.addEventListener("scroll", function (event) {
            element.onScroll(event);
            event.stopPropagation();
        });
        _this.scrollView.addEventListener("touchstart", function (event) {
            element.onTouchStart(event);
        });
        _this.scrollView.addEventListener("touchend", function (event) {
            element.onTouchEnd(event);
            event.preventDefault();
            event.stopPropagation();
        });
        _this.scrollView.addEventListener("touchcancel", function (event) {
            element.onTouchEnd(event);
            event.preventDefault();
            event.stopPropagation();
        });
        return _this;
    }
    Object.defineProperty(SwiperElement.prototype, "contentView", {
        get: function () {
            return this._view.contentView;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SwiperElement.prototype, "width", {
        get: function () {
            return this._view.clientWidth;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SwiperElement.prototype, "height", {
        get: function () {
            return this._view.clientHeight;
        },
        enumerable: true,
        configurable: true
    });
    SwiperElement.prototype.scrollTo = function (x, y, animated) {
        if (this._scrollAnimationId) {
            Anim_1.clearAnimation(this._scrollAnimationId);
            this._scrollAnimationId = 0;
        }
        if (animated) {
            var element_1 = this;
            var view_1 = this.scrollView;
            var x0_1 = view_1.scrollLeft;
            var y0_1 = view_1.scrollTop;
            var dx_1 = x - x0_1;
            var dy_1 = y - y0_1;
            this._scrollAnimationId = Anim_1.startAnimation(this._duration, function (v) {
                view_1.scrollTo(x0_1 + dx_1 * v, y0_1 + dy_1 * v);
                if (v >= 1) {
                    element_1._scrollAnimationId = 0;
                }
            });
        }
        else {
            this._view.scrollTo(x, y);
        }
    };
    SwiperElement.prototype.scrollToPage = function (index, animated, source) {
        if (this._vertical) {
            var b = this._view.clientHeight;
            this.scrollTo(this.scrollView.scrollLeft, (index * b), animated);
        }
        else {
            var b = this._view.clientWidth;
            this.scrollTo((index * b), this.scrollView.scrollTop, animated);
        }
        var view = this.dotsView;
        var items = view.children;
        var n = items.length;
        for (var i = 0; i < n; i++) {
            var item = items[i];
            if (i == index) {
                item.className = "wx-active";
            }
            else {
                item.className = "";
            }
        }
        if (this._index != index) {
            this._index = index;
            this.doElementEvent("change", { current: index, source: source });
        }
    };
    SwiperElement.prototype.onTouchStart = function (event) {
        this._touching = true;
    };
    SwiperElement.prototype.onTouchEnd = function (event) {
        this._touching = false;
        if (this._items.length == 0) {
            return;
        }
        if (this._vertical) {
            var index = Math.floor(this.scrollView.scrollTop / this._view.clientHeight);
            if (this._vScrollY > 5) {
                index++;
            }
            if (index < 0) {
                index = 0;
            }
            if (index > this._items.length) {
                index = this._items.length - 1;
            }
            this.scrollToPage(index, true, "touch");
        }
        else {
            var index = Math.floor(this.scrollView.scrollLeft / this._view.clientWidth);
            if (this._vScrollX > 5) {
                index++;
            }
            if (index < 0) {
                index = 0;
            }
            if (index > this._items.length) {
                index = this._items.length - 1;
            }
            this.scrollToPage(index, true, "touch");
        }
    };
    SwiperElement.prototype.onScroll = function (event) {
        var x = this.scrollView.scrollLeft;
        var y = this.scrollView.scrollTop;
        if (Math.abs(x - this._preScrollX) > 5) {
            this._vScrollX = x - this._preScrollX;
            this._preScrollX = x;
        }
        if (Math.abs(y - this._preScrollY) > 5) {
            this._vScrollY = y - this._preScrollY;
            this._preScrollY = y;
        }
    };
    Object.defineProperty(SwiperElement.prototype, "dotsView", {
        get: function () {
            return this._view.dotsView;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SwiperElement.prototype, "scrollView", {
        get: function () {
            return this._view.scrollView;
        },
        enumerable: true,
        configurable: true
    });
    SwiperElement.prototype.createView = function () {
        var v = document.createElement("wx-" + this._name);
        v.scrollView = document.createElement("div");
        v.scrollView.className = "wx-scroll";
        v.appendChild(v.scrollView);
        v.contentView = document.createElement("div");
        v.contentView.className = "wx-content";
        v.scrollView.appendChild(v.contentView);
        v.dotsView = document.createElement("div");
        v.dotsView.className = "wx-dots";
        v.appendChild(v.dotsView);
        return v;
    };
    SwiperElement.prototype.set = function (key, value) {
        _super.prototype.set.call(this, key, value);
        if (key == "indicator-dots") {
            if (V_1.booleanValue(value)) {
                this.dotsView.style.display = 'block';
            }
            else {
                this.dotsView.style.display = 'none';
            }
        }
        else if (key == 'vertical') {
            this._vertical = V_1.booleanValue(value);
            var v = this.scrollView;
            if (this._vertical) {
                v.style.overflowX = 'hidden';
                v.style.overflowY = 'auto';
            }
            else {
                v.style.overflowX = 'auto';
                v.style.overflowY = 'hidden';
            }
            this.setNeedsUpdateItems();
        }
        else if (key == 'autoplay') {
            this._autoplay = V_1.booleanValue(value);
            this.setNeedsAutoplay();
        }
        else if (key == 'duration') {
            this._duration = parseFloat(value || '0');
        }
        else if (key == 'interval') {
            this._interval = parseFloat(value || '0');
            this.setNeedsAutoplay();
        }
    };
    SwiperElement.prototype.updateDotView = function () {
        var view = this.dotsView;
        var p = view.firstElementChild;
        var index = 0;
        var n = this._items.length;
        while (p) {
            if (index < n) {
                p.className = '';
            }
            else {
                var e = p.nextElementSibling;
                view.removeChild(p);
                p = e;
                index++;
                continue;
            }
            p = p.nextElementSibling;
            index++;
        }
        while (index < n) {
            var v = document.createElement('span');
            view.appendChild(v);
            index++;
        }
        if (this._vertical) {
            view.className = 'wx-dots wx-vertical';
            view.style.marginLeft = 'auto';
            var h = (9 * this._items.length);
            view.style.height = h + 'px';
            view.style.marginTop = (-h * 0.5) + 'px';
        }
        else {
            view.className = 'wx-dots';
            view.style.height = 'auto';
            view.style.marginTop = 'auto';
            view.style.marginLeft = (-view.clientWidth * 0.5) + 'px';
        }
    };
    SwiperElement.prototype.updateItems = function () {
        var width = this.width;
        var height = this.height;
        var index = 0;
        var p = this.firstChild;
        var items = [];
        var vertical = this._vertical;
        var each = function (p) {
            if (vertical) {
                p.view.style.top = (index * height) + 'px';
                p.view.style.left = '0px';
            }
            else {
                p.view.style.left = (index * width) + 'px';
                p.view.style.top = '0px';
            }
            p.view.style.width = width + 'px';
            p.view.style.height = height + 'px';
            items.push(p);
            index++;
        };
        while (p) {
            if (p instanceof SwiperItemElement) {
                each(p);
            }
            else if (p instanceof BlockElement_1.BlockElement) {
                var e = p.firstChild;
                while (e) {
                    if (e instanceof SwiperItemElement) {
                        each(e);
                    }
                    e = e.nextSibling;
                }
            }
            p = p.nextSibling;
        }
        this._items = items;
        if (vertical) {
            this.contentView.style.width = width + 'px';
            this.contentView.style.height = (height * index) + 'px';
            this.scrollView.style.width = (width + 20) + 'px';
            this.scrollView.style.height = height + 'px';
        }
        else {
            this.contentView.style.width = (width * index) + 'px';
            this.contentView.style.height = height + 'px';
            this.scrollView.style.width = width + 'px';
            this.scrollView.style.height = (height + 20) + 'px';
        }
        if (index >= items.length) {
            index = 0;
        }
        this.updateDotView();
        this.scrollToPage(index, false, "autoplay");
        this._updateItemsing = false;
    };
    SwiperElement.prototype.autoplayNextPage = function () {
        if (this._touching) {
            return;
        }
        if (this._scrollAnimationId) {
            return;
        }
        var n = this._items.length;
        if (n == 0) {
            return;
        }
        this.scrollToPage((this._index + 1) % n, true, "autoplay");
    };
    SwiperElement.prototype.setAutoplay = function () {
        if (this._autoplay) {
            clearInterval(this._autoplayId);
            var v_1 = this;
            this._autoplayId = setInterval(function () {
                v_1.autoplayNextPage();
            }, this._interval);
        }
        else if (this._autoplayId) {
            clearInterval(this._autoplayId);
            this._autoplayId = undefined;
        }
        this._autoplaying = false;
    };
    SwiperElement.prototype.setNeedsAutoplay = function () {
        if (this._autoplaying) {
            return;
        }
        this._autoplaying = true;
        var v = this;
        once_1.once(function () {
            v.setAutoplay();
        });
    };
    SwiperElement.prototype.recycle = function () {
        if (this._autoplayId) {
            clearInterval(this._autoplayId);
            this._autoplayId = undefined;
        }
        _super.prototype.recycle.call(this);
    };
    SwiperElement.prototype.setNeedsUpdateItems = function () {
        if (this._updateItemsing) {
            return;
        }
        this._updateItemsing = true;
        var v = this;
        once_1.once(function () {
            v.updateItems();
        });
    };
    SwiperElement.prototype.onDidAddChildren = function (element) {
        _super.prototype.onDidAddChildren.call(this, element);
        this.setNeedsUpdateItems();
    };
    SwiperElement.prototype.onWillRemoveChildren = function (element) {
        _super.prototype.onWillRemoveChildren.call(this, element);
        this.setNeedsUpdateItems();
    };
    return SwiperElement;
}(ViewElement_1.ViewElement));
exports.SwiperElement = SwiperElement;
