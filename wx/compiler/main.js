
var path = require('path');
var fs = require('fs');
var Page = require("./page.js");
var App = require("./app.js");

var app = new App(process.argv[process.argv.length - 1]);

app.compile();

walk(process.argv[process.argv.length - 1]);

function walk(p,basePath) {

    if(basePath === undefined) {
        basePath = p;
    }

    if(p.endsWith(".wxml")) {

        console.info(path.relative(basePath,p));

        var page = new Page(p,basePath);
        
        page.compile();

    } else {
        var st = fs.statSync(p);
        if(st && st.isDirectory()) {
            for(var item of fs.readdirSync(p)) {
                walk(path.join(p,item),basePath);
            }
        }
    }
}
