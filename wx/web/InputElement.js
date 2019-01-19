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
var NViewElement_1 = require("./NViewElement");
var IPC_1 = require("./IPC");
var InputElement = /** @class */ (function (_super) {
    __extends(InputElement, _super);
    function InputElement() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this._value = "";
        return _this;
    }
    InputElement.prototype.createView = function () {
        var v = _super.prototype.createView.call(this);
        this._placeholderView = document.createElement('span');
        v.appendChild(this._placeholderView);
        IPC_1.postMessage({
            view: 'on',
            id: this._id,
            name: 'change'
        });
        return v;
    };
    InputElement.prototype.display = function () {
        _super.prototype.display.call(this);
        var s = window.getComputedStyle(this._view);
        IPC_1.postMessage({
            view: 'set',
            id: this._id,
            name: 'padding',
            value: [s.paddingTop, s.paddingRight, s.paddingBottom, s.paddingLeft].join(' '),
        });
    };
    InputElement.prototype.hasNativeKey = function (key) {
        if (key == "placeholder" || key.startsWith("placeholder-")) {
            return false;
        }
        return _super.prototype.hasNativeKey.call(this, key);
    };
    InputElement.prototype.set = function (key, value) {
        _super.prototype.set.call(this, key, value);
        if (key == 'placeholder') {
            this._placeholderView.textContent = value;
        }
        else if (key == 'placeholder-style') {
            this._placeholderView.setAttribute("style", value);
        }
        else if (key == 'placeholder-class') {
            this._placeholderView.className = value;
        }
        else if (key == 'value') {
            this.setValue(value || '');
        }
    };
    Object.defineProperty(InputElement.prototype, "value", {
        get: function () {
            return this._value;
        },
        set: function (value) {
            this._value = value;
            if (this._value) {
                this._placeholderView.style.visibility = 'hidden';
            }
            else {
                this._placeholderView.style.visibility = 'visible';
            }
        },
        enumerable: true,
        configurable: true
    });
    InputElement.prototype.onEvent = function (name, data) {
        if (name == "change") {
            this.value = data.value || '';
            this.doElementEvent("input", data);
        }
    };
    InputElement.prototype.getValue = function () {
        return this.value;
    };
    InputElement.prototype.setValue = function (value) {
        this.value = value;
        IPC_1.postMessage({
            view: 'set',
            id: this._id,
            name: 'value',
            value: value
        });
    };
    return InputElement;
}(NViewElement_1.NViewElement));
exports.InputElement = InputElement;
