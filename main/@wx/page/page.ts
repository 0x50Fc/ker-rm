import { getApp } from "../App";
import { PageOptions } from '../Page';

let appid: number = query.appid;
var refCount = getApp(appid)!;
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
    onload: function (document: Document): void {

    },
    onunload: function (): void {
        refCount.release();
    }

}, page, setTimeout);
