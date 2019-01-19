"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function booleanValue(v) {
    return v !== undefined && v != 'false' && v != '0';
}
exports.booleanValue = booleanValue;
function pixelValue(v) {
    if (v && v.endsWith("rpx")) {
        var r = parseFloat(document.documentElement.style.fontSize) || 14;
        return parseFloat(v) * r * 0.05;
    }
    else if (v) {
        return parseFloat(v) || 0;
    }
    return 0;
}
exports.pixelValue = pixelValue;
function nativePixelValue(v) {
    if (/Android/i.test(navigator.userAgent)) {
        return v * window.devicePixelRatio;
    }
    return v;
}
exports.nativePixelValue = nativePixelValue;
function pixelStringValue(v) {
    if (v && v.endsWith("rpx")) {
        return (parseFloat(v) * 0.05) + 'rem';
    }
    return v;
}
exports.pixelStringValue = pixelStringValue;
function parseStyleValue(v) {
    if (v) {
        return v.replace(/([0-9\.\+\-]+)rpx/g, function (text, v) {
            return (parseFloat(v) * 0.05) + 'rem';
        });
    }
    return '';
}
exports.parseStyleValue = parseStyleValue;
