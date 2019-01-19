"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var readys = [];
window.addEventListener("ker", function () {
    var w = window;
    while (readys.length > 0) {
        var func = readys.shift();
        if (func !== undefined) {
            func();
        }
    }
});
function postMessage(data) {
    var w = window;
    if (w.webkit && w.webkit.messageHandlers && w.webkit.messageHandlers.ker) {
        w.webkit.messageHandlers.ker.postMessage(data);
    }
    else if (w.ker) {
        w.ker.postMessage(JSON.stringify(data));
    }
    else {
        ready(function () {
            w.ker.postMessage(JSON.stringify(data));
        });
    }
}
exports.postMessage = postMessage;
function ready(func) {
    var w = window;
    if (w.webkit && w.webkit.messageHandlers && w.webkit.messageHandlers.ker) {
        func();
    }
    else if (w.ker) {
        func();
    }
    else {
        readys.push(func);
    }
}
exports.ready = ready;
