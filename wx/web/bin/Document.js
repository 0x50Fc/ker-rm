"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const Element_1 = require("./Element");
class Document {
    constructor() {
        this._autoId = 0;
        this._elementClass = {};
        this._elements = {};
        this._documentElement = new Element_1.Element(this, "document", 0);
    }
    createElement(name) {
        let id = ++this._autoId;
        let fn = this._elementClass[name];
        var v;
        if (fn === undefined) {
            v = new Element_1.Element(this, name, id);
        }
        else {
            v = new fn(this, name, id);
        }
        this._elements[id] = v;
        return v;
    }
    element(id) {
        return this._elements[id];
    }
    removeElement(id) {
        delete this._elementClass[id];
    }
    addElementClass(name, elementClass) {
        this._elementClass[name] = elementClass;
    }
    get documentElement() {
        return this._documentElement;
    }
}
exports.Document = Document;
//# sourceMappingURL=Document.js.map