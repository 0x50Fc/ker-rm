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
var Element_1 = require("./Element");
var RadioGroupElement = /** @class */ (function (_super) {
    __extends(RadioGroupElement, _super);
    function RadioGroupElement(document, name, id) {
        return _super.call(this, document, name, id) || this;
    }
    RadioGroupElement.prototype.findCheckbox = function (element, cur) {
        if (element instanceof RadioElement) {
            if (element != cur) {
                element.set("checked", undefined);
            }
        }
        else {
            var p = element.firstChild;
            while (p) {
                this.findCheckbox(p, cur);
                p = p.nextSibling;
            }
        }
    };
    RadioGroupElement.prototype.doChange = function (element) {
        this.findCheckbox(this, element);
        this.doElementEvent("change", { value: element.get("value") });
    };
    RadioGroupElement.prototype.getValue = function () {
        var v;
        Element_1.Each(this, function (element) {
            if (element instanceof RadioElement) {
                if (V_1.booleanValue(element.get("checked"))) {
                    v = element.get("value");
                }
                return false;
            }
            return v === undefined;
        });
        return v;
    };
    RadioGroupElement.prototype.setValue = function (value) {
        Element_1.Each(this, function (element) {
            if (element instanceof RadioElement) {
                if (element.get("value") == value) {
                    element.set("checked", "true");
                }
                else {
                    element.set("checked", undefined);
                }
                return false;
            }
            return true;
        });
    };
    return RadioGroupElement;
}(ViewElement_1.ViewElement));
exports.RadioGroupElement = RadioGroupElement;
var RadioElement = /** @class */ (function (_super) {
    __extends(RadioElement, _super);
    function RadioElement(document, name, id) {
        return _super.call(this, document, name, id) || this;
    }
    RadioElement.prototype.createView = function () {
        var v = _super.prototype.createView.call(this);
        v.checkedView = document.createElement('div');
        v.appendChild(v.checkedView);
        return v;
    };
    RadioElement.prototype.set = function (key, value) {
        _super.prototype.set.call(this, key, value);
        if (key == 'value') {
            if (value === undefined) {
                this._view.removeAttribute("value");
            }
            else {
                this._view.setAttribute("value", value);
            }
        }
        else if (key == 'disabled') {
            if (V_1.booleanValue(value)) {
                this._view.setAttribute("disabled", "disabled");
            }
            else {
                this._view.removeAttribute("disabled");
            }
        }
        else if (key == 'checked') {
            if (V_1.booleanValue(value)) {
                this._view.setAttribute("checked", "checked");
            }
            else {
                this._view.removeAttribute("checked");
            }
        }
        else if (key == 'color') {
            this._view.checkedView.style.backgroundColor = value || '';
        }
    };
    RadioElement.prototype.doEvent = function (event, name, detail) {
        _super.prototype.doEvent.call(this, event, name, detail);
        if (name == 'tap') {
            if (!this._view.getAttribute("disabled")) {
                this.set("checked", "true");
                var p = this.parent;
                while (p) {
                    if (p instanceof RadioGroupElement) {
                        p.doChange(this);
                        break;
                    }
                    p = p.parent;
                }
                event.stopPropagation();
            }
        }
    };
    return RadioElement;
}(ViewElement_1.ViewElement));
exports.RadioElement = RadioElement;
