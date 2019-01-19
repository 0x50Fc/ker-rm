"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var Evaluate = /** @class */ (function () {
    function Evaluate(evaluateScript, keys) {
        this.evaluateScript = evaluateScript;
        this.keys = keys;
        this.keySet = [];
        var m = {};
        for (var _i = 0, keys_1 = keys; _i < keys_1.length; _i++) {
            var key = keys_1[_i];
            if (!m[key[0]]) {
                m[key[0]] = true;
                this.keySet.push(key[0]);
            }
        }
    }
    Evaluate.prototype.exec = function (object) {
        var vs = [];
        for (var _i = 0, _a = this.keySet; _i < _a.length; _i++) {
            var key = _a[_i];
            var v = object[key];
            if (v === undefined) {
                v = window[key];
            }
            vs.push(v);
        }
        try {
            return this.evaluateScript.apply(undefined, vs);
        }
        catch (e) {
            console.error("[Evaluate]", e);
        }
    };
    return Evaluate;
}());
exports.Evaluate = Evaluate;
var IObject = /** @class */ (function () {
    function IObject() {
    }
    IObject.prototype.get = function (key) {
        return this[key];
    };
    IObject.prototype.set = function (key, value) {
        if (value === undefined) {
            delete this[key];
        }
        else {
            this[key] = value;
        }
    };
    return IObject;
}());
exports.IObject = IObject;
function get(object, keys, index) {
    if (index === void 0) { index = 0; }
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
function set(object, keys, value, index) {
    if (index === void 0) { index = 0; }
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
var KeyCallback = /** @class */ (function () {
    function KeyCallback(func) {
        this.hasChildren = false;
        this.priority = 0;
        this.func = func;
    }
    KeyCallback.prototype.run = function (object, changedKeys) {
        var v;
        if (this.evaluate !== undefined) {
            v = this.evaluate.exec(object);
        }
        else if (this.keys !== undefined) {
            v = get(object, this.keys);
        }
        this.func(v, changedKeys);
    };
    return KeyCallback;
}());
var KeyObserver = /** @class */ (function () {
    function KeyObserver() {
        this.children = {};
        this.callbacks = [];
    }
    KeyObserver.prototype.add = function (keys, callback, index) {
        if (index < keys.length) {
            var key = keys[index];
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
    };
    KeyObserver.prototype.remove = function (keys, func, index) {
        if (func === undefined) {
            this.children = {};
            this.callbacks = [];
        }
        else if (index < keys.length) {
            var key = keys[index];
            var ch_1 = this.children[key];
            if (ch_1 !== undefined) {
                ch_1.remove(keys, func, index + 1);
            }
        }
        else {
            var cbs = [];
            for (var _i = 0, _a = this.callbacks; _i < _a.length; _i++) {
                var cb = _a[_i];
                if (cb.func != func) {
                    cbs.push(cb);
                }
            }
            this.callbacks = cbs;
            for (var key in this.children) {
                var ch = this.children[key];
                ch.remove(keys, func, index);
            }
        }
    };
    KeyObserver.prototype.change = function (keys, callbacks, index) {
        if (index < keys.length) {
            var key = keys[index];
            var ch_2 = this.children[key];
            if (ch_2 !== undefined) {
                ch_2.change(keys, callbacks, index + 1);
            }
            for (var _i = 0, _a = this.callbacks; _i < _a.length; _i++) {
                var cb = _a[_i];
                if (cb.hasChildren) {
                    callbacks.push(cb);
                }
            }
        }
        else {
            for (var _b = 0, _c = this.callbacks; _b < _c.length; _b++) {
                var cb = _c[_b];
                callbacks.push(cb);
            }
            for (var key in this.children) {
                var ch = this.children[key];
                ch.change(keys, callbacks, index);
            }
        }
    };
    KeyObserver.prototype.changedKeys = function (object, keys) {
        var callbacks = [];
        this.change(keys, callbacks, 0);
        callbacks.sort(function (a, b) {
            return a.priority - b.priority;
        });
        for (var _i = 0, callbacks_1 = callbacks; _i < callbacks_1.length; _i++) {
            var cb = callbacks_1[_i];
            cb.run(object, keys);
        }
    };
    KeyObserver.prototype.on = function (object, keys, func, hasChildren, priority) {
        if (hasChildren === void 0) { hasChildren = false; }
        if (priority === void 0) { priority = 0; }
        var onKeys = [];
        var cb = new KeyCallback(func);
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
            for (var _i = 0, onKeys_1 = onKeys; _i < onKeys_1.length; _i++) {
                var ks = onKeys_1[_i];
                this.add(ks, cb, 0);
            }
        }
    };
    KeyObserver.prototype.off = function (keys, func) {
        this.remove(keys, func, 0);
    };
    return KeyObserver;
}());
var Data = /** @class */ (function () {
    function Data() {
        this._keyObserver = new KeyObserver();
        this.object = {};
    }
    Data.prototype.get = function (keys) {
        return get(this.object, keys);
    };
    Data.prototype.set = function (keys, value, changed) {
        if (changed === void 0) { changed = true; }
        set(this.object, keys, value);
        if (changed === true) {
            this.changedKeys(keys);
        }
    };
    Data.prototype.changedKeys = function (keys) {
        this._keyObserver.changedKeys(this.object, keys);
    };
    Data.prototype.on = function (keys, func, hasChildren, priority) {
        if (hasChildren === void 0) { hasChildren = false; }
        if (priority === void 0) { priority = 0; }
        this._keyObserver.on(this.object, keys, func, hasChildren, priority);
    };
    Data.prototype.off = function (keys, func) {
        this._keyObserver.off(keys, func);
    };
    Data.prototype.setParent = function (parent) {
        this.recycle();
        if (parent !== undefined) {
            this._parent = parent;
            var data_1 = this;
            this._parentFunc = function (value, keys) {
                if (value !== undefined) {
                    data_1.set(keys, get(value, keys));
                }
            };
            parent.on([], this._parentFunc, true);
            for (var key in parent.object) {
                this.object[key] = parent.object[key];
            }
        }
    };
    Data.prototype.recycle = function () {
        if (this._parent !== undefined) {
            this._parent.off([], this._parentFunc);
            this._parent = undefined;
            this._parentFunc = undefined;
        }
    };
    return Data;
}());
exports.Data = Data;
