
var path = require('path');
var fs = require('fs');

function App(basePath, outPath, shPath) {
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

    if (bname.startsWith(".") || bname == "node_modules") {
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
            if (this.basePath != this.outPath) {
                copy(this.basePath, this.outPath, function (from, to) {
                    if (from.endsWith(".xml") || from.endsWith(".ts")
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
