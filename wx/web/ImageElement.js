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
var ImageElement = /** @class */ (function (_super) {
    __extends(ImageElement, _super);
    function ImageElement() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    Object.defineProperty(ImageElement.prototype, "imageView", {
        get: function () {
            return this._view.firstElementChild;
        },
        enumerable: true,
        configurable: true
    });
    ImageElement.prototype.createView = function () {
        var v = document.createElement("wx-" + this._name);
        var img = document.createElement("img");
        v.appendChild(img);
        return v;
    };
    ImageElement.prototype.set = function (key, value) {
        _super.prototype.set.call(this, key, value);
        if (key == 'src') {
            if (value === undefined) {
                this.imageView.removeAttribute("src");
            }
            else {
                this.imageView.setAttribute("src", this.resolveURI(value));
            }
        }
    };
    return ImageElement;
}(ViewElement_1.ViewElement));
exports.ImageElement = ImageElement;
