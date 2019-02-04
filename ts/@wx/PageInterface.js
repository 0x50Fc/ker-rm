"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var wx = require("./wx");
var PageInterface = /** @class */ (function () {
    function PageInterface(app, basePath) {
        this.app = app;
        this.basePath = basePath;
    }
    PageInterface.prototype.request = function (object) {
        return wx.request(object);
    };
    PageInterface.prototype.downloadFile = function (object) {
        return wx.downloadFile(object);
    };
    PageInterface.prototype.uploadFile = function (object) {
        return wx.uploadFile(object);
    };
    PageInterface.prototype.reportAnalytics = function (name, data) {
        return wx.reportAnalytics(name, data);
    };
    PageInterface.prototype.saveFile = function (object) {
        return wx.saveFile(object);
    };
    PageInterface.prototype.removeSavedFile = function (object) {
        return wx.removeSavedFile(object);
    };
    PageInterface.prototype.navigateTo = function (object) {
        var url = object.url;
        var i = url.indexOf("?");
        var query = {};
        var path;
        if (i > 0) {
            path = url.substr(0, i);
            var vs = url.substr(i + 1).split("&");
            for (var _i = 0, vs_1 = vs; _i < vs_1.length; _i++) {
                var v = vs_1[_i];
                var kv = v.split("=");
                query[kv[0]] = kv.length > 1 ? decodeURIComponent(kv[1]) : "";
            }
        }
        else {
            path = url;
        }
        this.app.open(this.basePath + '/' + path, query);
    };
    return PageInterface;
}());
exports.PageInterface = PageInterface;
