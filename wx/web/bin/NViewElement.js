"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
const IPC_1 = require("./IPC");
const once_1 = require("./once");
var _autoId = 0;
class NViewElement extends ViewElement_1.ViewElement {
    constructor() {
        super(...arguments);
        this._displaying = false;
    }
    createView() {
        var v = document.createElement("wx-" + this._name);
        IPC_1.postMessage({
            view: 'create',
            id: this._id,
            name: this._name
        });
        return v;
    }
    display() {
        var p = this._view;
        var x = 0;
        var y = 0;
        while (p !== undefined && p != document.body) {
            x += p.offsetLeft;
            y += p.offsetTop;
            p = p.offsetParent;
        }
        IPC_1.postMessage({
            view: 'setFrame',
            id: this._id,
            x: x,
            y: y,
            width: this._view.clientWidth,
            height: this._view.clientHeight
        });
        this._displaying = false;
    }
    setNeedsDisplay() {
        if (this._displaying) {
            return;
        }
        this._displaying = true;
        var v = this;
        once_1.once(function () {
            v.display();
        });
    }
    set(key, value) {
        super.set(key, value);
        IPC_1.postMessage({
            view: 'set',
            id: this._id,
            name: key,
            value: value
        });
        this.setNeedsDisplay();
    }
    onDidAddToParent(element) {
        super.onDidAddToParent(element);
        var pid = undefined;
        if (element instanceof NViewElement) {
            pid = element._id;
        }
        IPC_1.postMessage({
            view: 'add',
            id: this._id,
            pid: pid
        });
        this.setNeedsDisplay();
    }
    onWillRemoveFromParent(element) {
        super.onWillRemoveFromParent(element);
        IPC_1.postMessage({
            view: 'remove',
            id: this._id
        });
    }
    recycle() {
        IPC_1.postMessage({
            view: 'remove',
            id: this._id
        });
        super.recycle();
    }
}
exports.NViewElement = NViewElement;
//# sourceMappingURL=NViewElement.js.map