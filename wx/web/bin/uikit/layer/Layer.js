"use strict";
var kk;
(function (kk) {
    let ui;
    (function (ui) {
        class ReuseLayer {
            constructor() {
                this._layers = {};
            }
            popLayer(reuse) {
                var vs = this._layers[reuse];
                if (vs !== undefined) {
                    return vs.shift();
                }
            }
            addLayer(reuse, layer) {
                var vs = this._layers[reuse];
                if (vs === undefined) {
                    this._layers[reuse] = [layer];
                }
                else {
                    vs.push(layer);
                }
            }
            static get(element, autocreate = true) {
                var v = element.object("ReuseLayer");
                if (autocreate && (v === undefined || !(v instanceof ReuseLayer))) {
                    v = new ReuseLayer();
                    element.setObject("ReuseLayer", v);
                }
                return v;
            }
        }
        class LayerElement extends LayoutElement {
            get layerClass() {
                return Layer.className;
            }
            get reuse() {
                var v = this.get("reuse");
                if (v === undefined && this.levelId != 0) {
                    v = this.layerClass + "_" + this.levelId;
                }
                return v;
            }
            get layer() {
                return this._layer;
            }
            invalidate() {
                let event = new ElementEvent(this);
                this.emit("invalidate", event);
            }
            changedKey(key) {
                super.changedKey(key);
                if (this._layer !== undefined) {
                    this.layerChangedKey(key, this._layer);
                }
            }
            layerChangedKey(key, layer) {
                switch (key) {
                    case "background-color":
                        layer.backgroundColor = parseColor(this.get(key));
                        break;
                    case "border-color":
                        layer.borderColor = parseColor(this.get(key));
                        break;
                    case "border-width":
                        layer.borderWidth = parsePixel(this.get(key));
                        break;
                    case "border-radius":
                        layer.borderRadius = parsePixel(this.get(key));
                        break;
                    case "opacity":
                        layer.opacity = parseFloat(this.get(key));
                        break;
                }
            }
            didLayouted() {
                super.didLayouted();
                if (this._layer !== undefined) {
                    this.layerDidLayouted(this._layer);
                }
            }
            layerDidLayouted(layer) {
                layer.frame = this.frame;
            }
            isVisibleChildren(element) {
                var left = element.frame.origin.x - this.contentOffset.x;
                var top = element.frame.origin.y - this.contentOffset.y;
                var right = left + element.frame.size.width;
                var bottom = left + element.frame.size.height;
                left = Math.max(left, 0);
                top = Math.max(top, 0);
                right = Math.min(right, this.frame.size.width);
                bottom = Math.min(bottom, this.frame.size.height);
                return right > left && bottom > top;
            }
            createLayer() {
                return new Layer();
            }
            obtainLayer() {
                if (this._layer !== undefined) {
                    return;
                }
                var v;
                let reuse = this.reuse;
                if (reuse !== undefined && this.parent !== undefined) {
                    let reuseLayer = ReuseLayer.get(this.parent, true);
                    v = reuseLayer.popLayer(reuse);
                }
                if (v === undefined) {
                    v = this.createLayer();
                }
                this._layer = v;
                this.layerDidLayouted(v);
                for (let key of this.keys()) {
                    this.layerChangedKey(key, v);
                }
            }
            recycleLayer() {
                if (this._layer === undefined) {
                    return;
                }
                let reuse = this.reuse;
                if (reuse !== undefined && this.parent !== undefined) {
                    let reuseLayer = ReuseLayer.get(this.parent, true);
                    reuseLayer.addLayer(reuse, this._layer);
                }
                this._layer = undefined;
            }
            display(context) {
                if (this._layer === undefined) {
                    this.obtainLayer();
                }
                this._layer.display(context);
                var e = this.firstChild;
                while (e) {
                    if (e instanceof LayerElement) {
                        if (this.isVisibleChildren(e)) {
                            e.display(context);
                        }
                        else {
                            e.recycleLayer();
                        }
                    }
                    e = e.nextSibling;
                }
            }
        }
        ui.LayerElement = LayerElement;
        class Layer {
            constructor() {
                this._backgroundColor = { r: 0, g: 0, b: 0, a: 0 };
                this._contentsRect = { origin: { x: 0, y: 0 }, size: { width: 1, height: 1 } };
                this._contentsCenter = { origin: { x: 0, y: 0 }, size: { width: 1, height: 1 } };
                this._opacity = 1.0;
                this._hidden = false;
                this._borderWidth = { value: 0, type: PixelType.Auto };
                this._borderRadius = { value: 0, type: PixelType.Auto };
                this._borderColor = { r: 0, g: 0, b: 0, a: 0 };
                this._shadowColor = { r: 0, g: 0, b: 0, a: 0 };
                this._shadowOffset = { x: 0, y: 0 };
                this._shadowBlur = { value: 0, type: PixelType.Auto };
                this._invalidate = true;
                this._allowDraw = false;
                this._frame = { origin: { x: 0, y: 0 }, size: { width: 0, height: 0 } };
            }
            get backgroundColor() {
                return this._backgroundColor;
            }
            set backgroundColor(v) {
                this._backgroundColor = v;
            }
            get contents() {
                return this._contents;
            }
            set contents(v) {
                this._contents = v;
                if (v !== undefined) {
                    this._allowDraw = false;
                }
            }
            get contentsRect() {
                return this._contentsRect;
            }
            set contentsRect(value) {
                this._contentsRect = value;
            }
            get contentsCenter() {
                return this._contentsCenter;
            }
            set contentsCenter(value) {
                this._contentsCenter = value;
            }
            get opacity() {
                return this._opacity;
            }
            set opacity(value) {
                this._opacity = value;
            }
            get hidden() {
                return this._hidden;
            }
            set hidden(value) {
                this._hidden = value;
            }
            get borderWidth() {
                return this._borderWidth;
            }
            set borderWidth(value) {
                this._borderWidth = value;
            }
            get borderRadius() {
                return this._borderRadius;
            }
            set borderRadius(value) {
                this._borderRadius = value;
            }
            get borderColor() {
                return this._borderColor;
            }
            set borderColor(value) {
                this._borderColor = value;
            }
            get shadowColor() {
                return this._shadowColor;
            }
            set shadowColor(value) {
                this._shadowColor = value;
            }
            get shadowOffset() {
                return this._shadowOffset;
            }
            set shadowOffset(value) {
                this._shadowOffset = value;
            }
            get shadowBlur() {
                return this._shadowBlur;
            }
            set shadowBlur(value) {
                this._shadowBlur = value;
            }
            get allowDraw() {
                return this._allowDraw;
            }
            set allowDraw(value) {
                this._allowDraw = value;
            }
            get frame() {
                return this._frame;
            }
            set frame(value) {
                this._frame = value;
            }
            get className() {
                return Layer.className;
            }
            draw(context, canvas) {
            }
            display(context) {
                if (this._invalidate
                    && this._allowDraw
                    && this._frame.size.width > 0 && this._frame.size.height > 0) {
                    let canvas = context.createCanvas(this._frame.size.width, this._frame.size.height);
                    this.draw(context, canvas);
                    this._contents = canvas.toImage();
                    this._invalidate = false;
                }
                context.drawLayer(this);
            }
            invalidate() {
                this._invalidate = true;
            }
        }
        Layer.className = "Layer";
        ui.Layer = Layer;
    })(ui = kk.ui || (kk.ui = {}));
})(kk || (kk = {}));
//# sourceMappingURL=Layer.js.map