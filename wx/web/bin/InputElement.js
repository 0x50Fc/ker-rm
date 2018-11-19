"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const NViewElement_1 = require("./NViewElement");
const IPC_1 = require("./IPC");
class InputElement extends NViewElement_1.NViewElement {
    constructor() {
        super(...arguments);
        this._value = "";
    }
    createView() {
        var v = super.createView();
        this._placeholderView = document.createElement('span');
        v.appendChild(this._placeholderView);
        IPC_1.postMessage({
            view: 'on',
            id: this._id,
            name: 'change'
        });
        return v;
    }
    set(key, value) {
        super.set(key, value);
        if (key == 'placeholder') {
            this._placeholderView.textContent = value;
        }
        else if (key == 'placeholder-style') {
            this._placeholderView.setAttribute("style", value);
        }
        else if (key == 'placeholder-class') {
            this._placeholderView.className = value;
        }
        else if (key == 'value') {
            this.value = value || '';
        }
    }
    set value(value) {
        this._value = value;
        if (this._value) {
            this._placeholderView.style.visibility = 'hidden';
        }
        else {
            this._placeholderView.style.visibility = 'visible';
        }
    }
    get value() {
        return this._value;
    }
    onEvent(name, data) {
        console.info(name, data);
        if (name == "change") {
            this.value = data.value || '';
        }
    }
}
exports.InputElement = InputElement;
//# sourceMappingURL=InputElement.js.map