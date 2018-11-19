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
//# sourceMappingURL=EventEmitter.js.map