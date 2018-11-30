
var path = require('path');
var fs = require('fs');
var Page = require("./page.js");
var App = require("./app.js");

var basePath = process.argv[2];
var outPath = process.argv[3];

var app = new App(basePath, outPath);

app.compile();

walk(basePath);

function walk(p) {

    if (p.endsWith(".wxml")) {

        console.info("[PAGE]", p);

        var page = new Page(p, basePath, outPath);

        page.compile();

    } else {
        var st = fs.statSync(p);
        if (st && st.isDirectory()) {
            for (var item of fs.readdirSync(p)) {
                walk(path.join(p, item), basePath);
            }
        }
    }
}
