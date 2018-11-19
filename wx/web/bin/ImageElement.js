"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
const URI_1 = require("./URI");
class ImageElement extends ViewElement_1.ViewElement {
    get imageView() {
        return this._view.firstElementChild;
    }
    createView() {
        var v = document.createElement("wx-" + this._name);
        var img = document.createElement("img");
        img.style.width = "100%";
        v.appendChild(img);
        v.style.overflow = 'hidden';
        return v;
    }
    set(key, value) {
        super.set(key, value);
        if (key == 'src') {
            if (value === undefined) {
                this.imageView.removeAttribute("src");
            }
            else {
                this.imageView.setAttribute("src", URI_1.resolveURI(value));
            }
        }
    }
}
exports.ImageElement = ImageElement;
//# sourceMappingURL=ImageElement.js.map