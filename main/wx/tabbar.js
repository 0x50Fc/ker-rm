

var tabBar = app.options.tabBar;
var items = tabBar.list || [];

var view = app.createView("UIView");

view.set("background-color",tabBar.backgroundColor || '#ffffff');

var topView = app.createView("UIView");

topView.set("background-color",tabBar.borderStyle == 'block' ? 'rgba(30,30,30,30)' : '#dddddd');

view.addSubview(topView);

var buttons = [];

for(var item of items) {

}
