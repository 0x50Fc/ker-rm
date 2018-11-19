
import { Event } from "./Event";

type EventFunction = (event: Event) => void;

interface EventMap {
    [key: string]: EventFunction[]
}

export class EventEmitter {

    private _events: EventMap = {};

    public on(name: string, func: EventFunction): void {
        var v: EventFunction[] = this._events[name];
        if (v === undefined) {
            v = [];
            this._events[name] = v;
        }
        v.push(func);
    }

    public off(name?: string, func?: EventFunction): void {
        if (name === undefined && func === undefined) {
            this._events = {};
        } else if (func === undefined && name !== undefined) {
            delete this._events[name];
        } else if (name !== undefined) {
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

    public emit(name: string, event: Event): void {
        var v = this._events[name];
        if (v !== undefined) {
            var vs = v.slice();
            for (let fn of vs) {
                fn(event);
            }
        }
    }

    public has(name: string): boolean {
        return this._events[name] !== undefined;
    }


}