"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const Element_1 = require("./Element");
function getDataSet(element) {
    let v = {};
    let length = element.attributes.length;
    for (var i = 0; i < length; i++) {
        var attr = element.attributes.item(i);
        if (attr.localName.startsWith("data-")) {
            v[attr.localName.substr(5)] = attr.value;
        }
    }
    return v;
}
function getTouches(touches) {
    let vs = [];
    let length = touches.length;
    for (var i = 0; i < length; i++) {
        let touch = touches.item(i);
        vs.push({
            identifier: touch.identifier,
            pageX: touch.pageX,
            pageY: touch.pageY,
            clientX: touch.clientX,
            clientY: touch.clientY
        });
    }
    return vs;
}
class ViewElement extends Element_1.Element {
    get view() {
        return this._view;
    }
    createView() {
        return document.createElement("wx-" + this._name);
    }
    doEvent(event, name, detail) {
        let e = new Element_1.ElementEvent(this);
        e.data = {
            type: name,
            timeStamp: event.timeStamp,
            target: event.target ? {
                id: event.target.id,
                dataset: getDataSet(event.target)
            } : undefined,
            currentTarget: event.currentTarget ? {
                id: event.currentTarget.id,
                dataset: getDataSet(event.target)
            } : undefined,
            detail: detail,
            touches: event instanceof TouchEvent ? getTouches(event.touches) : undefined,
            changedTouches: event instanceof TouchEvent ? getTouches(event.changedTouches) : undefined,
        };
        this.emit(name, e);
    }
    constructor(document, name, id) {
        super(document, name, id);
        let timeStamp = 0;
        let element = this;
        this._view = this.createView();
        this._view.addEventListener("touchstart", (event) => {
            timeStamp = event.timeStamp;
            element.doEvent(event, "touchstart", {});
            event.stopPropagation();
        });
        this._view.addEventListener("touchmove", (event) => {
            element.doEvent(event, "touchmove", {});
            event.stopPropagation();
        });
        this._view.addEventListener("touchcancel", (event) => {
            element.doEvent(event, "touchcancel", {});
            event.stopPropagation();
        });
        this._view.addEventListener("touchend", (event) => {
            element.doEvent(event, "touchend", {});
            if (event.timeStamp - timeStamp > 350) {
                if (element.has("longpress")) {
                    element.doEvent(event, "longpress", {});
                    return;
                }
                element.doEvent(event, "longtap", {});
            }
            element.doEvent(event, "tap", {});
            event.stopPropagation();
        });
        this._view.addEventListener("touchforcechange", (event) => {
            element.doEvent(event, "touchforcechange", {});
            event.stopPropagation();
        });
    }
    set(key, value) {
        super.set(key, value);
        if (key == 'class') {
            this._view.className = value === undefined ? '' : value;
        }
        else if (key == 'style') {
            if (value === undefined) {
                this._view.removeAttribute(key);
            }
            else {
                this._view.setAttribute('style', value);
            }
        }
        else if (key == '#text') {
            this._view.textContent = value === undefined ? '' : value;
        }
        else if (key == 'id') {
            if (value === undefined) {
                this._view.removeAttribute("id");
            }
            else {
                this._view.setAttribute("id", value);
            }
        }
        else if (key.startsWith("data-")) {
            if (value === undefined) {
                this._view.removeAttribute(key);
            }
            else {
                this._view.setAttribute(key, value);
            }
        }
    }
    onDidAddToParent(element) {
        super.onDidAddToParent(element);
        if (element instanceof ViewElement) {
            element.view.appendChild(this._view);
        }
        else {
            document.body.appendChild(this._view);
        }
    }
    onWillRemoveFromParent(element) {
        super.onWillRemoveFromParent(element);
        let p = this._view.parentElement;
        if (p) {
            p.removeChild(this._view);
        }
    }
    recycle() {
        super.recycle();
    }
}
exports.ViewElement = ViewElement;
//# sourceMappingURL=ViewElement.js.map