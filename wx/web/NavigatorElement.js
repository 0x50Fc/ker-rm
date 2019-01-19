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
var NavigatorElement = /** @class */ (function (_super) {
    __extends(NavigatorElement, _super);
    function NavigatorElement(document, name, id) {
        var _this = _super.call(this, document, name, id) || this;
        _this._hoverStartTime = 50;
        _this._hoverStayTime = 600;
        _this.set("hover-class", "navigator-hover");
        return _this;
    }
    NavigatorElement.prototype.doEvent = function (event, name, detail) {
        _super.prototype.doEvent.call(this, event, name, detail);
        var v = this.get("url");
        if (name == "tap" && v) {
            IPC_1.postMessage({ page: "open", url: v });
        }
    };
    return NavigatorElement;
}(ViewElement_1.ViewElement));
exports.NavigatorElement = NavigatorElement;
