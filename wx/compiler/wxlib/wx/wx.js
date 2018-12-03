


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

wx.showToast = function(object) {

};

wx.showModal = function(object) {

};

wx.showLoading = function(object) {

};

wx.showActionSheet = function(object) {

};

wx.hideToast = function(object) {

};

wx.hideLoading = function(object) {

};

