import { getApp } from "../App";
import { PageOptions } from '../Page';

let appid: number = query.appid;
var refCount = getApp(appid)!;
var wx_app = refCount.object

refCount.retain();

var pageOptions: PageOptions[] = [];

function loadPage(index: number): void {
    let item = wx_app.options.tabBar.list[index];
    let p = pageOptions[index];
    if (p === undefined) {
        let v = app.getTextContent(wx_app.basePath + "/" + item.pagePath + ".json");
        if (v !== undefined) {
            p = JSON.parse(v);
            pageOptions[index] = p;
        }
    }
    page.setOptions({
        title: p.navigationBarTitleText || wx_app.options.window.navigationBarTitleText
    })
}

loadPage(0);

ker.Page({
    path: path,
    data: {
        appid: wx_app.appid,
        selectedIndex: 0,
        items: wx_app.options.tabBar.list,
        bottombar: {
            height: platform == 'iOS' && screen.height >= 812 ? '74px' : '50px',
            padding: platform == 'iOS' && screen.height >= 812 ? '0px 0px 24px 0px' : '0px',
        }
    },
    onload: function (document: Document): void {

    },
    onunload: function (): void {
        refCount.release();
    },
    onTabAction: function (e: ElementEvent): void {
        let dataSet = e.dataSet;
        this.setData({ selectedIndex: dataSet.index });
        loadPage(dataSet.index);
    },

}, page, setTimeout);
