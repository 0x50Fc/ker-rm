"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const Event_1 = require("./Event");
const EventEmitter_1 = require("./EventEmitter");
class ElementEvent extends Event_1.Event {
    constructor(element) {
        super();
        this.cancelBubble = false;
        this.element = element;
    }
}
exports.ElementEvent = ElementEvent;
class Element extends EventEmitter_1.EventEmitter {
    constructor(document, name, id) {
        super();
        this._attributes = {};
        this._levelId = 0;
        this._depth = 0;
        this._autoLevelId = 0;
        this._name = name;
        this._id = id;
        this._document = document;
    }
    get document() {
        return this._document;
    }
    get id() {
        return this._id;
    }
    get name() {
        return this._name;
    }
    get levelId() {
        return this._levelId;
    }
    get depth() {
        return this._depth;
    }
    get firstChild() {
        return this._firstChild;
    }
    get lastChild() {
        return this._lastChild;
    }
    get nextSibling() {
        return this._nextSibling;
    }
    get prevSibling() {
        return this._prevSibling;
    }
    get parent() {
        return this._parent;
    }
    onDidAddToParent(element) {
    }
    onDidAddChildren(element) {
        let e = element;
        e._depth = this._depth + 1;
        e._levelId = this._autoLevelId + 1;
        this._autoLevelId = e._levelId;
        e.onDidAddToParent(this);
    }
    append(element) {
        var e = element;
        e.remove();
        if (this._lastChild !== undefined) {
            this._lastChild._nextSibling = e;
            e._prevSibling = this._lastChild;
            this._lastChild = e;
            e._parent = this;
        }
        else {
            this._firstChild = e;
            this._lastChild = e;
            e._parent = this;
        }
        this.onDidAddChildren(element);
    }
    before(element) {
        var e = element;
        e.remove();
        if (this._prevSibling !== undefined) {
            this._prevSibling._nextSibling = e;
            e._prevSibling = this._prevSibling;
            e._nextSibling = this;
            e._parent = this._parent;
            this._prevSibling = e;
        }
        else if (this._parent) {
            e._nextSibling = this;
            e._parent = this._parent;
            this._prevSibling = e;
            this._parent._firstChild = e;
        }
        if (this._parent !== undefined) {
            this._parent.onDidAddChildren(element);
        }
    }
    after(element) {
        var e = element;
        e.remove();
        if (this._nextSibling !== undefined) {
            this._nextSibling._prevSibling = e;
            e._nextSibling = this._nextSibling;
            e._prevSibling = this;
            e._parent = this._parent;
            this._nextSibling = e;
        }
        else if (this._parent) {
            e._prevSibling = this;
            e._parent = this._parent;
            this._nextSibling = e;
            this._parent._lastChild = e;
        }
        if (this._parent !== undefined) {
            this._parent.onDidAddChildren(element);
        }
    }
    onWillRemoveFromParent(element) {
    }
    onWillRemoveChildren(element) {
        let e = element;
        e._depth = 0;
        e._levelId = 0;
        e.onWillRemoveFromParent(this);
    }
    remove() {
        if (this._prevSibling !== undefined && this._parent !== undefined) {
            this._parent.onWillRemoveChildren(this);
            this._prevSibling._nextSibling = this._nextSibling;
            if (this._nextSibling !== undefined) {
                this._nextSibling._prevSibling = this._prevSibling;
            }
            else {
                this._parent._lastChild = this._prevSibling;
            }
        }
        else if (this._parent !== undefined) {
            this._parent.onWillRemoveChildren(this);
            this._parent._firstChild = this._nextSibling;
            if (this._nextSibling) {
                this._nextSibling._prevSibling = undefined;
            }
            else {
                this._parent._lastChild = undefined;
            }
        }
        if (this._parent) {
            this._parent = undefined;
            this._prevSibling = undefined;
            this._nextSibling = undefined;
        }
    }
    appendTo(element) {
        element.append(this);
    }
    beforeTo(element) {
        element.before(this);
    }
    afterTo(element) {
        element.after(this);
    }
    get(key) {
        return this._attributes[key];
    }
    set(key, value) {
        if (value === undefined) {
            delete this._attributes[key];
        }
        else {
            this._attributes[key] = value;
        }
    }
    get attributes() {
        return this._attributes;
    }
    emit(name, event) {
        if (event instanceof ElementEvent) {
            var e = event;
            if (e.element === undefined) {
                e.element = this;
            }
        }
        super.emit(name, event);
        if (event instanceof ElementEvent) {
            var e = event;
            if (!e.cancelBubble) {
                if (this._parent !== undefined) {
                    this._parent.emit(name, event);
                }
            }
        }
    }
    hasBubble(name) {
        if (super.has(name)) {
            return true;
        }
        if (this._parent !== undefined) {
            return this._parent.hasBubble(name);
        }
        return false;
    }
    recycle() {
        this._document.removeElement(this._id);
        var p = this._firstChild;
        while (p !== undefined) {
            let n = p._nextSibling;
            p.recycle();
            p = n;
        }
        this._parent = undefined;
        this._firstChild = undefined;
        this._lastChild = undefined;
        this._prevSibling = undefined;
        this._nextSibling = undefined;
    }
    onEvent(name, data) {
    }
}
exports.Element = Element;
//# sourceMappingURL=Element.js.map