"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function resolveURI(uri) {
    if (uri.indexOf('://') < 0) {
        let v = window.__basePath;
        if (v !== undefined) {
            return v + '/' + uri;
        }
    }
    return uri;
}
exports.resolveURI = resolveURI;
//# sourceMappingURL=URI.js.map