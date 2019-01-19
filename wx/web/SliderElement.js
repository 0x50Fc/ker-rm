"use strict";
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    }
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var ViewElement_1 = require("./ViewElement");
var V_1 = require("./V");
var once_1 = require("./once");
var SliderElement = /** @class */ (function (_super) {
    __extends(SliderElement, _super);
    function SliderElement(document, name, id) {
        var _this = _super.call(this, document, name, id) || this;
        _this._updatePercentViewing = false;
        _this._touchPageX = 0;
        _this._touchCurX = 0;
        _this._changing = false;
        _this._min = 0;
        _this._max = 100;
        _this._step = 1;
        _this._value = 0;
        return _this;
    }
    SliderElement.prototype.createView = function () {
        var v = _super.prototype.createView.call(this);
        v.progressView = document.createElement('div');
        v.progressView.className = 'bar';
        v.appendChild(v.progressView);
        v.progressBar = document.createElement('div');
        v.progressView.appendChild(v.progressBar);
        v.infoView = document.createElement('div');
        v.infoView.className = 'info';
        v.appendChild(v.infoView);
        v.curView = document.createElement('div');
        v.curView.className = 'cur';
        v.progressView.appendChild(v.curView);
        return v;
    };
    SliderElement.prototype.updatePercentView = function () {
        var v = (this._value - this._min) * 100 / (this._max - this._min);
        this._view.progressBar.style.width = v + '%';
        this._view.infoView.innerText = this._value + '';
        this._view.curView.style.left = v + '%';
        this._updatePercentViewing = false;
    };
    SliderElement.prototype.setUpdatePercentView = function () {
        if (this._updatePercentViewing) {
            return;
        }
        this._updatePercentViewing = true;
        var v = this;
        once_1.once(function () {
            v.updatePercentView();
        });
    };
    SliderElement.prototype.set = function (key, value) {
        _super.prototype.set.call(this, key, value);
        if (key == 'value') {
            this._value = parseFloat(value || '0');
            this.setUpdatePercentView();
        }
        else if (key == 'min') {
            this._min = parseFloat(value || '0');
            this.setUpdatePercentView();
        }
        else if (key == 'max') {
            this._max = parseFloat(value || '0');
            this.setUpdatePercentView();
        }
        else if (key == 'step') {
            this._step = parseFloat(value || '0');
            this.setUpdatePercentView();
        }
        else if (key == 'show-value') {
            if (V_1.booleanValue(value)) {
                this._view.infoView.style.display = 'block';
                this.setUpdatePercentView();
            }
            else {
                this._view.infoView.style.display = 'none';
            }
        }
        else if (key == 'block-color') {
            this._view.curView.style.backgroundColor = value;
        }
        else if (key == 'block-size') {
            var v = parseFloat(value || '28');
            this._view.curView.style.width = v + 'px';
            this._view.curView.style.height = v + 'px';
            this._view.curView.style.borderRadius = (v * 0.5) + 'px';
            this._view.curView.style.marginLeft = -(v * 0.5) + 'px';
            this._view.curView.style.marginTop = -(v * 0.5) + 'px';
        }
        else if (key == 'selected-color' || key == 'activeColor') {
            this._view.progressBar.style.backgroundColor = value;
        }
        else if (key == 'color' || key == 'backgroundColor') {
            this._view.progressView.style.backgroundColor = value;
        }
        else if (key == 'backgroundColor') {
            this._view.style.backgroundColor = value;
        }
    };
    SliderElement.prototype.doEvent = function (event, name, detail) {
        _super.prototype.doEvent.call(this, event, name, detail);
        if (name == "touchstart") {
            var width = this._view.progressView.clientWidth;
            var x = event.touches[0].pageX;
            var curX_1 = (this._value - this._min) * width / (this._max - this._min);
            if (Math.abs(x - curX_1) <= this._view.progressView.clientWidth * 0.5) {
                this._changing = true;
                this._touchPageX = x;
                this._touchCurX = curX_1;
            }
        }
        else if (name == "touchmove" && this._changing) {
            var width = this._view.progressView.clientWidth;
            var x = event.touches[0].pageX;
            var curX = this._touchCurX + (x - this._touchPageX);
            if (curX < 0) {
                curX = 0;
            }
            if (curX > width) {
                curX = width;
            }
            var v = (curX * (this._max - this._min)) / width;
            v = Math.floor(v / this._step) * this._step + this._min;
            if (this._value != v) {
                this._value = v;
                this.updatePercentView();
                this.doElementEvent("changing", { value: v });
            }
        }
        else if ((name == "touchend" || name == "touchcancel") && this._changing) {
            this.doElementEvent("change", { value: this._value });
            this._changing = false;
        }
    };
    SliderElement.prototype.getValue = function () {
        return this.get("value") || 0;
    };
    SliderElement.prototype.setValue = function (value) {
        this.set("value", value || 0);
    };
    return SliderElement;
}(ViewElement_1.ViewElement));
exports.SliderElement = SliderElement;
