#!/bin/bash

tsc
browserify wx.js > ../../main/wx/wx.web.js
