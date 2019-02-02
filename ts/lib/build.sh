#!/bin/bash

tsc -p tsconfig.json

LIST="console.js promise.js date.js func.js data.js view.js page.js audio.js http.js db.js dialog.js toast.js"

echo "" > ker.js
for FILE in $LIST
do 
    cat $FILE >> ker.js
done

uglifyjs ker.js -o ker-min.js

rm ../../app/@ker/ker.js
cp ker.js ../../app/@ker/ker.js