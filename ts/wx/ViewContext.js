var wx;
(function (wx) {
    var ViewContext = /** @class */ (function () {
        function ViewContext(view) {
            this._view = view;
            this._viewSet = {};
            this._canvasViewSet = {};
            this._canvasSet = {};
        }
        Object.defineProperty(ViewContext.prototype, "view", {
            get: function () {
                return this._view;
            },
            enumerable: true,
            configurable: true
        });
        ViewContext.prototype.create = function (name, id) {
            var v = this._viewSet[id];
            if (v !== undefined) {
                return id;
            }
            var v = app.createView(name, undefined);
            v.id = id;
            v.name = name;
            this._viewSet[id] = v;
            return id;
        };
        ViewContext.prototype.set = function (id, name, value) {
            var v = this._viewSet[id];
            if (v) {
                if (v.name == 'canvas' && name == 'canvas-id') {
                    v.canvasId = value;
                    this._canvasViewSet[v.canvasId] = v;
                }
                v.set(name, value);
            }
        };
        ViewContext.prototype.getCanvasContext = function (canvasId) {
            var v = this._canvasSet[canvasId];
            if (v === undefined) {
                var view = this._canvasViewSet[canvasId];
                if (view !== undefined) {
                    v = view.createCanvas();
                    this._canvasSet[canvasId] = v;
                }
            }
            if (v !== undefined) {
                v = v.getContext('2d');
                if (v !== undefined) {
                    if (v.draw === undefined) {
                        if (typeof screen == 'object' && screen.density !== undefined) {
                            v.scale(screen.density, screen.density);
                        }
                        v.draw = function () { };
                    }
                }
                return v;
            }
        };
        ViewContext.prototype.setFrame = function (id, x, y, width, height) {
            var v = this._viewSet[id];
            if (v) {
                v.setFrame(x, y, width, height);
                v.width = width;
                v.height = height;
            }
        };
        ViewContext.prototype.setContentSizee = function (id, width, height) {
            var v = this._viewSet[id];
            if (v) {
                v.setContentSize(width, height);
            }
        };
        ViewContext.prototype.on = function (id, name, func) {
            var v = this._viewSet[id];
            if (v) {
                v.on(name, func);
            }
        };
        ViewContext.prototype.off = function (id, name, func) {
            var v = this._viewSet[id];
            if (v) {
                v.off(name, func);
            }
        };
        ViewContext.prototype.add = function (id, pid) {
            var v = this._viewSet[id];
            if (v) {
                if (pid === undefined) {
                    this._view.addSubview(v);
                }
                else {
                    var p = this._viewSet[pid];
                    if (p) {
                        p.addSubview(v);
                    }
                }
            }
        };
        ViewContext.prototype.remove = function (id) {
            var v = this._viewSet[id];
            if (v) {
                v.removeView();
                delete this._viewSet[id];
            }
        };
        ViewContext.prototype.recycle = function () {
            for (var id in this._viewSet) {
                var v = this._viewSet[id];
                v.removeView();
            }
            delete this._viewSet;
            delete this._view;
        };
        return ViewContext;
    }());
    wx.ViewContext = ViewContext;
})(wx || (wx = {}));
