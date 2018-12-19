
page.on("app.toast.hide", function (e) {
    page.close();
});

var view = page.view;

var title = new UIAttributedText();
var fontSize = 14;
var maxWidth = 200;
var minWidth = 80;
var minHeight = 20;
var padding = 20;
var borderRadius = 5;

if(typeof screen == 'object' && screen.density !== undefined) {
    fontSize = fontSize * screen.density;
    maxWidth = maxWidth * screen.density;
    minWidth = minWidth * screen.density;
    minHeight = minHeight * screen.density;
    padding = padding * screen.density;
    borderRadius = borderRadius * screen.density;
}

title.appendText(query.title || '', fontSize + 'px', '#fff');

var titleSize = app.getAttributedTextContentSize(title, maxWidth);

if (titleSize.width < minWidth) {
    titleSize.width = minWidth;
} else {
    titleSize.width = Math.ceil(titleSize.width);
}

if (titleSize.height < minHeight) {
    titleSize.height = minHeight;
} else {
    titleSize.height = Math.ceil(titleSize.height);
}

var bgView = app.createView("UIView");
var bgSize = { width: titleSize.width + padding, height: titleSize.height + padding };

bgView.set('background-color', 'rgba(0,0,0,0.65)');
bgView.set('border-radius', borderRadius + '');
bgView.set('overflow', 'hidden');

view.addSubview(bgView);

var titleView = app.createView("UILabel");

titleView.set('text-align', 'center');
titleView.setFrame((bgSize.width - titleSize.width) * 0.5, (bgSize.height - titleSize.height) * 0.5, titleSize.width, titleSize.height)

titleView.setAttributedText(title);

bgView.addSubview(titleView);

var resize = function () {
    bgView.setFrame((page.width - bgSize.width) * 0.5, (page.height - bgSize.height) * 0.5, bgSize.width, bgSize.height);
};

page.on('resize', resize);

resize();

page.on('backPressed',function(e){
    e.returnValue = false;
});

setTimeout(function () {
    page.close();
}, query.duration || 1500);
