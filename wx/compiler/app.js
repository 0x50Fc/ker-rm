
var path = require('path');
var fs = require('fs');

function App(basePath) {

    this.basePath = basePath;
};

App.prototype = Object.create(Object.prototype, {
    compile: {
        value: function () {
            var vs = [];
            this.compileCSS(vs);
            fs.writeFileSync(path.join(this.basePath,"app.css"), vs.join(''));
        },
        writable: false
    },
    compileCSS: {
        value: function (vs) {

            vs.push(Parse(path.join(this.basePath,"app.wxss")));

            function Parse(p) {
                if (fs.existsSync(p)) {
                    var v = fs.readFileSync(p) + '';
                    var basedir = path.dirname(p);
                    v = v.replace(/([0-9\.\-]+)rpx/g,function(text,v){
                        return (v * 0.05) + 'rem';
                    });
                    v = v.replace(/\@import +\"([^\"]*?)\";/g, function (text, v) {
                        return Parse(path.join(basedir, v));
                    });
                    return v;
                }
                return '';
            }

        },
        writable: false
    },
});

module.exports = App;
