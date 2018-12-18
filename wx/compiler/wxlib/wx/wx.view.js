

var viewClass = {
    input : 'UITextField',
    textarea : 'UITextView',
    canvas : 'UIView'
};


function Context(view) {
    this._view = view;
    this._views = {};
    this._canvasViews = {};
    this._canvas = {};
};

Context.prototype = Object.create(Object.prototype, {

    create: {
        value: function (name, id) {
            var v = this._views[id];
            if (v !== undefined) {
                return id;
            }
            var isa = viewClass[name];
            if (isa === undefined) {
                v = app.createView(name);
            } else if (typeof isa == 'function') {
                v = isa(name);
            } else {
                v = app.createView(isa);
            }
            v.tagName = name;
            v.id = id;
            this._views[id] = v;
            return id;
        },
        writable: false,
        configurable: false,
        enumerable: true
    },

    set: {
        value: function (id, name, value) {
            var v = this._views[id];
            if (v) {
                if (v.tagName == 'canvas' && name == 'canvas-id') {
                    v.canvasId = value;
                    this._canvasViews[v.canvasId] = v;
                }
                v.set(name, value);
            }
        },
        writable: false,
        configurable: false,
        enumerable: true
    },

    getCanvasContext: {
        value: function (canvasId) {
            var v = this._canvas[canvasId];
            if (v === undefined) {
                v = this._canvasViews[canvasId];
                if (v !== undefined) {
                    v = v.createCanvas();
                    this._canvas[canvasId] = v;
                }
            }
            if (v !== undefined) {
                v = v.getContext('2d');
                if (v !== undefined) {
                    v.draw = function () { };
                }
                return v;
            }
        },
        writable: false,
        configurable: false,
        enumerable: true
    },

    setFrame: {
        value: function (id, x, y, width, height) {
            var v = this._views[id];
            if (v) {
                v.setFrame(x, y, width, height);
            }
        },
        writable: false,
        configurable: false,
        enumerable: true
    },

    setContentSizee: {
        value: function (id, width, height) {
            var v = this._views[id];
            if (v) {
                v.setContentSize(width, height);
            }
        },
        writable: false,
        configurable: false,
        enumerable: true
    },

    on: {
        value: function (id, name, func) {
            var v = this._views[id];
            if (v) {
                v.on(name, func);
            }
        },
        writable: false,
        configurable: false,
        enumerable: true
    },

    off: {
        value: function (id, name, func) {
            var v = this._views[id];
            if (v) {
                v.off(name, func);
            }
        },
        writable: false,
        configurable: false,
        enumerable: true
    },

    add: {
        value: function (id, pid) {
            var v = this._views[id];
            if (v) {
                if (pid === undefined) {
                    this._view.addSubview(v);
                } else {
                    var p = this._views[pid];
                    if (p) {
                        p.addSubview(v);
                    }
                }
            }
        },
        writable: false,
        configurable: false,
        enumerable: true
    },

    remove: {
        value: function (id) {
            var v = this._views[id];
            if (v) {
                v.remove();
                delete this._views[id];
            }
        },
        writable: false,
        configurable: false,
        enumerable: true
    },

    recycle: {
        value: function () {
            for (var id in this._views) {
                var v = this._views[id];
                v.remove();
            }
            delete this._views;
            delete this._view;
        },
        writable: false,
        configurable: false,
        enumerable: true
    },

});

exports.Context = Context;

exports.addClass = function (name, isa) {
    viewClass[name] = isa;
};
