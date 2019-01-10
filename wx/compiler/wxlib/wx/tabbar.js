
var Page = require("wx/wx.page.js");
var changedSelectedIndex, resize;
var options = __appOptions;
var tabBar = options.tabBar;
var items = tabBar.list || [];

page.setOptions({
    title: options.window.navigationBarTitleText
});

page.on("ready", function () {

    page.view.set("background-color", options.window.backgroundColor || "#fff");

    var view = app.createView("UIView");

    view.set("background-color", tabBar.backgroundColor || '#ffffff');

    page.view.addSubview(view);

    var topView = app.createView("UIView");

    topView.set("background-color", tabBar.borderStyle == 'block' ? 'rgba(30,30,30,30)' : '#dddddd');

    view.addSubview(topView);

    var buttons = [];
    var selectedIndex = 0;
    var pages = [];
    var fontSize = 12;
    var barHeight = 49;
    var iconSize = 28;
    var paddingTop = 64;
    var barBottom = 0;
    var textHeight = 20;

    if (/iPhone/i.test(userAgent)) {
        if (page.height >= 812) {
            barHeight += 34;
            barBottom = 34;
        }
    }

    if (platform == 'Android') {
        paddingTop = 44;
    }

    if (typeof screen == 'object' && screen.density !== undefined) {
        barHeight = barHeight * screen.density;
        iconSize = iconSize * screen.density;
        paddingTop = paddingTop * screen.density;
        barBottom = barBottom * screen.density;
        fontSize = fontSize * screen.density;
        textHeight = textHeight * screen.density;
    }

    for (var i = 0; i < items.length; i++) {
        var item = items[i];
        var button = app.createView("KerButton");
        view.addSubview(button);

        button.textView = app.createView("UILabel");
        button.addSubview(button.textView);
        button.textView.set("text-align", 'center');
        button.textView.set("font", fontSize + 'px');
        button.textView.set("color", tabBar.color);
        button.textView.set("#text", item.text || '');
        button.textView.set("enabled", "false");

        button.imageView = app.createView("UIView");
        button.addSubview(button.imageView);
        button.imageView.setImage(app.createImage(item.iconPath));
        button.imageView.set("enabled", "false");

        button.selectImageView = app.createView("UIView");
        button.addSubview(button.selectImageView);
        button.selectImageView.setImage(app.createImage(item.selectedIconPath));
        button.selectImageView.set("enabled", "false");

        (function (i, button) {

            button.on("tap", function () {
                if (selectedIndex != i) {
                    selectedIndex = i;
                    changedSelectedIndex();
                }
            });

        })(i, button);

        buttons.push(button);
        pages.push({ path: item.pagePath });
    }


    changedSelectedIndex = function () {
        for (var i = 0; i < buttons.length; i++) {
            var button = buttons[i];
            var p = pages[i];
            if (selectedIndex == i) {
                button.imageView.set("hidden", "true");
                button.selectImageView.set("hidden", "false");
                button.textView.set("color", tabBar.selectedColor);

                if (p.webview === undefined) {
                    p.webview = Page({}, p.path, page, app, { top: paddingTop, left: 0, bottom: barHeight, right: 0 });
                } else {
                    p.webview.set("hidden", "false");
                }
            } else {
                button.imageView.set("hidden", "false");
                button.selectImageView.set("hidden", "true");
                button.textView.set("color", tabBar.color);
                if (p.webview !== undefined) {
                    p.webview.set("hidden", "true");
                }
            }
        }
    };

    resize = function () {

        view.setFrame(0, page.height - barHeight, page.width, barHeight);
        topView.setFrame(0, 0, page.width, 1);

        if (buttons.length > 0) {
            var w = page.width / buttons.length;
            for (var i = 0; i < buttons.length; i++) {
                var button = buttons[i];
                var p = pages[i];
                button.setFrame(i * w, 0, w, barHeight);
                button.imageView.setFrame((w - iconSize) * 0.5, 4, iconSize, iconSize);
                button.selectImageView.setFrame((w - iconSize) * 0.5, 4, iconSize, iconSize);
                button.textView.setFrame(0, barHeight - textHeight - barBottom, w, textHeight);
                if (p.webview !== undefined) {
                    p.webview.setFrame(0, paddingTop, page.width, page.height - paddingTop - barHeight);
                }
            }
        }

    };

    page.on("resize", resize);

    resize();
    changedSelectedIndex();

});
