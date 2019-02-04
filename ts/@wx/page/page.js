"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var App_1 = require("../App");
var appid = query.appid;
var refCount = App_1.getApp(appid);
var wx_app = refCount.object;
refCount.retain();
ker.Page({
    path: path,
    data: {
        appid: wx_app.appid,
        bottombar: {
            height: platform == 'iOS' && screen.height >= 812 ? '74px' : '50px',
            padding: platform == 'iOS' && screen.height >= 812 ? '0px 0px 24px 0px' : '0px',
        },
    },
    onload: function (document) {
    },
    onunload: function () {
        refCount.release();
    }
}, page, setTimeout);
