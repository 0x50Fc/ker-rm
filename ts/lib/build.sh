#!/bin/bash

tsc -p tsconfig.json

LIST="console.js data.js view.js page.js audio.js http.js db.js dialog.js toast.js export.js"

echo "" > ker.js
for FILE in $LIST
do 
    cat $FILE >> ker.js
done

uglifyjs ker.js -o ker-min.js

rm ../../app/@ker/ker.js
cp ker.js ../../app/@ker/ker.js
