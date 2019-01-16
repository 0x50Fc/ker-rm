
var Doc = require("lib/document.js");

document = Doc.create(page);

(function(element){

    var text = document.createElement("text");

    text.set("#text","hello world");
    text.set("font","28rpx");
    text.set("color","#000");

    element.append(text);

    element.set("background-color","#fff");

    document.layout();

})(document.rootElement);
