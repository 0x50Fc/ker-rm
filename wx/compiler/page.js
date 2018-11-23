
var path = require('path');
var fs = require('fs');
var htmlparser = require("htmlparser2");
var css = require("./css");

function Page(p, basePath) {

    this.basePath = basePath;
    this.dirname = path.dirname(p);

    var ext = path.extname(p);
    var base = p.substr(0, p.length - ext.length);

    this.base = base;
    this.path = {
        html: base + '.wx.html',
        wxml: base + '.wxml',
        wxss: base + '.wxss',
        json: base + '.json',
        page: base + '.page.js',
    };

};

Page.prototype = Object.create(Object.prototype, {
    compile: {
        value: function () {

            fs.writeFileSync(this.path.page, 'require("wx/wx.page.js")({path: path,query: query}, ' + JSON.stringify(path.relative(this.basePath, this.base)) + ', page);');
            var vs = [];
            vs.push('<style type="text/css">\n');
            this.compilePageCSS(vs);
            vs.push('</style></head><body><script type="text/javascript">\n');
            vs.push("(function(){\n");
            vs.push("var View = kk.CreateElement;\n")
            vs.push("kk.Page(");

            this.compilePageView(vs);

            vs.push(");})();\n");
            vs.push('</script></body>');

            fs.writeFileSync(this.path.html, vs.join(''));
        },
        writable: false
    },
    compilePageView: {
        value: function (vs) {

            var element = {
                name: 'document',
                attrs: {},
                children: [],
                text: ''
            };

            var done = function () {
                var v = element.text.trim();
                if (v) {
                    console.info("[DONE]", v);
                    element.children.push({
                        name: 'text',
                        attrs: {
                            '#text': v
                        },
                        parent: element
                    });
                }
                element.text = '';
            };

            var end = function () {
                var v = element.text.trim();
                if (v) {
                    if (element.children.length == 0) {
                        element.attrs['#text'] = v;
                    } else {
                        element.children.push({
                            name: 'text',
                            attrs: {
                                '#text': v
                            },
                            parent: element
                        });
                    }
                }
                element.text = '';
            };

            var parse = new htmlparser.Parser({

                onopentag: function (tagname, attrs) {

                    done();

                    var e = {
                        name: tagname,
                        attrs: attrs,
                        text: '',
                        children: [],
                        parent: element
                    };

                    element.children.push(e);

                    element = e;

                },

                ontext: function (text) {
                    element.text += text;
                },

                onclosetag: function (tagname) {
                    while (element && tagname != element.name) {
                        element = element.parent;
                    }
                    end();
                    element = element.parent;
                },

                onerror: function (errmsg) {
                    console.error(errmsg);
                    process.abort();
                }

            }, { decodeEntities: true, recognizeSelfClosing: true });

            parse.write(fs.readFileSync(this.path.wxml, { encoding: 'utf8' }));
            parse.end();
            end();

            vs.push("function(element,data,context){\n");

            var level = 0;

            level++;

            for (var element of element.children) {
                View(element);
            }

            level--;

            vs.push("}");

            function View(element) {

                vs.push("\t".repeat(level));
                vs.push("View(element,data,");
                vs.push(JSON.stringify(element.name));
                vs.push(",");

                vs.push(JSON.stringify(element.attrs));

                vs.push(",context,function(element,data,context){\n");

                level++;

                if (element.children) {
                    for (var e of element.children) {
                        View(e);
                    }
                }

                level--;

                vs.push("\t".repeat(level));
                vs.push("});\n");
            }

        },
        writable: false
    },
    compilePageCSS: {
        value: function (vs) {

            vs.push(Parse(this.path.wxss));

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

module.exports = Page;
