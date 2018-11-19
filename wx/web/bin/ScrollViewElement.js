"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
const V_1 = require("./V");
class ScrollViewElement extends ViewElement_1.ViewElement {
    constructor(document, name, id) {
        super(document, name, id);
        let element = this;
        let delta = {
            x: 0,
            y: 0,
            deltaX: 0,
            deltaY: 0
        };
        this._view = this.createView();
        this._view.addEventListener("scroll", (event) => {
            delta.deltaX = element.scrollLeft - delta.x;
            delta.deltaY = element.scrollTop - delta.y;
            delta.x = element.scrollLeft;
            delta.y = element.scrollTop;
            element.doEvent(event, "scroll", {
                scrollLeft: element.scrollLeft,
                scrollTop: element.scrollTop,
                scrollWidth: element.scrollWidth,
                scrollHeight: element.scrollHeight,
                deltaX: delta.deltaX,
                deltaY: delta.deltaY
            });
            event.stopPropagation();
        });
    }
    get scrollLeft() {
        return this._view.scrollLeft;
    }
    get scrollTop() {
        return this._view.scrollTop;
    }
    get scrollWidth() {
        return this._view.scrollWidth;
    }
    get scrollHeight() {
        return this._view.scrollHeight;
    }
    set(key, value) {
        super.set(key, value);
        if (key == 'scroll-x') {
            if (V_1.booleanValue(value)) {
                this._view.style.overflowX = 'auto';
            }
            else {
                this._view.style.overflowX = 'hidden';
            }
        }
        else if (key == 'scroll-y') {
            if (V_1.booleanValue(value)) {
                this._view.style.overflowY = 'auto';
            }
            else {
                this._view.style.overflowY = 'hidden';
            }
        }
    }
}
exports.ScrollViewElement = ScrollViewElement;
//# sourceMappingURL=ScrollViewElement.js.map