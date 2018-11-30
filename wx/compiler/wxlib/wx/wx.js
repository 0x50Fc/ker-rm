


var context;

if(typeof wx != 'object') {
    wx = {};
}

wx.setContext = function(v) {
    context = v;
};

wx.createCanvasContext = function(canvasId) {
    if(context) {
        return context.getCanvasContext(canvasId);
    }
};
