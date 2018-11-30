#!/bin/bash

tsc
browserify wx.js > ../compiler/wxlib/wx/wx.web.js
