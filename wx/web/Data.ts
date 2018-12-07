import { int } from "./declare";

export type EvaluateScript = any;

interface KeySet {
    [key: string]: boolean
}

export class Evaluate {
    evaluateScript: EvaluateScript;
    keys: string[][];
    keySet: string[];

    constructor(evaluateScript: EvaluateScript, keys: string[][]) {
        this.evaluateScript = evaluateScript;
        this.keys = keys;
        this.keySet = [];
        let m: KeySet = {};
        for (let key of keys) {
            if (!m[key[0]]) {
                m[key[0]] = true;
                this.keySet.push(key[0]);
            }
        }
    }
    exec(object: any): any {
        var vs = [];
        for (let key of this.keySet) {
            let v = object[key];
            if (v === undefined) {
                v = (window as TObject)[key];
            }
            vs.push(v);
        }
        try {
            return this.evaluateScript.apply(undefined, vs);
        } catch (e) {
            console.error("[Evaluate]", e);
        }

    }
}

export class IObject {

    [key: string]: any;

    get(key: string): any {
        return this[key];
    }

    set(key: string, value: any): void {
        if (value === undefined) {
            delete this[key];
        } else {
            this[key] = value;
        }
    }
}

export function get(object: any, keys: string[], index: number = 0): any {

    if (index < keys.length) {

        var key = keys[index];

        if (typeof object == 'object') {
            if (object instanceof IObject) {
                return get(object.get(key), keys, index + 1);
            }
            return get(object[key], keys, index + 1);
        }

    } else {
        return object;
    }
}

export function set(object: any, keys: string[], value: any, index: number = 0): void {

    if (typeof object != 'object') {
        return;
    }

    if (index + 1 < keys.length) {

        var key = keys[index];

        var v;

        if (object instanceof IObject) {
            v = object.get(key);
        } else {
            v = object[key];
        }

        if (v === undefined) {
            v = {};
            if (object instanceof IObject) {
                object.set(key, v);
            } else {
                object[key] = v;
            }
        }

        set(v, keys, value, index + 1);

    } else if (index < keys.length) {
        var key = keys[index];
        if (object instanceof IObject) {
            object.set(key, value);
        } else {
            object[key] = value;
        }
    }
}

export type DataFunction = (value: any, changedKeys: string[]) => void;


interface TObject {
    [key: string]: any
}

class KeyCallback {

    hasChildren: boolean = false;
    keys: string[] | undefined;
    priority: number = 0;
    evaluate: Evaluate | undefined;
    func: DataFunction;

    constructor(func: DataFunction) {
        this.func = func;
    }

    run(object: any, changedKeys: string[]): void {

        var v: any;

        if (this.evaluate !== undefined) {
            v = this.evaluate.exec(object);
        } else if (this.keys !== undefined) {
            v = get(object, this.keys);
        }

        this.func(v, changedKeys);
    }

}

interface KeyObserverMap {
    [key: string]: KeyObserver
}

class KeyObserver {
    children: KeyObserverMap = {};
    callbacks: KeyCallback[] = [];
    add(keys: string[], callback: KeyCallback, index: int): void {
        if (index < keys.length) {
            let key = keys[index];
            var ch = this.children[key];
            if (ch === undefined) {
                ch = new KeyObserver();
                this.children[key] = ch;
            }
            ch.add(keys, callback, index + 1);
        } else {
            this.callbacks.push(callback);
        }
    }
    remove(keys: string[], func: DataFunction | undefined, index: int): void {
        if (func === undefined) {
            this.children = {};
            this.callbacks = [];
        } else if (index < keys.length) {
            let key = keys[index];
            let ch = this.children[key];
            if (ch !== undefined) {
                ch.remove(keys, func, index + 1);
            }
        } else {
            let cbs: KeyCallback[] = [];
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
    change(keys: string[], callbacks: KeyCallback[], index: int): void {
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
        } else {
            for (let cb of this.callbacks) {
                callbacks.push(cb);
            }
            for (let key in this.children) {
                var ch = this.children[key];
                ch.change(keys, callbacks, index);
            }
        }
    }
    changedKeys(object: any, keys: string[]): void {

        let callbacks: KeyCallback[] = [];
        this.change(keys, callbacks, 0);
        callbacks.sort((a: KeyCallback, b: KeyCallback): number => {
            return a.priority - b.priority;
        });
        for (let cb of callbacks) {
            cb.run(object, keys);
        }
    }
    on(object: any, keys: string[] | Evaluate, func: DataFunction, hasChildren: boolean = false, priority: number = 0): void {
        let onKeys: string[][] = [];
        let cb = new KeyCallback(func);
        cb.hasChildren = hasChildren;
        cb.priority = priority;

        if (keys instanceof Evaluate) {
            onKeys = keys.keys;
            cb.evaluate = keys;
        } else {
            cb.keys = keys;
            onKeys.push(keys);
        }

        if (onKeys.length == 0) {

            var vv: any;

            if (cb.evaluate !== undefined) {
                try {
                    vv = cb.evaluate.exec(object);
                } catch (e) {
                    console.info("[ERROR] " + e);
                }
            }

            if (vv !== undefined) {
                func(vv, []);
            }
        } else {
            for (let ks of onKeys) {
                this.add(ks, cb, 0);
            }
        }
    }
    off(keys: string[], func: DataFunction | undefined): void {
        this.remove(keys, func, 0);
    }
}

export class Data {

    private _parent: Data | undefined;
    private _parentFunc: DataFunction | undefined;
    private _keyObserver: KeyObserver = new KeyObserver();

    public object: any = {};

    public get(keys: string[]): any {
        return get(this.object, keys);
    }

    public set(keys: string[], value: any, changed: boolean = true): void {
        set(this.object, keys, value);
        if (changed === true) {
            this.changedKeys(keys);
        }
    }

    public changedKeys(keys: string[]): void {
        this._keyObserver.changedKeys(this.object, keys);
    }

    public on(keys: string[] | Evaluate, func: DataFunction, hasChildren: boolean = false, priority: number = 0): void {
        this._keyObserver.on(this.object, keys, func, hasChildren, priority);
    }

    public off(keys: string[], func: ((value: any, changedKeys: string[]) => void) | undefined): void {
        this._keyObserver.off(keys, func);
    }

    public setParent(parent: Data | undefined) {
        this.recycle();
        if (parent !== undefined) {
            this._parent = parent;
            let data = this;
            this._parentFunc = function (value: any, keys: string[]) {
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

    public recycle(): void {
        if (this._parent !== undefined) {
            this._parent.off([], this._parentFunc);
            this._parent = undefined;
            this._parentFunc = undefined;
        }
    }

}
