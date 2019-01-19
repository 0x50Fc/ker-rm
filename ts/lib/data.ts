
namespace ker {

    export interface DataObject {
        [key: string]: any
    }

    export interface DataKeySet {
        [key: string]: boolean
    }


    export type EvaluateScript = any;

    export class Evaluate {

        evaluateScript: EvaluateScript
        keys: string[]
        keySet: DataKeySet

        constructor(evaluateScript: EvaluateScript, keys: string[]) {
            this.evaluateScript = evaluateScript;
            this.keys = keys;
            this.keySet = {};
            for (let key of keys) {
                this.keySet[key] = true;
            }
        }

        exec(object: DataObject, global: DataObject): any {
            var vs = [];
            for (let key of this.keys) {
                let v = object[key];
                if (v === undefined) {
                    v = global[key];
                }
                vs.push(v);
            }
            try {
                return this.evaluateScript.apply(undefined, vs);
            } catch (e) {
                console.error("[Evaluate:exec]", e);
            }

        }
    }

    export type DataFunction = (value: any, changedKeys: DataKeySet) => void;

    class KeyCallback {

        keys: string[] | undefined;
        priority: number = 0;
        evaluate: Evaluate | undefined;
        func: DataFunction;

        constructor(func: DataFunction) {
            this.func = func;
        }

        run(object: DataObject, global: DataObject, changedKeys: DataKeySet): void {

            var v: any;

            if (this.evaluate !== undefined) {
                v = this.evaluate.exec(object, global);
            } else if (this.keys !== undefined) {
                v = Data.get(object, this.keys);
            }

            this.func(v, changedKeys);
        }

    }

    interface KeyObserver {
        [key: string]: KeyCallback[] | undefined
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


    export class Data {

        private _object: DataObject
        private _keySet: DataKeySet | undefined
        private _global: DataObject
        private _keyObserver: KeyObserver
        private _parent: Data | undefined
        private _onDataFunction: DataFunction | undefined

        constructor(global: DataObject) {
            this._object = {};
            this._global = global;
            this._keyObserver = {};
        }

        get object(): DataObject {
            return this._object;
        }

        set object(object: DataObject) {
            this._object = object;
        }

        setData(data: DataObject, changed: boolean = true): void {
            if(changed) {
                this.begin();
            }
            for (var key in data) {
                this._object[key] = data[key];
                if (this._keySet !== undefined) {
                    this._keySet[key] = true;
                }
            }
            if(changed) {
                this.commit();
            }
        }

        begin(): void {
            this._keySet = {};
        }

        commit(): void {
            let keys = this._keySet;
            this._keySet = undefined;
            if (keys !== undefined) {
                this.changeKeys(keys);
            }
        }

        cancel(): void {
            this._keySet = undefined;
        }

        changeKeys(keySet: DataKeySet): void {

            let cbs: KeyCallback[] = [];

            for (let key in keySet) {
                let v = this._keyObserver[key];
                if (v !== undefined) {
                    cbs = cbs.concat(v);
                }
            }

            cbs.sort((a: KeyCallback, b: KeyCallback): number => {
                return a.priority - b.priority;
            });

            for (let cb of cbs) {
                cb.run(this._object, this._global, keySet);
            }
        }

        on(keys: string[] | Evaluate, func: DataFunction, priority: number = 0): void {
            let cb: KeyCallback;
            if (keys instanceof Evaluate) {
                cb = new KeyCallback(func)
                cb.priority = priority
                cb.keys = keys.keys
                cb.evaluate = keys;
            } else {
                cb = new KeyCallback(func)
                cb.priority = priority
                cb.keys = keys
            }
            for (let key of cb.keys) {
                let vs = this._keyObserver[key];
                if (vs === undefined) {
                    this._keyObserver[key] = [cb];
                } else {
                    vs.push(cb);
                }
            }
        }

        off(keys: string[] | Evaluate, func: DataFunction | undefined): void {
            if (keys instanceof Evaluate) {
                keys = keys.keys;
            }
            for (let key of keys) {
                if (func === undefined) {
                    delete this._keyObserver[key];
                } else {
                    let cbs = this._keyObserver[key];
                    if (cbs !== undefined) {
                        let vs: KeyCallback[] = [];
                        for (let cb of cbs) {
                            if (cb.func !== func) {
                                vs.push(cb);
                            }
                        }
                        this._keyObserver[key] = vs;
                    }
                }

            }
        }

        setParent(parent: Data | undefined): void {
            this.recycle();
            if (parent !== undefined) {
                this._parent = parent;
                let data = this;
                this._onDataFunction = (value: any, keySet: DataKeySet): void => {
                    if (value !== undefined) {
                        data.begin();
                        for (let key in keySet) {
                            data.set([key], value[key], false);
                        }
                        data.commit();
                    }
                };
                parent.on([], this._onDataFunction);
                let object = parent.object;
                for (var key in object) {
                    this._object[key] = object[key];
                }
            }
        }

        recycle(): void {
            if (this._parent !== undefined) {
                this._parent.off([], this._onDataFunction);
                this._parent = undefined;
                this._onDataFunction = undefined;
            }
        }

        get(keys: string[]): any {
            return Data.get(this._object, keys);
        }

        set(keys: string[], value: any, changed: boolean = true): void {
            if (changed) {
                this.begin();
            }
            Data.set(this._object, keys, value);
            if (changed) {
                this.commit();
            }
        }

        static get(object: any, keys: string[], index: number = 0): any {

            if (index < keys.length) {

                var key = keys[index];

                if (typeof object == 'object') {
                    if (object instanceof IObject) {
                        return Data.get(object.get(key), keys, index + 1);
                    }
                    return Data.get(object[key], keys, index + 1);
                }

            } else {
                return object;
            }
        }

        static set(object: any, keys: string[], value: any, index: number = 0): void {

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

                Data.set(v, keys, value, index + 1);

            } else if (index < keys.length) {
                var key = keys[index];
                if (object instanceof IObject) {
                    object.set(key, value);
                } else {
                    object[key] = value;
                }
            }
        }

    }

}


