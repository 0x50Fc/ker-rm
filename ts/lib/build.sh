#!/bin/bash

tsc -p tsconfig.json

LIST="require.js console.js data.js page.js"

if [ "$1" = "release" ]; then
uglifyjs $LIST -o ker.js
else
    echo "" > ker.js
    for FILE in $LIST
    do 
        cat $FILE >> ker.js
    done
fi

rm ../../app/lib/ker.js
cp ker.js ../../app/lib/ker.js
