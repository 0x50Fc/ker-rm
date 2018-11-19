(function(){function e(t,n,r){function s(o,u){if(!n[o]){if(!t[o]){var a=typeof require=="function"&&require;if(!u&&a)return a(o,!0);if(i)return i(o,!0);var f=new Error("Cannot find module '"+o+"'");throw f.code="MODULE_NOT_FOUND",f}var l=n[o]={exports:{}};t[o][0].call(l.exports,function(e){var n=t[o][1][e];return s(n?n:e)},l,l.exports,e,t,n,r)}return n[o].exports}var i=typeof require=="function"&&require;for(var o=0;o<r.length;o++)s(r[o]);return s}return e})()({1:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class ButtonElement extends ViewElement_1.ViewElement {
}
exports.ButtonElement = ButtonElement;

},{"./ViewElement":34}],2:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const NViewElement_1 = require("./NViewElement");
class CanvasElement extends NViewElement_1.NViewElement {
}
exports.CanvasElement = CanvasElement;

},{"./NViewElement":17}],3:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class CheckboxElement extends ViewElement_1.ViewElement {
}
exports.CheckboxElement = CheckboxElement;

},{"./ViewElement":34}],4:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
class IObject {
    get(key) {
        return this[key];
    }
    set(key, value) {
        if (value === undefined) {
            delete this[key];
        }
        else {
            this[key] = value;
        }
    }
}
exports.IObject = IObject;
function get(object, keys, index = 0) {
    if (index < keys.length) {
        var key = keys[index];
        if (typeof object == 'object') {
            if (object instanceof IObject) {
                return get(object.get(key), keys, index + 1);
            }
            return get(object[key], keys, index + 1);
        }
    }
    else {
        return object;
    }
}
exports.get = get;
function set(object, keys, value, index = 0) {
    if (typeof object != 'object') {
        return;
    }
    if (index + 1 < keys.length) {
        var key = keys[index];
        var v;
        if (object instanceof IObject) {
            v = object.get(key);
        }
        else {
            v = object[key];
        }
        if (v === undefined) {
            v = {};
            if (object instanceof IObject) {
                object.set(key, v);
            }
            else {
                object[key] = v;
            }
        }
        set(v, keys, value, index + 1);
    }
    else if (index < keys.length) {
        var key = keys[index];
        if (object instanceof IObject) {
            object.set(key, value);
        }
        else {
            object[key] = value;
        }
    }
}
exports.set = set;
class Evaluate {
    constructor(keys, evaluateScript) {
        this.keys = keys;
        this.evaluateScript = evaluateScript;
    }
    exec(object) {
        var vs = [];
        for (let key of this.keys) {
            let v = object[key[0]];
            if (v === undefined) {
                v = window[key[0]];
            }
            vs.push(v);
        }
        return this.evaluateScript.apply(undefined, vs);
    }
}
exports.Evaluate = Evaluate;
class KeyCallback {
    constructor(func) {
        this.hasChildren = false;
        this.priority = 0;
        this.func = func;
    }
    run(object, changedKeys) {
        var v;
        if (this.evaluate !== undefined) {
            v = this.evaluate.exec(object);
        }
        else if (this.keys !== undefined) {
            v = get(object, this.keys);
        }
        this.func(v, changedKeys);
    }
}
class KeyObserver {
    constructor() {
        this.children = {};
        this.callbacks = [];
    }
    add(keys, callback, index) {
        if (index < keys.length) {
            let key = keys[index];
            var ch = this.children[key];
            if (ch === undefined) {
                ch = new KeyObserver();
                this.children[key] = ch;
            }
            ch.add(keys, callback, index + 1);
        }
        else {
            this.callbacks.push(callback);
        }
    }
    remove(keys, func, index) {
        if (func === undefined) {
            this.children = {};
            this.callbacks = [];
        }
        else if (index < keys.length) {
            let key = keys[index];
            let ch = this.children[key];
            if (ch !== undefined) {
                ch.remove(keys, func, index + 1);
            }
        }
        else {
            let cbs = [];
            for (let cb of this.callbacks) {
                if (cb.func != func) {
                    cbs.push(cb);
                }
            }
            this.callbacks = cbs;
            for (let key in this.children) {
                var ch = this.children[key];
                ch.remove(keys, func, index);
            }
        }
    }
    change(keys, callbacks, index) {
        if (index < keys.length) {
            let key = keys[index];
            let ch = this.children[key];
            if (ch !== undefined) {
                ch.change(keys, callbacks, index + 1);
            }
            for (let cb of this.callbacks) {
                if (cb.hasChildren) {
                    callbacks.push(cb);
                }
            }
        }
        else {
            for (let cb of this.callbacks) {
                callbacks.push(cb);
            }
            for (let key in this.children) {
                var ch = this.children[key];
                ch.change(keys, callbacks, index);
            }
        }
    }
    changedKeys(object, keys) {
        let callbacks = [];
        this.change(keys, callbacks, 0);
        callbacks.sort((a, b) => {
            return a.priority - b.priority;
        });
        for (let cb of callbacks) {
            cb.run(object, keys);
        }
    }
    on(object, keys, func, hasChildren = false, priority = 0) {
        let onKeys = [];
        let cb = new KeyCallback(func);
        cb.hasChildren = hasChildren;
        cb.priority = priority;
        if (keys instanceof Evaluate) {
            onKeys = keys.keys;
            cb.evaluate = keys;
        }
        else {
            cb.keys = keys;
            onKeys.push(keys);
        }
        if (onKeys.length == 0) {
            var vv;
            if (cb.evaluate !== undefined) {
                try {
                    vv = cb.evaluate.exec(object);
                }
                catch (e) {
                    console.info("[ERROR] " + e);
                }
            }
            if (vv !== undefined) {
                func(vv, []);
            }
        }
        else {
            for (let ks of onKeys) {
                this.add(ks, cb, 0);
            }
        }
    }
    off(keys, func) {
        this.remove(keys, func, 0);
    }
}
class Data {
    constructor() {
        this._keyObserver = new KeyObserver();
        this.object = {};
    }
    get(keys) {
        return get(this.object, keys);
    }
    set(keys, value, changed = true) {
        set(this.object, keys, value);
        if (changed === true) {
            this.changedKeys(keys);
        }
    }
    changedKeys(keys) {
        this._keyObserver.changedKeys(this.object, keys);
    }
    on(keys, func, hasChildren = false, priority = 0) {
        this._keyObserver.on(this.object, keys, func, hasChildren, priority);
    }
    off(keys, func) {
        this._keyObserver.off(keys, func);
    }
    setParent(parent) {
        this.recycle();
        if (parent !== undefined) {
            this._parent = parent;
            let data = this;
            this._parentFunc = function (value, keys) {
                if (value !== undefined) {
                    data.set(keys, get(value, keys));
                }
            };
            parent.on([], this._parentFunc, true);
            for (var key in parent.object) {
                this.object[key] = parent.object[key];
            }
        }
    }
    recycle() {
        if (this._parent !== undefined) {
            this._parent.off([], this._parentFunc);
            this._parent = undefined;
            this._parentFunc = undefined;
        }
    }
    static evaluateKeys(evaluate, keys) {
        var v = evaluate.replace(/(\\\')|(\\\")/g, '');
        v = v.replace(/(\'.*?\')|(\".*?\")/g, '');
        v = v.replace(/\".*?\"/g, '');
        v.replace(/[a-zA-Z_][0-9a-zA-Z\\._]*/g, (name) => {
            if (!/(true)|(false)|(null)|(undefined)|(NaN)/i.test(name)) {
                keys.push(name.split("."));
            }
            return '';
        });
    }
    static evaluateScript(evaluateScript) {
        let keys = [];
        let code = [];
        var idx = 0;
        var count = 0;
        evaluateScript.replace(/\{\{(.*?)\}\}/g, (text, exaluate, index) => {
            if (index > idx) {
                if (count != 0) {
                    code.push("+");
                }
                code.push(JSON.stringify(evaluateScript.substr(idx, index - idx)));
                count++;
            }
            Data.evaluateKeys(exaluate, keys);
            if (count != 0) {
                code.push("+");
            }
            code.push("(");
            code.push(exaluate);
            code.push(")");
            count++;
            idx = index + text.length;
            return '';
        });
        if (evaluateScript.length > idx && count != 0) {
            code.push("+");
            code.push(JSON.stringify(evaluateScript.substr(idx)));
        }
        if (count == 0) {
            return undefined;
        }
        let args = [];
        for (let key of keys) {
            args.push(key[0]);
        }
        return new Evaluate(keys, eval('(function(' + args.join(',') + '){ return ' + code.join('') + '; })'));
    }
}
exports.Data = Data;

},{}],5:[function(require,module,exports){
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

},{"./Element":6}],6:[function(require,module,exports){
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

},{"./Event":7,"./EventEmitter":8}],7:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
class Event {
}
exports.Event = Event;

},{}],8:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
class EventEmitter {
    constructor() {
        this._events = {};
    }
    on(name, func) {
        var v = this._events[name];
        if (v === undefined) {
            v = [];
            this._events[name] = v;
        }
        v.push(func);
    }
    off(name, func) {
        if (name === undefined && func === undefined) {
            this._events = {};
        }
        else if (func === undefined && name !== undefined) {
            delete this._events[name];
        }
        else if (name !== undefined) {
            var v = this._events[name];
            if (v !== undefined) {
                var vs = [];
                for (let fn of v) {
                    if (fn != func) {
                        vs.push(fn);
                    }
                }
                this._events[name] = vs;
            }
        }
    }
    emit(name, event) {
        var v = this._events[name];
        if (v !== undefined) {
            var vs = v.slice();
            for (let fn of vs) {
                fn(event);
            }
        }
    }
    has(name) {
        return this._events[name] !== undefined;
    }
}
exports.EventEmitter = EventEmitter;

},{}],9:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class FormElement extends ViewElement_1.ViewElement {
}
exports.FormElement = FormElement;

},{"./ViewElement":34}],10:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function postMessage(data) {
    let w = window;
    if (w.webkit && w.webkit.messageHandlers && w.webkit.messageHandlers.kk) {
        w.webkit.messageHandlers.kk.postMessage(data);
    }
}
exports.postMessage = postMessage;

},{}],11:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class IconElement extends ViewElement_1.ViewElement {
}
exports.IconElement = IconElement;

},{"./ViewElement":34}],12:[function(require,module,exports){
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

},{"./URI":32,"./ViewElement":34}],13:[function(require,module,exports){
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

},{"./IPC":10,"./NViewElement":17}],14:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class LabelElement extends ViewElement_1.ViewElement {
}
exports.LabelElement = LabelElement;

},{"./ViewElement":34}],15:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const Data_1 = require("./Data");
const Element_1 = require("./Element");
const Page_1 = require("./Page");
const ViewElement_1 = require("./ViewElement");
const ImageElement_1 = require("./ImageElement");
const IPC_1 = require("./IPC");
const InputElement_1 = require("./InputElement");
const ScrollViewElement_1 = require("./ScrollViewElement");
const SwiperElement_1 = require("./SwiperElement");
const MovableViewElement_1 = require("./MovableViewElement");
const IconElement_1 = require("./IconElement");
const TextElement_1 = require("./TextElement");
const RichTextElement_1 = require("./RichTextElement");
const ProgressElement_1 = require("./ProgressElement");
const ButtonElement_1 = require("./ButtonElement");
const CheckboxElement_1 = require("./CheckboxElement");
const FormElement_1 = require("./FormElement");
const LabelElement_1 = require("./LabelElement");
const PickerElement_1 = require("./PickerElement");
const PickerViewElement_1 = require("./PickerViewElement");
const RadioElement_1 = require("./RadioElement");
const SliderElement_1 = require("./SliderElement");
const SwitchElement_1 = require("./SwitchElement");
const TextareaElement_1 = require("./TextareaElement");
const NavigatorElement_1 = require("./NavigatorElement");
const CanvasElement_1 = require("./CanvasElement");
const once_1 = require("./once");
function ElementOnEvent(element, prefix, name, value) {
    element.on(name, (event) => {
        if (event instanceof Element_1.ElementEvent) {
            IPC_1.postMessage({
                event: value,
                data: event.data
            });
            if (prefix.endsWith("catch")) {
                event.cancelBubble = true;
            }
        }
    });
}
function ElementSetAttributes(element, data, attributes) {
    for (let key in attributes) {
        if (key.startsWith("wx:")) {
        }
        else if (key.startsWith("bind:")) {
            ElementOnEvent(element, key.substr(0, 4), key.substr(5), attributes[key]);
        }
        else if (key.startsWith("bind")) {
            ElementOnEvent(element, key.substr(0, 4), key.substr(4), attributes[key]);
        }
        else if (key.startsWith("catch:")) {
            ElementOnEvent(element, key.substr(0, 5), key.substr(6), attributes[key]);
        }
        else if (key.startsWith("catch")) {
            ElementOnEvent(element, key.substr(0, 5), key.substr(6), attributes[key]);
        }
        else if (key.startsWith("capture-bind:")) {
            ElementOnEvent(element, key.substr(0, 12), key.substr(13), attributes[key]);
        }
        else if (key.startsWith("capture-bind")) {
            ElementOnEvent(element, key.substr(0, 12), key.substr(12), attributes[key]);
        }
        else if (key.startsWith("capture-catch:")) {
            ElementOnEvent(element, key.substr(0, 13), key.substr(14), attributes[key]);
        }
        else if (key.startsWith("capture-catch")) {
            ElementOnEvent(element, key.substr(0, 13), key.substr(13), attributes[key]);
        }
        else {
            let v = attributes[key];
            let evaluate = Data_1.Data.evaluateScript(v);
            if (evaluate === undefined) {
                element.set(key, v);
            }
            else {
                let fn = (key, element, evaluate) => {
                    data.on(evaluate, (value, changdKeys) => {
                        element.set(key, value + '');
                    });
                };
                fn(key, element, evaluate);
            }
        }
    }
}
function CreateForElement(element, data, name, attributes, context, children) {
    let evaluate = Data_1.Data.evaluateScript(attributes["wx:for"]);
    if (evaluate === undefined) {
        return;
    }
    delete attributes["wx:for"];
    let before = context.page.document.createElement("element");
    before.appendTo(element);
    let index = attributes["wx:for-index"] || "index";
    let item = attributes["wx:for-item"] || "item";
    let items = [];
    data.on(evaluate, (object, changedKeys) => {
        var i = 0;
        context.begin();
        if (object instanceof Array) {
            for (let d of object) {
                let v;
                if (i < items.length) {
                    v = items[i];
                }
                else {
                    v = {
                        data: new Data_1.Data(),
                        element: context.page.document.createElement(name)
                    };
                    ElementSetAttributes(v.element, v.data, attributes);
                    before.before(v.element);
                    context.begin();
                    children(v.element, v.data, context);
                    context.end();
                    v.data.setParent(data);
                    items.push(v);
                }
                v.data.set([index], i, false);
                v.data.set([item], d, false);
                v.data.changedKeys([]);
                i++;
            }
        }
        while (i < items.length) {
            let v = items.pop();
            v.element.remove();
            v.element.recycle();
            v.data.recycle();
        }
        context.end();
    });
}
function CreateIfElement(element, data, name, attributes, context, children) {
    let evaluate = Data_1.Data.evaluateScript(attributes["wx:if"]);
    if (evaluate === undefined) {
        return;
    }
    let before = context.page.document.createElement("element");
    before.appendTo(element);
    let scope = context.scope();
    let block;
    block = new Page_1.IfBlock({
        element: undefined,
        name: name,
        attributes: attributes,
        children: children,
        evaluate: evaluate,
        data: data,
        elementData: undefined
    }, (value, changedKeys) => {
        let e;
        for (let item of block.elements) {
            if (e === undefined) {
                if (item.evaluate !== undefined) {
                    let v = item.evaluate.evaluateScript(item.data.object);
                    if (v) {
                        e = item;
                    }
                }
                else {
                    e = item;
                }
            }
            if (e == item) {
                if (item.element === undefined) {
                    item.element = context.page.document.createElement(item.name);
                    item.elementData = new Data_1.Data();
                    ElementSetAttributes(item.element, item.data, item.attributes);
                    context.begin();
                    item.children(item.element, item.elementData, context);
                    context.end();
                    item.elementData.setParent(item.data);
                }
                else {
                    before.before(item.element);
                    item.elementData.changedKeys([]);
                }
            }
            else if (item.element !== undefined) {
                item.element.remove();
                item.element.recycle();
                item.elementData.recycle();
                item.element = undefined;
                item.elementData = undefined;
            }
        }
    });
    scope.ifblock = block;
    data.on(evaluate, block.func);
}
function CreateElifElement(element, data, name, attributes, context, children) {
    let scope = context.scope();
    if (scope.ifblock !== undefined) {
        let evaluate = Data_1.Data.evaluateScript(attributes["wx:elif"]);
        if (evaluate === undefined) {
            return;
        }
        scope.ifblock.elements.push({
            element: undefined,
            name: name,
            attributes: attributes,
            children: children,
            evaluate: evaluate,
            data: data,
            elementData: undefined
        });
        data.on(evaluate, scope.ifblock.func);
    }
}
function CreateElseElement(element, data, name, attributes, context, children) {
    let scope = context.scope();
    if (scope.ifblock !== undefined) {
        scope.ifblock.elements.push({
            element: undefined,
            name: name,
            attributes: attributes,
            children: children,
            evaluate: undefined,
            data: data,
            elementData: undefined
        });
        scope.ifblock = undefined;
    }
}
function CreateElement(element, data, name, attributes, context, children) {
    if (attributes["wx:for"] !== undefined) {
        CreateForElement(element, data, name, attributes, context, children);
    }
    else if (attributes["wx:if"] !== undefined) {
        CreateIfElement(element, data, name, attributes, context, children);
    }
    else if (attributes["wx:elif"] !== undefined) {
        CreateElifElement(element, data, name, attributes, context, children);
    }
    else if (attributes["wx:else"] !== undefined) {
        CreateElseElement(element, data, name, attributes, context, children);
    }
    else {
        let e = context.page.document.createElement(name);
        ElementSetAttributes(e, data, attributes);
        element.append(e);
        context.begin();
        children(e, data, context);
        context.end();
    }
}
exports.CreateElement = CreateElement;
var page = new Page_1.Page();
page.document.addElementClass("view", ViewElement_1.ViewElement);
page.document.addElementClass("input", InputElement_1.InputElement);
page.document.addElementClass("image", ImageElement_1.ImageElement);
page.document.addElementClass("scroll-view", ScrollViewElement_1.ScrollViewElement);
page.document.addElementClass("swiper", SwiperElement_1.SwiperElement);
page.document.addElementClass("movable-view", MovableViewElement_1.MovableViewElement);
page.document.addElementClass("icon", IconElement_1.IconElement);
page.document.addElementClass("text", TextElement_1.TextElement);
page.document.addElementClass("rich-text", RichTextElement_1.RichTextElement);
page.document.addElementClass("progress", ProgressElement_1.ProgressElement);
page.document.addElementClass("button", ButtonElement_1.ButtonElement);
page.document.addElementClass("checkbox", CheckboxElement_1.CheckboxElement);
page.document.addElementClass("form", FormElement_1.FormElement);
page.document.addElementClass("label", LabelElement_1.LabelElement);
page.document.addElementClass("picker", PickerElement_1.PickerElement);
page.document.addElementClass("picker-view", PickerViewElement_1.PickerViewElement);
page.document.addElementClass("radio", RadioElement_1.RadioElement);
page.document.addElementClass("slider", SliderElement_1.SliderElement);
page.document.addElementClass("switch", SwitchElement_1.SwitchElement);
page.document.addElementClass("textarea", TextareaElement_1.TextareaElement);
page.document.addElementClass("navigator", NavigatorElement_1.NavigatorElement);
page.document.addElementClass("canvas", CanvasElement_1.CanvasElement);
function Page(view, styleSheet, options) {
    IPC_1.postMessage({ page: 'readying' });
    view(page.document.documentElement, page.data, new Page_1.PageViewContext(page));
    once_1.once(() => { IPC_1.postMessage({ page: 'ready' }); });
}
exports.Page = Page;
function setData(data) {
    for (var key in data) {
        page.data.set([key], data[key]);
    }
    console.info("[DATA]", data);
}
exports.setData = setData;
function sendEvent(id, name, data) {
    let element = page.document.element(id);
    if (element != undefined) {
        element.onEvent(name, data);
    }
}
exports.sendEvent = sendEvent;

},{"./ButtonElement":1,"./CanvasElement":2,"./CheckboxElement":3,"./Data":4,"./Element":6,"./FormElement":9,"./IPC":10,"./IconElement":11,"./ImageElement":12,"./InputElement":13,"./LabelElement":14,"./MovableViewElement":16,"./NavigatorElement":18,"./Page":19,"./PickerElement":20,"./PickerViewElement":21,"./ProgressElement":22,"./RadioElement":23,"./RichTextElement":24,"./ScrollViewElement":25,"./SliderElement":26,"./SwiperElement":28,"./SwitchElement":29,"./TextElement":30,"./TextareaElement":31,"./ViewElement":34,"./once":35}],16:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class MovableViewElement extends ViewElement_1.ViewElement {
}
exports.MovableViewElement = MovableViewElement;

},{"./ViewElement":34}],17:[function(require,module,exports){
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

},{"./IPC":10,"./ViewElement":34,"./once":35}],18:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class NavigatorElement extends ViewElement_1.ViewElement {
}
exports.NavigatorElement = NavigatorElement;

},{"./ViewElement":34}],19:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const Data_1 = require("./Data");
const Document_1 = require("./Document");
class IfBlock {
    constructor(element, func) {
        this.elements = [element];
        this.func = func;
    }
}
exports.IfBlock = IfBlock;
class PageViewScope {
}
exports.PageViewScope = PageViewScope;
class PageViewContext {
    constructor(page) {
        this._page = page;
        this._scopes = [new PageViewScope()];
    }
    get page() {
        return this._page;
    }
    begin() {
        this._scopes.push(new PageViewScope());
    }
    end() {
        this._scopes.pop();
    }
    scope() {
        return this._scopes[this._scopes.length - 1];
    }
}
exports.PageViewContext = PageViewContext;
class Page {
    constructor() {
        this._document = new Document_1.Document();
        this._data = new Data_1.Data();
    }
    get document() {
        return this._document;
    }
    get data() {
        return this._data;
    }
}
exports.Page = Page;

},{"./Data":4,"./Document":5}],20:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class PickerElement extends ViewElement_1.ViewElement {
}
exports.PickerElement = PickerElement;

},{"./ViewElement":34}],21:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class PickerViewElement extends ViewElement_1.ViewElement {
}
exports.PickerViewElement = PickerViewElement;

},{"./ViewElement":34}],22:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class ProgressElement extends ViewElement_1.ViewElement {
}
exports.ProgressElement = ProgressElement;

},{"./ViewElement":34}],23:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class RadioElement extends ViewElement_1.ViewElement {
}
exports.RadioElement = RadioElement;

},{"./ViewElement":34}],24:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class RichTextElement extends ViewElement_1.ViewElement {
}
exports.RichTextElement = RichTextElement;

},{"./ViewElement":34}],25:[function(require,module,exports){
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

},{"./V":33,"./ViewElement":34}],26:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class SliderElement extends ViewElement_1.ViewElement {
}
exports.SliderElement = SliderElement;

},{"./ViewElement":34}],27:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });

},{}],28:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class SwiperElement extends ViewElement_1.ViewElement {
}
exports.SwiperElement = SwiperElement;

},{"./ViewElement":34}],29:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class SwitchElement extends ViewElement_1.ViewElement {
}
exports.SwitchElement = SwitchElement;

},{"./ViewElement":34}],30:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const ViewElement_1 = require("./ViewElement");
class TextElement extends ViewElement_1.ViewElement {
}
exports.TextElement = TextElement;

},{"./ViewElement":34}],31:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const InputElement_1 = require("./InputElement");
class TextareaElement extends InputElement_1.InputElement {
}
exports.TextareaElement = TextareaElement;

},{"./InputElement":13}],32:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function resolveURI(uri) {
    if (uri.indexOf('://') < 0) {
        let v = window.__basePath;
        if (v !== undefined) {
            return v + '/' + uri;
        }
    }
    return uri;
}
exports.resolveURI = resolveURI;

},{}],33:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function booleanValue(v) {
    return v == 'true';
}
exports.booleanValue = booleanValue;

},{}],34:[function(require,module,exports){
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

},{"./Element":6}],35:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var _running = false;
var _funcs = [];
function run() {
    var fn;
    while ((fn = _funcs.shift())) {
        fn();
    }
    _running = true;
}
function once(func) {
    _funcs.push(func);
    if (!_running) {
        _running = true;
        setTimeout(function () {
            run();
        }, 0);
    }
}
exports.once = once;

},{}],36:[function(require,module,exports){

require('./bin/Data.js');
require('./bin/V.js');
require('./bin/URI.js');
require('./bin/IPC.js');
require('./bin/once.js');
require('./bin/Event.js');
require('./bin/EventEmitter.js');
require('./bin/Style.js');
require('./bin/Element.js');
require('./bin/ViewElement.js');
require('./bin/ImageElement.js');
require('./bin/NViewElement.js');
require('./bin/InputElement.js');
require('./bin/ScrollViewElement.js');
require('./bin/SwiperElement.js');
require('./bin/MovableViewElement.js');
require('./bin/IconElement.js');
require('./bin/TextElement.js');
require('./bin/RichTextElement.js');
require('./bin/ProgressElement.js');
require('./bin/ButtonElement.js');
require('./bin/CheckboxElement.js');
require('./bin/FormElement.js');
require('./bin/LabelElement.js');
require('./bin/PickerElement.js');
require('./bin/PickerViewElement.js');
require('./bin/RadioElement.js');
require('./bin/SliderElement.js');
require('./bin/SwitchElement.js');
require('./bin/TextareaElement.js');
require('./bin/NavigatorElement.js');
require('./bin/CanvasElement.js');

require('./bin/Document.js');
require('./bin/Page.js');

kk = require('./bin/Main.js');


},{"./bin/ButtonElement.js":1,"./bin/CanvasElement.js":2,"./bin/CheckboxElement.js":3,"./bin/Data.js":4,"./bin/Document.js":5,"./bin/Element.js":6,"./bin/Event.js":7,"./bin/EventEmitter.js":8,"./bin/FormElement.js":9,"./bin/IPC.js":10,"./bin/IconElement.js":11,"./bin/ImageElement.js":12,"./bin/InputElement.js":13,"./bin/LabelElement.js":14,"./bin/Main.js":15,"./bin/MovableViewElement.js":16,"./bin/NViewElement.js":17,"./bin/NavigatorElement.js":18,"./bin/Page.js":19,"./bin/PickerElement.js":20,"./bin/PickerViewElement.js":21,"./bin/ProgressElement.js":22,"./bin/RadioElement.js":23,"./bin/RichTextElement.js":24,"./bin/ScrollViewElement.js":25,"./bin/SliderElement.js":26,"./bin/Style.js":27,"./bin/SwiperElement.js":28,"./bin/SwitchElement.js":29,"./bin/TextElement.js":30,"./bin/TextareaElement.js":31,"./bin/URI.js":32,"./bin/V.js":33,"./bin/ViewElement.js":34,"./bin/once.js":35}]},{},[36]);
