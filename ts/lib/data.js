var ker;
(function (ker) {
    var Evaluate = /** @class */ (function () {
        function Evaluate(evaluateScript, keys) {
            this.evaluateScript = evaluateScript;
            this.keys = keys;
        }
        Evaluate.prototype.exec = function (object, global) {
            var vs = [];
            for (var _i = 0, _a = this.keys; _i < _a.length; _i++) {
                var key = _a[_i];
                var v = object[key];
                if (v === undefined) {
                    v = global[key];
                }
                vs.push(v);
            }
            try {
                return this.evaluateScript.apply(undefined, vs);
            }
            catch (e) {
                console.error("[Evaluate:exec]", e.stack);
            }
        };
        return Evaluate;
    }());
    ker.Evaluate = Evaluate;
    var KeyCallback = /** @class */ (function () {
        function KeyCallback(func) {
            this.priority = 0;
            this.func = func;
        }
        KeyCallback.prototype.run = function (object, global, changedKeys) {
            var v;
            if (this.evaluate !== undefined) {
                v = this.evaluate.exec(object, global);
            }
            else if (this.keys !== undefined) {
                v = Data.get(object, this.keys);
            }
            this.func(v, changedKeys);
        };
        return KeyCallback;
    }());
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
    ker.IObject = IObject;
    var Data = /** @class */ (function () {
        function Data(global) {
            this._object = {};
            this._global = global;
            this._keyObserver = {};
            this._defaultObserver = [];
        }
        Object.defineProperty(Data.prototype, "global", {
            get: function () {
                return this._global;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(Data.prototype, "object", {
            get: function () {
                return this._object;
            },
            set: function (object) {
                this._object = object;
            },
            enumerable: true,
            configurable: true
        });
        Data.prototype.setData = function (data, changed) {
            if (changed === void 0) { changed = true; }
            if (changed) {
                this.begin();
            }
            for (var key in data) {
                this._object[key] = data[key];
                if (this._keySet !== undefined) {
                    this._keySet[key] = true;
                }
            }
            if (changed) {
                this.commit();
            }
        };
        Data.prototype.begin = function () {
            this._keySet = {};
        };
        Data.prototype.commit = function () {
            var keys = this._keySet;
            this._keySet = undefined;
            if (keys !== undefined) {
                this.changeKeys(keys);
            }
        };
        Data.prototype.cancel = function () {
            this._keySet = undefined;
        };
        Data.prototype.changeKeys = function (keySet) {
            var cbs = [].concat(this._defaultObserver);
            if (keySet === undefined) {
                for (var key in this._keyObserver) {
                    var v = this._keyObserver[key];
                    if (v !== undefined) {
                        cbs = cbs.concat(v);
                    }
                }
            }
            else {
                for (var key in keySet) {
                    var v = this._keyObserver[key];
                    if (v !== undefined) {
                        cbs = cbs.concat(v);
                    }
                }
            }
            cbs.sort(function (a, b) {
                return a.priority - b.priority;
            });
            for (var _i = 0, cbs_1 = cbs; _i < cbs_1.length; _i++) {
                var cb = cbs_1[_i];
                cb.run(this._object, this._global, keySet);
            }
        };
        Data.prototype.on = function (keys, func, priority) {
            if (priority === void 0) { priority = 0; }
            var cb;
            if (keys instanceof Evaluate) {
                cb = new KeyCallback(func);
                cb.priority = priority;
                cb.keys = keys.keys;
                cb.evaluate = keys;
            }
            else {
                cb = new KeyCallback(func);
                cb.priority = priority;
                cb.keys = keys;
            }
            if (cb.keys.length == 0) {
                this._defaultObserver.push(cb);
            }
            else {
                for (var _i = 0, _a = cb.keys; _i < _a.length; _i++) {
                    var key = _a[_i];
                    var vs = this._keyObserver[key];
                    if (vs === undefined) {
                        this._keyObserver[key] = [cb];
                    }
                    else {
                        vs.push(cb);
                    }
                }
            }
        };
        Data.prototype.off = function (keys, func) {
            if (keys instanceof Evaluate) {
                keys = keys.keys;
            }
            if (keys.length == 0) {
                if (func === undefined) {
                    this._defaultObserver = [];
                }
                else {
                    var vs = [];
                    for (var _i = 0, _a = this._defaultObserver; _i < _a.length; _i++) {
                        var cb = _a[_i];
                        if (cb.func !== func) {
                            vs.push(cb);
                        }
                    }
                    this._defaultObserver = vs;
                }
            }
            else {
                for (var _b = 0, keys_1 = keys; _b < keys_1.length; _b++) {
                    var key = keys_1[_b];
                    if (func === undefined) {
                        delete this._keyObserver[key];
                    }
                    else {
                        var cbs = this._keyObserver[key];
                        if (cbs !== undefined) {
                            var vs = [];
                            for (var _c = 0, cbs_2 = cbs; _c < cbs_2.length; _c++) {
                                var cb = cbs_2[_c];
                                if (cb.func !== func) {
                                    vs.push(cb);
                                }
                            }
                            this._keyObserver[key] = vs;
                        }
                    }
                }
            }
        };
        Data.prototype.setParent = function (parent) {
            this.recycle();
            if (parent !== undefined) {
                this._parent = parent;
                var data_1 = this;
                this._onDataFunction = function (value, keySet) {
                    if (value !== undefined) {
                        if (keySet === undefined) {
                            data_1.changeKeys();
                        }
                        else {
                            data_1.begin();
                            for (var key_1 in keySet) {
                                data_1.set([key_1], value[key_1], false);
                            }
                            data_1.commit();
                        }
                    }
                };
                parent.on([], this._onDataFunction);
                var object = parent.object;
                for (var key in object) {
                    this._object[key] = object[key];
                }
            }
        };
        Data.prototype.recycle = function () {
            if (this._parent !== undefined) {
                this._parent.off([], this._onDataFunction);
                this._parent = undefined;
                this._onDataFunction = undefined;
            }
        };
        Data.prototype.get = function (keys) {
            return Data.get(this._object, keys);
        };
        Data.prototype.set = function (keys, value, changed) {
            if (changed === void 0) { changed = true; }
            if (changed) {
                this.begin();
            }
            Data.set(this._object, keys, value);
            if (this._keySet !== undefined && keys.length > 0) {
                this._keySet[keys[0]] = true;
            }
            if (changed) {
                this.commit();
            }
        };
        Data.get = function (object, keys, index) {
            if (index === void 0) { index = 0; }
            if (index < keys.length) {
                var key = keys[index];
                if (typeof object == 'object') {
                    if (object instanceof IObject) {
                        return Data.get(object.get(key), keys, index + 1);
                    }
                    return Data.get(object[key], keys, index + 1);
                }
            }
            else {
                return object;
            }
        };
        Data.set = function (object, keys, value, index) {
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
                Data.set(v, keys, value, index + 1);
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
        };
        return Data;
    }());
    ker.Data = Data;
})(ker || (ker = {}));
