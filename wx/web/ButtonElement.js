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
var FormElement_1 = require("./FormElement");
var ButtonElement = /** @class */ (function (_super) {
    __extends(ButtonElement, _super);
    function ButtonElement(document, name, id) {
        var _this = _super.call(this, document, name, id) || this;
        _this._enabled = true;
        _this._hoverStartTime = 20;
        _this._hoverStayTime = 70;
        _this.set("hover-class", "button-hover");
        return _this;
    }
    ButtonElement.prototype.doEvent = function (event, name, detail) {
        if (this._enabled) {
            _super.prototype.doEvent.call(this, event, name, detail);
            if (name == "tap") {
                var v = this.get("formType");
                if (v === undefined) {
                    v = this.get("formtype");
                }
                if (v === undefined) {
                    v = this.get("form-type");
                }
                if (v) {
                    var p = this.parent;
                    while (p) {
                        if (p instanceof FormElement_1.FormElement) {
                            p.doAction(v);
                            break;
                        }
                        p = p.parent;
                    }
                }
            }
        }
    };
    ButtonElement.prototype.set = function (key, value) {
        _super.prototype.set.call(this, key, value);
        if (key == 'size') {
            if (value === undefined) {
                this._view.removeAttribute(key);
            }
            else {
                this._view.setAttribute(key, value);
            }
        }
        else if (key == 'disabled') {
            if (value == 'true' || value == 'disabled') {
                this._view.setAttribute(key, value);
                this._enabled = false;
            }
            else {
                this._view.removeAttribute(key);
                this._enabled = true;
            }
        }
        else if (key == 'type') {
            if (value === undefined) {
                this._view.removeAttribute(key);
            }
            else {
                this._view.setAttribute(key, value);
            }
        }
        else if (key == 'plain') {
            if (value == 'true' || value == 'plain') {
                this._view.setAttribute(key, value);
            }
            else {
                this._view.removeAttribute(key);
            }
        }
    };
    return ButtonElement;
}(ViewElement_1.ViewElement));
exports.ButtonElement = ButtonElement;
