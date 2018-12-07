
page.on("app.toast.hide", function (e) {
    page.close();
});

var view = page.view;

var title = new UIAttributedText();
title.appendText(query.title || '', '14px', '#fff');

var titleSize = app.getAttributedTextContentSize(title, 200);

if (titleSize.width < 80) {
    titleSize.width = 80;
} else {
    titleSize.width = Math.ceil(titleSize.width);
}

if (titleSize.height < 20) {
    titleSize.height = 20;
} else {
    titleSize.height = Math.ceil(titleSize.height);
}

var bgView = app.createView("UIView");
var bgSize = { width: titleSize.width + 20, height: titleSize.height + 20 };

bgView.set('background-color', 'rgba(0,0,0,0.65)');
bgView.set('border-radius', '5');
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

setTimeout(function () {
    page.close();
}, query.duration || 1500);
