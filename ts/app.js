
var path = require('path');
var fs = require('fs');
var UglifyJS = require("uglify-js");
var Page = require("./page.js");

function App(basePath, outPath, shPath, debug) {
    this.debug = debug;
    this.basePath = basePath;
    this.outPath = outPath === undefined ? basePath : outPath;
    this.shPath = shPath === undefined ? __dirname : shPath;
};

function mkdirs(p) {

    if (!p) {
        return;
    }

    if (fs.existsSync(p)) {
        return;
    }

    mkdirs(path.dirname(p));

    fs.mkdirSync(p);
};

function copy(from, to, filter) {

    var bname = path.basename(from);

    if (from != "." && (bname.startsWith(".") || bname == "node_modules")) {
        return;
    }

    var f = fs.statSync(from);

    if (f && f.isDirectory()) {
        if (!fs.existsSync(to)) {
            fs.mkdirSync(to);
        }
        for (var item of fs.readdirSync(from)) {
            copy(path.join(from, item), path.join(to, item), filter);
        }
    } else if (f) {
        if (filter === undefined || filter(from, to)) {
            mkdirs(path.dirname(to));
            console.info('[COPY]', from, ">>", to);
            fs.copyFileSync(from, to);
        }
    }

}


App.prototype = Object.create(Object.prototype, {
    compile: {
        value: function () {

            var debug = this.debug;

            function walk(p, basePath, outPath) {

                if (p.endsWith(".xml")) {

                    var page = new Page(p, basePath, outPath, debug);

                    page.compile();

                } else {
                    var st = fs.statSync(p);
                    if (st && st.isDirectory()) {
                        for (var item of fs.readdirSync(p)) {
                            walk(path.join(p, item), basePath, outPath);
                        }
                    }
                }
            }
            
            walk(this.basePath, this.basePath, this.outPath);

            if (this.basePath != this.outPath) {
                
                copy(this.basePath, this.outPath, function (from, to) {
                    if (from.endsWith(".js") && !debug) {
                        let v = UglifyJS.minify(fs.readFileSync(from, { encoding: 'utf-8' }));
                        fs.writeFileSync(to, v.code, { encoding: "utf-8" });
                        console.info('[COPY]', from, ">>", to);
                        return false;
                    }
                    if (from.endsWith(".xml") 
                        || from.endsWith(".ts")
                        || from.endsWith(".wxss")
                        || from.endsWith(".wxml")
                        || from.endsWith("tsconfig.json") || from.endsWith("package.json")) {
                        return false;
                    }
                    return true;
                });
            }
        },
        writable: false
    },
});

module.exports = App;
