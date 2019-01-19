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
var CheckboxGroupElement = /** @class */ (function (_super) {
    __extends(CheckboxGroupElement, _super);
    function CheckboxGroupElement(document, name, id) {
        return _super.call(this, document, name, id) || this;
    }
    CheckboxGroupElement.prototype.doChange = function () {
        this.doElementEvent("change", { value: this.getValue() });
    };
    CheckboxGroupElement.prototype.getValue = function () {
        var vs = [];
        Element_1.Each(this, function (element) {
            if (element instanceof CheckboxElement) {
                if (V_1.booleanValue(element.get("checked"))) {
                    var v = element.get("value");
                    if (v !== undefined) {
                        vs.push(v);
                    }
                }
                return false;
            }
            return true;
        });
        return vs;
    };
    CheckboxGroupElement.prototype.setValue = function (value) {
        var vs = {};
        if (value instanceof Array) {
            for (var _i = 0, value_1 = value; _i < value_1.length; _i++) {
                var v = value_1[_i];
                vs[v] = true;
            }
        }
        Element_1.Each(this, function (element) {
            if (element instanceof CheckboxElement) {
                var v = element.get("value");
                if (v) {
                    if (vs[v]) {
                        element.set("checked", "true");
                    }
                    else {
                        element.set("checked", undefined);
                    }
                }
                return false;
            }
            return true;
        });
    };
    return CheckboxGroupElement;
}(ViewElement_1.ViewElement));
exports.CheckboxGroupElement = CheckboxGroupElement;
var CheckboxElement = /** @class */ (function (_super) {
    __extends(CheckboxElement, _super);
    function CheckboxElement(document, name, id) {
        return _super.call(this, document, name, id) || this;
    }
    CheckboxElement.prototype.createView = function () {
        var v = _super.prototype.createView.call(this);
        v.checkedView = document.createElement('div');
        v.appendChild(v.checkedView);
        return v;
    };
    CheckboxElement.prototype.set = function (key, value) {
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
    CheckboxElement.prototype.doEvent = function (event, name, detail) {
        _super.prototype.doEvent.call(this, event, name, detail);
        if (name == 'tap') {
            if (!this._view.getAttribute("disabled")) {
                if (V_1.booleanValue(this.get("checked"))) {
                    this.set("checked", undefined);
                }
                else {
                    this.set("checked", "true");
                }
                var p = this.parent;
                while (p) {
                    if (p instanceof CheckboxGroupElement) {
                        p.doChange();
                        break;
                    }
                    p = p.parent;
                }
                event.stopPropagation();
            }
        }
    };
    return CheckboxElement;
}(ViewElement_1.ViewElement));
exports.CheckboxElement = CheckboxElement;
