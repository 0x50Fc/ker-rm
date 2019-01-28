
var path = require('path');
var fs = require('fs');
var htmlparser = require("htmlparser2");
var babel = require('@babel/parser');

function mkdirs(p) {
    if (fs.existsSync(p)) {
        return;
    }
    mkdirs(path.dirname(p));
    fs.mkdirSync(p, 0o777);
}

function Page(p, basePath, outPath) {

    this.basePath = basePath;
    this.outPath = outPath === undefined ? basePath : outPath;
    this.dirname = path.dirname(p);

    var ext = path.extname(p);
    var base = p.substr(0, p.length - ext.length);

    this.base = base;
    this.outBase = path.join(this.outPath, path.relative(basePath, base));

    this.path = {
        xml: base + '.xml',
        js: this.outBase + '_view.js',
    };

    if (this.basePath != this.outPath) {
        mkdirs(path.dirname(this.base));
    }

};

function compileCode(node, keys, keySet, keyMap, ofnode) {
    var vs = [];
    switch (node.type) {
        case 'ObjectExpression':
            vs.push('{');
            var dot = '';
            for (let p of node.properties) {
                vs.push(dot);
                vs.push(compileCode(p, keys, keySet, keyMap));
                dot = ',';
            }
            vs.push('}');
            break;
        case 'SpreadElement':
            vs.push("...");
            vs.push(compileCode(node.argument, keys, keySet, keyMap));
            break;
        case 'ObjectProperty':
            if (node.key.type == 'Identifier') {
                vs.push(node.key.name);
            } else {
                vs.push(compileCode(node.key, keys, keySet, keyMap))
            }
            vs.push(':');
            vs.push(compileCode(node.value, keys, keySet, keyMap))
            break;
        case 'StringLiteral':
        case 'NumericLiteral':
        case 'BooleanLiteral':
            vs.push(JSON.stringify(node.value));
            break;
        case 'Identifier':
            if (ofnode === undefined && !keyMap[node.name]) {
                keyMap[node.name] = true;
                keySet.push(node.name);
                keys.push([node.name]);
            }
            vs.push(node.name);
            break;
        case 'ConditionalExpression':
            vs.push(compileCode(node.test, keys, keySet, keyMap));
            vs.push('?');
            vs.push(compileCode(node.consequent, keys, keySet, keyMap));
            vs.push(':');
            vs.push(compileCode(node.alternate, keys, keySet, keyMap));
            break;
        case 'UnaryExpression':
            if (node.prefix) {
                vs.push(node.operator);
                vs.push(compileCode(node.argument, keys, keySet, keyMap));
            } else {
                vs.push(compileCode(node.argument, keys, keySet, keyMap));
                vs.push(node.operator);
            }
            break;
        case 'MemberExpression':
            if (ofnode === undefined) {
                var key;
                var p = node.object;
                while (p) {
                    if (p.type == 'MemberExpression') {
                        p = p.object;
                    } else if (p.type == 'Identifier') {
                        key = p.name;
                        break;
                    } else {
                        console.info(JSON.stringify(node, undefined, 2));
                        process.abort();
                        break;
                    }
                }
                if (key !== undefined && !keyMap[key]) {
                    keyMap[key] = true;
                    keySet.push(key);
                    keys.push([key]);
                }
            }
            vs.push(compileCode(node.object, keys, keySet, keyMap, ofnode || node));

            if (node.computed) {
                vs.push('[');
                vs.push(compileCode(node.property, keys, keySet, keyMap));
                vs.push(']');
            } else {
                vs.push('.');
                vs.push(compileCode(node.property, keys, keySet, keyMap, ofnode || node));
            }

            break;
        case 'BinaryExpression':
        case 'LogicalExpression':
            vs.push(compileCode(node.left, keys, keySet, keyMap));
            vs.push(node.operator);
            vs.push(compileCode(node.right, keys, keySet, keyMap));
            break;
        case 'CallExpression':
            vs.push(compileCode(node.callee, keys, keySet, keyMap, node));
            vs.push('(');
            var dot = '';
            for (let p of node.arguments) {
                vs.push(dot);
                vs.push(compileCode(p, keys, keySet, keyMap));
                dot = ',';
            }
            vs.push(')');
            break;
        case 'ArrayExpression':
            vs.push('[');
            var dot = '';
            for (let p of node.elements) {
                vs.push(dot);
                vs.push(compileCode(p, keys, keySet, keyMap));
                dot = ',';
            }
            vs.push(']');
            break;
        default:
            console.error(JSON.stringify(node, undefined, 2));
            process.abort();
            break;
    }
    return vs.join('');
}

