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
    .action(function (inDir, outDir, version) {

        console.info(inDir, ">>", outDir, version);

        if (!fs.existsSync(outDir)) {
            fs.mkdirSync(outDir);
        }

        var object;

        if (fs.existsSync(outDir + '/app.json')) {
            object = JSON.parse(fs.readFileSync(outDir + '/app.json', { encoding: 'utf-8' })) || {};
        } else {
            object = {};
        }

        object['version'] = version;

        var app = new App(inDir, outDir + '/' + version);

        app.compile();

        walk(inDir, inDir, outDir + '/' + version);

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

program.parse(process.argv);

function walk(p, basePath, outPath) {

    if (p.endsWith(".xml")) {

        var page = new Page(p, basePath, outPath);

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
