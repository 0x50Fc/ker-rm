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
var ButtonElement_1 = require("./ButtonElement");
var CheckboxElement_1 = require("./CheckboxElement");
var RadioElement_1 = require("./RadioElement");
var SwitchElement_1 = require("./SwitchElement");
var LabelElement = /** @class */ (function (_super) {
    __extends(LabelElement, _super);
    function LabelElement() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    LabelElement.prototype.doEvent = function (event, name, detail) {
        _super.prototype.doEvent.call(this, event, name, detail);
        if (name == "tap") {
            var p = this.firstChild;
            while (p) {
                if (p instanceof ButtonElement_1.ButtonElement || p instanceof CheckboxElement_1.CheckboxElement || p instanceof RadioElement_1.RadioElement || p instanceof SwitchElement_1.SwitchElement) {
                    p.doEvent(event, name, detail);
                    break;
                }
                p = p.nextSibling;
            }
        }
    };
    return LabelElement;
}(ViewElement_1.ViewElement));
exports.LabelElement = LabelElement;
