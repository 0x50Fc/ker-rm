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
//# sourceMappingURL=once.js.map