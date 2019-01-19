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
var SwitchElement = /** @class */ (function (_super) {
    __extends(SwitchElement, _super);
    function SwitchElement(document, name, id) {
        return _super.call(this, document, name, id) || this;
    }
    SwitchElement.prototype.createView = function () {
        var v = _super.prototype.createView.call(this);
        v.checkedView = document.createElement('div');
        v.checkedView.className = "cur";
        v.appendChild(v.checkedView);
        return v;
    };
    SwitchElement.prototype.set = function (key, value) {
        _super.prototype.set.call(this, key, value);
        if (key == 'disabled') {
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
    };
    SwitchElement.prototype.doEvent = function (event, name, detail) {
        _super.prototype.doEvent.call(this, event, name, detail);
        if (name == 'tap') {
            if (!this._view.getAttribute("disabled")) {
                var v = V_1.booleanValue(this.get("checked"));
                if (v) {
                    this.set("checked", undefined);
                    v = false;
                }
                else {
                    this.set("checked", "true");
                    v = true;
                }
                this.doElementEvent("change", { value: v });
                event.stopPropagation();
            }
        }
    };
    SwitchElement.prototype.getValue = function () {
        return V_1.booleanValue(this.get("checked"));
    };
    SwitchElement.prototype.setValue = function (value) {
        if (V_1.booleanValue(value)) {
            this.set("checked", "true");
        }
        else {
            this.set("checked", undefined);
        }
    };
    return SwitchElement;
}(ViewElement_1.ViewElement));
exports.SwitchElement = SwitchElement;
