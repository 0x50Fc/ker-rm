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
            this.changedKeys([]);
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
            idx = index + exaluate.length;
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
//# sourceMappingURL=Data.js.map