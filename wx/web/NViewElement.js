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
var IPC_1 = require("./IPC");
var once_1 = require("./once");
var V_1 = require("./V");
var _elements = {};
function add(element) {
    _elements[element.id] = element;
}
function remove(element) {
    delete _elements[element.id];
}
setInterval(function () {
    for (var id in _elements) {
        var e = _elements[id];
        if (e.needsDisplay) {
            e.display();
        }
    }
}, 1000 / 30);
var NViewElement = /** @class */ (function (_super) {
    __extends(NViewElement, _super);
    function NViewElement(document, name, id) {
        var _this = _super.call(this, document, name, id) || this;
        _this._displaying = false;
        _this._displayFrame = { x: 0, y: 0, width: 0, height: 0 };
        add(_this);
        return _this;
    }
    NViewElement.prototype.createView = function () {
        var v = document.createElement("wx-" + this._name);
        IPC_1.postMessage({
            view: 'create',
            id: this._id,
            name: this._name
        });
        return v;
    };
    Object.defineProperty(NViewElement.prototype, "displaying", {
        get: function () {
            return this._displaying;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(NViewElement.prototype, "needsDisplay", {
        get: function () {
            if (this._displaying) {
                return false;
            }
            var p = this._view;
            var x = 0;
            var y = 0;
            while (p !== undefined && p != document.body) {
                x += p.offsetLeft;
                y += p.offsetTop;
                p = p.offsetParent;
            }
            return (x != this._displayFrame.x || y != this._displayFrame.y
                || this._view.clientWidth != this._displayFrame.width
                || this._view.clientHeight != this._displayFrame.height);
        },
        enumerable: true,
        configurable: true
    });
    NViewElement.prototype.display = function () {
        var p = this._view;
        var x = 0;
        var y = 0;
        while (p !== undefined && p != document.body) {
            x += p.offsetLeft;
            y += p.offsetTop;
            p = p.offsetParent;
        }
        this._displayFrame.x = x;
        this._displayFrame.y = y;
        this._displayFrame.width = this._view.clientWidth;
        this._displayFrame.height = this._view.clientHeight;
        IPC_1.postMessage({
            view: 'setFrame',
            id: this._id,
            x: V_1.nativePixelValue(x),
            y: V_1.nativePixelValue(y),
            width: V_1.nativePixelValue(this._displayFrame.width),
            height: V_1.nativePixelValue(this._displayFrame.height)
        });
        this._displaying = false;
    };
    NViewElement.prototype.setNeedsDisplay = function () {
        if (this._displaying) {
            return;
        }
        this._displaying = true;
        var v = this;
        once_1.once(function () {
            v.display();
        });
    };
    NViewElement.prototype.hasNativeKey = function (key) {
        return true;
    };
    NViewElement.prototype.set = function (key, value) {
        _super.prototype.set.call(this, key, value);
        if (this.hasNativeKey(key)) {
            IPC_1.postMessage({
                view: 'set',
                id: this._id,
                name: key,
                value: value
            });
        }
        this.setNeedsDisplay();
    };
    NViewElement.prototype.onDidAddToParent = function (element) {
        _super.prototype.onDidAddToParent.call(this, element);
        var pid = undefined;
        if (element instanceof NViewElement) {
            pid = element._id;
        }
        IPC_1.postMessage({
            view: 'add',
            id: this._id,
            pid: pid
        });
        this.setNeedsDisplay();
    };
    NViewElement.prototype.onWillRemoveFromParent = function (element) {
        _super.prototype.onWillRemoveFromParent.call(this, element);
        IPC_1.postMessage({
            view: 'remove',
            id: this._id
        });
    };
    NViewElement.prototype.recycle = function () {
        remove(this);
        IPC_1.postMessage({
            view: 'remove',
            id: this._id
        });
        _super.prototype.recycle.call(this);
    };
    return NViewElement;
}(ViewElement_1.ViewElement));
exports.NViewElement = NViewElement;
