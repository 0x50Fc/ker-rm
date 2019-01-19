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
var ViewElement_1 = require("./ViewElement");
var BlockElement = /** @class */ (function (_super) {
    __extends(BlockElement, _super);
    function BlockElement() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    BlockElement.prototype.onDidAddChildren = function (element) {
        _super.prototype.onDidAddChildren.call(this, element);
        if (element instanceof ViewElement_1.ViewElement) {
            var p = this.parent;
            if (p instanceof ViewElement_1.ViewElement) {
                p.view.appendChild(element.view);
            }
            else if (p) {
                document.body.appendChild(element.view);
            }
        }
    };
    BlockElement.prototype.onWillRemoveFromParent = function (element) {
        _super.prototype.onWillRemoveFromParent.call(this, element);
        var p = this.firstChild;
        while (p) {
            if (p instanceof ViewElement_1.ViewElement) {
                var v = p.view;
                var pv = v.parentElement;
                if (pv) {
                    pv.removeChild(v);
                }
            }
            p = p.nextSibling;
        }
    };
    return BlockElement;
}(Element_1.Element));
exports.BlockElement = BlockElement;
