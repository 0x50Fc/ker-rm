#!/bin/bash

tsc -p tsconfig.json

LIST="console.js data.js page.js audio.js export.js"

echo "" > ker.js
for FILE in $LIST
do 
    cat $FILE >> ker.js
done

uglifyjs ker.js -o ker-min.js

rm ../../app/@ker/ker.js
cp ker.js ../../app/@ker/ker.js
