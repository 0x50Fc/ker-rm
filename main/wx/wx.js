


var context;

exports.setContext = function(v) {
    context = v;
};

exports.createCanvasContext = function(canvasId) {
    if(context) {
        return context.getCanvasContext(canvasId);
    }
};
