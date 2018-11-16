

print("[MAIN]");

app.exec('require.js');

require("wx/wx.console.js");

require("wx/wx.app.js")({});

wx.getLocation({
               success : function(data) {
                console.info(data);
               }
});

app.open("canvas/index.js");
