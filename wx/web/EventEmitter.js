"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var EventEmitter = /** @class */ (function () {
    function EventEmitter() {
        this._events = {};
    }
    EventEmitter.prototype.on = function (name, func) {
        var v = this._events[name];
        if (v === undefined) {
            v = [];
            this._events[name] = v;
        }
        v.push(func);
    };
    EventEmitter.prototype.off = function (name, func) {
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
                for (var _i = 0, v_1 = v; _i < v_1.length; _i++) {
                    var fn = v_1[_i];
                    if (fn != func) {
                        vs.push(fn);
                    }
                }
                this._events[name] = vs;
            }
        }
    };
    EventEmitter.prototype.emit = function (name, event) {
        var v = this._events[name];
        if (v !== undefined) {
            var vs = v.slice();
            for (var _i = 0, vs_1 = vs; _i < vs_1.length; _i++) {
                var fn = vs_1[_i];
                fn(event);
            }
        }
    };
    EventEmitter.prototype.has = function (name) {
        return this._events[name] !== undefined;
    };
    return EventEmitter;
}());
exports.EventEmitter = EventEmitter;
