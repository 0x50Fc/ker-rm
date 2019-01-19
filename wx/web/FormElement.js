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
var SwitchElement_1 = require("./SwitchElement");
var InputElement_1 = require("./InputElement");
var CheckboxElement_1 = require("./CheckboxElement");
var SliderElement_1 = require("./SliderElement");
var RadioElement_1 = require("./RadioElement");
var PickerElement_1 = require("./PickerElement");
var once_1 = require("./once");
function Each(element, func) {
    if (element instanceof SwitchElement_1.SwitchElement
        || element instanceof InputElement_1.InputElement
        || element instanceof CheckboxElement_1.CheckboxGroupElement
        || element instanceof SliderElement_1.SliderElement
        || element instanceof RadioElement_1.RadioGroupElement
        || element instanceof PickerElement_1.PickerElement) {
        var name_1 = element.get("name");
        if (name_1 !== undefined) {
            var v = element;
            func(name_1, v);
        }
    }
    else {
        var p = element.firstChild;
        while (p) {
            Each(p, func);
            p = p.nextSibling;
        }
    }
}
var FormElement = /** @class */ (function (_super) {
    __extends(FormElement, _super);
    function FormElement(document, name, id) {
        var _this = _super.call(this, document, name, id) || this;
        var v = _this;
        once_1.once(function () {
            v.defaultValues();
        });
        return _this;
    }
    FormElement.prototype.doAction = function (action) {
        switch (action) {
            case 'submit':
                this.doSubmit();
                break;
            case 'reset':
                this.doReset();
                break;
        }
    };
    FormElement.prototype.defaultValues = function () {
        if (this._formValues === undefined) {
            this._formValues = {};
            this.getValues(this._formValues, this);
        }
    };
    FormElement.prototype.doSubmit = function () {
        var values = {};
        Each(this, function (name, element) {
            values[name] = element.getValue();
        });
        this.doElementEvent("submit", { value: values, formId: this.get("id") });
    };
    FormElement.prototype.getValues = function (values, element) {
        Each(this, function (name, element) {
            values[name] = element.getValue();
        });
    };
    FormElement.prototype.setValues = function (values, element) {
        Each(this, function (name, element) {
            element.setValue(values[name]);
        });
    };
    FormElement.prototype.doReset = function () {
        if (this._formValues !== undefined) {
            this.setValues(this._formValues, this);
        }
        this.doElementEvent("reset", {});
    };
    return FormElement;
}(ViewElement_1.ViewElement));
exports.FormElement = FormElement;
