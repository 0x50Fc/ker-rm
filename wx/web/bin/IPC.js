"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function postMessage(data) {
    let w = window;
    if (w.webkit && w.webkit.messageHandlers && w.webkit.messageHandlers.kk) {
        w.webkit.messageHandlers.kk.postMessage(data);
    }
}
exports.postMessage = postMessage;
//# sourceMappingURL=IPC.js.map