var fs = require("fs");
var path = require("path");

function Token(type, start, end) {
    this.type = type;
    this.start = start;
    this.end = end;
    this.text = undefined;
    this.source = undefined;
};

Token.TYPE_RAW = 0;
Token.TYPE_AT = 1;
Token.TYPE_SYMBOL = 2;
Token.TYPE_KEY = 3;
Token.TYPE_VALUE = 4;

function Source(p, basePath) {
    this.path = p;
    if (basePath === undefined) {
        this.basePath = path.dirname(p);
    } else {
        this.basePath = basePath;
    }
    console.info("[CSS]", this.path, this.basePath);
    this.source = fs.readFileSync(p, { encoding: 'utf8' });
    this.tokens = [];
};

Source.prototype = Object.create(Object.prototype, {


    compile: {
        writable: false,
        value: function () {

            var tokens = this.tokens;

            this.source.replace(/\@[a-zA-Z]+/g, function (text, index) {
                tokens.push(new Token(Token.TYPE_AT, index, index + text.length))
            });

            this.source.replace(/[\ \,a-zA-Z0-9\-\_\:\.\[\]\=]+\{/g, function (text, index) {

                text.replace(/[a-zA-Z0-9\-\_\:\.\#]+/g, function (name, s) {
                    tokens.push(new Token(Token.TYPE_SYMBOL, index + s, index + s + name.length));
                });

            });

            this.source.replace(/\{[^\{\}]*?\}/g, function (text, index) {

                text.replace(/[^;]+\;/g, function (l, s) {
                    l.replace(/[a-zA-Z0-9\-\ ]+\:/g, function (key, j) {
                        tokens.push(new Token(Token.TYPE_KEY, index + s + j, index + s + j + key.length - 1));
                    });
                    l.replace(/\:[^\;]*/g, function (value, j) {
                        tokens.push(new Token(Token.TYPE_VALUE, index + s + j + 1, index + s + j + value.length));
                    });
                });

            });

            tokens.sort(function (a, b) {
                return a.start - b.start;
            });

        }
    },

    exec: {
        writable: false,
        value: function () {

            for (var i = 0; i < this.tokens.length; i++) {

                var token = this.tokens[i];

                if (token.type == Token.TYPE_AT) {
                    var name = this.source.substr(token.start, token.end - token.start);
                    if (name == "@import") {
                        var basePath = this.basePath;
                        this.source.substr(token.end).replace(/([^\;]+)\;/i, function (text, p) {
                            p = JSON.parse(p);
                            token.path = path.join(basePath, p);
                            token.source = new Source(token.path, basePath);
                            token.source.compile();
                            token.source.exec();
                        });
                        if (token.source === undefined) {
                            token.text = '';
                        }
                        var p = token.end;
                        while (p < this.source.length) {
                            if (this.source.charAt(p) == ';') {
                                break;
                            }
                            p++;
                        }
                        token.end = p + 1;
                    }
                } else if (token.type == Token.TYPE_VALUE) {
                    // console.info("[CSS] [RPX]", this.source.substr(token.start, token.end - token.start), ">");
                    token.text = this.source.substr(token.start, token.end - token.start).replace(/([0-9\.\-]+)rpx/g, function (text, v) {
                        return (v * 0.05) + 'rem';
                    });
                    var dirname = path.dirname(this.path);
                    var basePath = this.basePath;

                    token.text = token.text.replace(/url\((.*?)\)/i, function (text, url) {
                        if(/https?:\/\//.i.test(url)) {
                            return text;
                        }
                        return 'url(' + path.relative(basePath, path.join(dirname, url)) + ')';
                    });

                    // console.info("[CSS] [RPX]", token.text, "<");
                } else if (token.type == Token.TYPE_SYMBOL) {

                    var name = this.source.substr(token.start, token.end - token.start);

                    if (name.startsWith(".") || name.startsWith("#")) {
                    } else if (name == "page") {
                        token.text = "body";
                    } else {
                        token.text = 'wx-' + name;
                    }

                }
            }

        }
    },

    toString: {
        writable: false,
        value: function () {
            var vs = [];
            var idx = 0;

            for (var i = 0; i < this.tokens.length; i++) {
                var token = this.tokens[i];

                if (token.start > idx) {
                    vs.push(this.source.substr(idx, token.start - idx));
                }

                if (token.text !== undefined) {
                    vs.push(token.text);
                } else if (token.source !== undefined) {
                    vs.push(token.source.toString());
                } else {
                    vs.push(this.source.substr(token.start, token.end - token.start));
                }

                idx = token.end;
            }

            if (idx < this.source.length) {
                vs.push(this.source.substr(idx));
            }

            return vs.join('');
        }
    }

});

exports.Source = Source;
exports.Token = Token;
