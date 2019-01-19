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
var Element_1 = require("./Element");
var BlockElement_1 = require("./BlockElement");
var V_1 = require("./V");
function getDataSet(element) {
    var v = element;
    if (v.__kk_dataSet === undefined) {
        if (element.parentElement !== undefined) {
            return getDataSet(element.parentElement);
        }
    }
    else {
        return v.__kk_dataSet;
    }
    return undefined;
}
function getTouches(touches) {
    var vs = [];
    var length = touches.length;
    for (var i = 0; i < length; i++) {
        var touch = touches.item(i);
        vs.push({
            identifier: touch.identifier,
            pageX: touch.pageX,
            pageY: touch.pageY,
            clientX: touch.clientX,
            clientY: touch.clientY
        });
    }
    return vs;
}
var ViewElement = /** @class */ (function (_super) {
    __extends(ViewElement, _super);
    function ViewElement(document, name, id) {
        var _this = _super.call(this, document, name, id) || this;
        _this._hoverStartTime = 50;
        _this._hoverStayTime = 400;
        _this._hoverStopPropagation = false;
        _this._hover = false;
        var timeStamp = 0;
        var element = _this;
        _this._view = _this.createView();
        _this._view.__kk_dataSet = {};
        _this._view.addEventListener("touchstart", function (event) {
            timeStamp = event.timeStamp;
            element.doEvent(event, "touchstart", {});
        });
        _this._view.addEventListener("touchmove", function (event) {
            element.doEvent(event, "touchmove", {});
        });
        _this._view.addEventListener("touchcancel", function (event) {
            element.doEvent(event, "touchcancel", {});
        });
        _this._view.addEventListener("touchend", function (event) {
            element.doEvent(event, "touchend", {});
        });
        _this._view.addEventListener("touchforcechange", function (event) {
            element.doEvent(event, "touchforcechange", {});
        });
        _this._view.addEventListener("mouseup", function (event) {
            if (event.timeStamp - timeStamp > 350) {
                if (element.has("longpress")) {
                    element.doEvent(event, "longpress", {});
                    return;
                }
                element.doEvent(event, "longtap", {});
            }
            element.doEvent(event, "tap", {});
        });
        return _this;
    }
    Object.defineProperty(ViewElement.prototype, "view", {
        get: function () {
            return this._view;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ViewElement.prototype, "contentView", {
        get: function () {
            return this._view;
        },
        enumerable: true,
        configurable: true
    });
    ViewElement.prototype.createView = function () {
        return document.createElement("wx-" + this._name);
    };
    ViewElement.prototype.setHover = function (hover) {
        if (this._hoverTimeoutId) {
            clearTimeout(this._hoverTimeoutId);
            this._hoverTimeoutId = undefined;
        }
        if (hover) {
            var v = this.get("hover-class");
            if (v && v != 'none') {
                this._view.className = this.resolveClassName((this.get("class") || '') + ' ' + v);
                return;
            }
        }
        this._view.className = this.resolveClassName(this.get("class"));
        this._hover = hover;
    };
    ViewElement.prototype.doElementEvent = function (name, detail) {
        var e = new Element_1.ElementEvent(this);
        var target = {
            id: this._view.id,
            dataset: getDataSet(this._view)
        };
        e.data = {
            type: name,
            target: target,
            currentTarget: target,
            detail: detail
        };
        this.emit(name, e);
    };
    ViewElement.prototype.doEvent = function (event, name, detail) {
        var e = new Element_1.ElementEvent(this);
        e.data = {
            type: name,
            timeStamp: event.timeStamp,
            target: event.target ? {
                id: event.target.id,
                dataset: getDataSet(event.target)
            } : undefined,
            currentTarget: event.currentTarget ? {
                id: event.currentTarget.id,
                dataset: getDataSet(event.currentTarget)
            } : undefined,
            detail: detail,
            touches: event instanceof TouchEvent ? getTouches(event.touches) : undefined,
            changedTouches: event instanceof TouchEvent ? getTouches(event.changedTouches) : undefined,
        };
        this.emit(name, e);
        if (e.cancelBubble) {
            event.stopPropagation();
        }
        if (name == "touchstart") {
            if (this._hoverTimeoutId) {
                clearTimeout(this._hoverTimeoutId);
            }
            var v_1 = this;
            this._hoverTimeoutId = setTimeout(function () {
                v_1.setHover(true);
            }, this._hoverStartTime);
        }
        else if (name == "touchend" || name == "touchcancel") {
            if (this._hoverTimeoutId) {
                clearTimeout(this._hoverTimeoutId);
            }
            var v_2 = this;
            this._hoverTimeoutId = setTimeout(function () {
                v_2.setHover(false);
            }, this._hoverStayTime);
        }
    };
    ViewElement.prototype.resolveClassName = function (v) {
        return v || '';
    };
    ViewElement.prototype.setData = function (key, value) {
        var e = this._view;
        var dataSet = e.__kk_dataSet;
        if (dataSet === undefined) {
            e.__kk_dataSet = dataSet = {};
        }
        if (value === undefined) {
            delete dataSet[key];
        }
        else {
            dataSet[key] = value;
        }
    };
    ViewElement.prototype.set = function (key, value) {
        _super.prototype.set.call(this, key, value);
        if (key == 'class') {
            this._view.className = this.resolveClassName(value);
        }
        else if (key == 'style') {
            if (value === undefined) {
                this._view.removeAttribute(key);
            }
            else {
                this._view.setAttribute('style', V_1.parseStyleValue(value));
            }
        }
        else if (key == '#text') {
            this._view.innerText = value === undefined ? '' : value.replace('\\n', '\n');
        }
        else if (key == 'id') {
            if (value === undefined) {
                this._view.removeAttribute("id");
            }
            else {
                this._view.setAttribute("id", value);
            }
        }
        else if (key == 'hover-stop-propagation') {
            this._hoverStopPropagation = value == 'true';
        }
        else if (key == 'hover-start-time') {
            this._hoverStartTime = parseInt(value || '0');
        }
        else if (key == 'hover-stay-time') {
            this._hoverStayTime = parseInt(value || '0');
        }
    };
    ViewElement.prototype.onDidAddToParent = function (element) {
        _super.prototype.onDidAddToParent.call(this, element);
        if (element instanceof ViewElement) {
            element.contentView.appendChild(this._view);
        }
        else if (element instanceof BlockElement_1.BlockElement) {
            if (element.parent instanceof ViewElement) {
                element.parent.contentView.appendChild(this._view);
            }
        }
        else {
            document.body.appendChild(this._view);
        }
    };
    ViewElement.prototype.onDidAddChildren = function (element) {
        _super.prototype.onDidAddChildren.call(this, element);
        if (element instanceof BlockElement_1.BlockElement) {
            var p = element.firstChild;
            while (p) {
                if (p instanceof ViewElement) {
                    this.contentView.appendChild(p.view);
                }
                p = p.nextSibling;
            }
        }
    };
    ViewElement.prototype.onWillRemoveFromParent = function (element) {
        _super.prototype.onWillRemoveFromParent.call(this, element);
        var p = this._view.parentElement;
        if (p) {
            p.removeChild(this._view);
        }
    };
    ViewElement.prototype.recycle = function () {
        _super.prototype.recycle.call(this);
    };
    return ViewElement;
}(Element_1.Element));
exports.ViewElement = ViewElement;
