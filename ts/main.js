#!/usr/bin/env node

var path = require('path');
var fs = require('fs');
var Page = require("./page.js");
var App = require("./app.js");
var program = require('commander');
var archiver = require('archiver');

program
    .command('package <inDir> <outDir> <version>')
    .description('quick generate app package')
    .usage("package <inDir> <outDir> <version>")
    .option('-d, --debug', 'Debug')
    .action(function (inDir, outDir, version, cmd) {

        console.info(inDir, ">>", outDir, version);

        if (!fs.existsSync(outDir)) {
            mkdirs(outDir);
        }

        var object;

        if (fs.existsSync(outDir + '/app.json')) {
            object = JSON.parse(fs.readFileSync(outDir + '/app.json', { encoding: 'utf-8' })) || {};
        } else {
            object = {};
        }

        if (typeof object != 'object') {
            object = {};
        }

        if (object.items === undefined) {
            object.items = [version];
        } else {
            var i = 0;
            for (; i < object.items.length; i++) {
                if (object.items[i] == version) {
                    break;
                }
            }
            if (i >= object.items.length) {
                object.items.push(version);
            }
        }

        object['version'] = version;

        var app = new App(inDir, outDir + '/' + version, undefined, cmd.debug);

        app.compile();

        fs.writeFileSync(outDir + '/app.json', JSON.stringify(object, undefined, 2));

        var output = fs.createWriteStream(outDir + '/' + version + '.ker');

        var archive = archiver('zip');
        archive.on('error', function (err) {
            throw err;
        });
        archive.pipe(output);
        archive.directory(outDir + '/' + version, false);
        archive.finalize();

    });

program
    .command('view <dir>')
    .description('quick generate view javascript')
    .usage("view <dir>")
    .action(function (dir) {

        function walk(p, basePath, outPath) {

            if (p.endsWith(".xml")) {

                var page = new Page(p, basePath, outPath, true);

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

        walk(dir, dir, dir);


    });

program
    .command('init')
    .description('quick generate app project')
    .usage("init")
    .action(function () {

        cp(path.join(__dirname, "@ker/ker.js"), "@ker/ker.js");
        cp(path.join(__dirname, "@typings/lib.es5.d.ts"), "@ker/lib.es5.d.ts");
        cp(path.join(__dirname, "@typings/ker.d.ts"), "@ker/ker.d.ts");
        cp(path.join(__dirname, "@typings/global.d.ts"), "@ker/global.d.ts");

        cp(path.join(__dirname, "@wx"), "@wx",function(from,to){
            return (from.endsWith(".ts") || from.endsWith(".xml"))
        });

        if (!fs.existsSync("./main.ts")) {
            fs.writeFileSync("./main.ts", [
                'require("@ker/ker.js");',
                'console.info("Hello World!");'
            ].join('\r\n'), { encoding: 'utf-8' });
        }

        if (!fs.existsSync("./tsconfig.json")) {
            fs.writeFileSync("./tsconfig.json", JSON.stringify({
                "compilerOptions": {
                    "target": "es5",
                    "module": "commonjs",
                    "noLib": false
                },
                "compileOnSave": true,
                "include": [
                    "**/*.ts"
                ]
            }, undefined, 2), { encoding: 'utf-8' });
        }


    });

program.parse(process.argv);


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

function cp(from, to, filter) {

    var st = fs.statSync(from);
    if (st && st.isDirectory()) {
        for (var item of fs.readdirSync(from)) {
            cp(path.join(from, item), path.join(to, item), filter);
        }
    } else if (st && (filter === undefined || (filter(from, to)))) {
        mkdirs(path.dirname(to));
        fs.copyFileSync(from, to);
    }

}

