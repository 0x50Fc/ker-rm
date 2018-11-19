

print("[MAIN]");

app.exec('require.js');
require("wx/wx.console.js");

require("wx/wx.app.js")(JSON.parse(app.getTextContent('app.json')));
