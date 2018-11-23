
var path = require('path');
var fs = require('fs');
var css = require('./css');

function App(basePath) {

    this.basePath = basePath;
};

App.prototype = Object.create(Object.prototype, {
    compile: {
        value: function () {
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
            fs.writeFileSync(path.join(this.basePath,"app.css"), vs.join(''));
        },
        writable: false
    },
    compileCSS: {
        value: function (vs) {

            vs.push(Parse(path.join(this.basePath,"app.wxss")));

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
