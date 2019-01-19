"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var autoId = 0;
var cur = 0;
var speed = 1000 / 60;
var frames = {};
function add(frame) {
    var id = ++autoId;
    frames[id] = frame;
    return id;
}
function remove(id) {
    delete frames[id];
}
setInterval(function () {
    cur += speed;
    var rm = [];
    for (var id in frames) {
        var fn = frames[id];
        if (fn(cur) === false) {
            rm.push(id);
        }
    }
    for (var _i = 0, rm_1 = rm; _i < rm_1.length; _i++) {
        var id = rm_1[_i];
        delete frames[id];
    }
}, speed);
function startAnimation(duration, func) {
    var start = cur;
    return add(function (cur) {
        var v = (cur - start) / duration;
        if (v >= 1) {
            v = 1;
            func(v);
            return false;
        }
        func(v);
        return true;
    });
}
exports.startAnimation = startAnimation;
;
function clearAnimation(id) {
    remove(id);
}
exports.clearAnimation = clearAnimation;
;
