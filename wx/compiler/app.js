
var path = require('path');
var fs = require('fs');
var css = require('./css');
var babel = require('babel-core');

function App(basePath, outPath) {
    this.basePath = basePath;
    this.outPath = outPath === undefined ? basePath : outPath;
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
            console.info(from, ">>", to,typeof fs,typeof fs.copyFileSync);
            fs.copyFileSync(from, to);
        }
    }

}

App.prototype = Object.create(Object.prototype, {
    compile: {
        value: function () {

            copy("wxlib", this.outPath);
            if (this.basePath != this.outPath) {
                copy(this.basePath, this.outPath, function (from, to) {
                    if (from.endsWith(".js")) {
                        let v = babel.transformFileSync(from, {
                            presets: ['babel-preset-es2015'].map(require.resolve),
                            plugins: ["babel-plugin-transform-es2015-destructuring", "babel-plugin-transform-object-rest-spread"].map(require.resolve)
                        }).code;
                        fs.writeFileSync(to, v);
                        return false;
                    }
                    if (from.endsWith(".wxml") || from.endsWith(".wxss")) {
                        return false;
                    }
                    return true;
                });
            }

            var vs = [];
            var s = new css.Source("app.css");
            s.compile();
            for (var i = 0; i < s.tokens.length; i++) {
                var token = s.tokens[i];
                if (token.type == css.Token.TYPE_VALUE) {
                    token.text = s.source.substr(token.start, token.end - token.start).replace(/([0-9\.\-]+)rpx/g, function (text, v) {
                        return (v * 0.05) + 'rem';
                    });
                }
            }
            vs.push(s.toString());
            this.compileCSS(vs);
            fs.writeFileSync(path.join(this.outPath, "app.css"), vs.join(''));
        },
        writable: false
    },
    compileCSS: {
        value: function (vs) {

            vs.push(Parse(path.join(this.basePath, "app.wxss")));

            function Parse(p) {
                if (fs.existsSync(p)) {
                    var s = new css.Source(p);
                    s.compile();
                    s.exec();
                    return s.toString();
                }
                return '';
            }

        },
        writable: false
    },
});

module.exports = App;