function compileAttributeValue(source, vs) {

    if (typeof source == 'string') {

        var idx = 0;
        var code = [];
        var keys = [];
        var keySet = [];
        var keyMap = {};

        source.replace(/{{([^\{\}]*?)}}/g, function (text, v, index) {

            // console.info("[CODE]", v);

            var e;

            try {
                e = babel.parseExpression(v);
            } catch (ex) {
                e = babel.parseExpression('{' + v + '}');
            }

            if (idx < index) {
                code.push(JSON.stringify(source.substr(idx, index - idx)));
            }

            idx = index + text.length;

            v = compileCode(e, keys, keySet, keyMap);

            // console.info("[E]", v);

            code.push('(' + v + ')');

        });

        if (code.length == 0) {
            vs.push(JSON.stringify(source));
            return;
        }

        if (idx < source.length) {
            code.push(JSON.stringify(source.substr(idx)));
        }

        vs.push("E(function(");
        vs.push(keySet.join(','));
        vs.push("){ return ");
        vs.push(code.join('+'))
        vs.push("; },");
        vs.push(JSON.stringify(keySet));
        vs.push(")");


    } else {
        vs.push(JSON.stringify(source));
    }

}

function compileAttributeStringValue(v, vs) {
    vs.push(JSON.stringify(v + ''));
}

function compileAttributes(attrs, vs) {
    vs.push('{');
    var dot = '';
    for (var key in attrs) {
        vs.push(dot);
        compileAttributeStringValue(key, vs);
        vs.push(":");
        compileAttributeValue(attrs[key], vs);
        dot = ',';
    }
    vs.push('}');
}

Page.prototype = Object.create(Object.prototype, {
    compile: {
        value: function () {
            mkdirs(path.dirname(this.path.js));
            var vs = [];
            this.compilePageView(vs);
            fs.writeFileSync(this.path.js, vs.join(''));
        },
        writable: false
    },
    compilePageView: {
        value: function (vs) {
            var wxmlPath = this.path.xml;
            var dir = path.dirname(wxmlPath);
            var object = this.object || {};

            var element = {
                name: 'document',
                attrs: {},
                children: [],
                text: ''
            };

            var onopentag, ontext, onclosetag, onerror;
            var basePaths = [dir];
            var relativePath = dir;

            var done = function () {
                var v = element.text.trim();
                if (v) {
                    element.children.push({
                        name: 'text',
                        attrs: {
                            '#text': v
                        },
                        parent: element,
                        basePath: basePaths[basePaths.length - 1]
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
                            parent: element,
                            basePath: basePaths[basePaths.length - 1]
                        });
                    }
                }
                element.text = '';
            };

            onopentag = function (tagname, attrs) {

                done();

                var e = {
                    name: tagname,
                    attrs: attrs,
                    text: '',
                    children: [],
                    parent: element,
                    basePath: basePaths[basePaths.length - 1]
                };

                element.children.push(e);

                element = e;

            };

            ontext = function (text) {
                element.text += text;
            };

            onclosetag = function (tagname) {
                while (element && tagname != element.name) {
                    element = element.parent;
                }
                end();
                if (element.name == "include") {
                    var src = element.attrs['src'];
                    element = element.parent;
                    element.children.pop();
                    if (src) {
                        var basePath = basePaths[basePaths.length - 1];

                        var par = new htmlparser.Parser({
                            onopentag: onopentag,
                            ontext: ontext,
                            onclosetag: onclosetag,
                            onerror: onerror
                        }, { decodeEntities: true, recognizeSelfClosing: true, lowerCaseAttributeNames: false, lowerCaseTags: false });

                        var p = path.join(basePath, src);
                        var v = fs.readFileSync(p, { encoding: 'utf8' });
                        basePaths.push(path.dirname(p));
                        par.write(v);
                        par.end();
                        basePaths.pop();
                    }
                } else {
                    element = element.parent;
                }
            };

            onerror = function (errmsg) {
                console.error(errmsg);
                process.abort();
            };

            var parse = new htmlparser.Parser({
                onopentag: onopentag,
                ontext: ontext,
                onclosetag: onclosetag,
                onerror: onerror
            }, { decodeEntities: true, recognizeSelfClosing: true, lowerCaseAttributeNames: false, lowerCaseTags: false });

            parse.write(fs.readFileSync(wxmlPath, { encoding: 'utf8' }));
            parse.end();
            end();

            var level = 0;
            for (var element of element.children) {
                View(element, path.relative(relativePath, basePaths[basePaths.length - 1]));
            }

            function View(element, basePath) {
                vs.push("\t".repeat(level));
                vs.push("V(element,data,");
                vs.push(JSON.stringify(element.name));
                vs.push(",");

                compileAttributes(element.attrs, vs)

                vs.push(",function(element,data){\n");

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
    }
});

module.exports = Page;
