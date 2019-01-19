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
var ProgressElement = /** @class */ (function (_super) {
    __extends(ProgressElement, _super);
    function ProgressElement(document, name, id) {
        var _this = _super.call(this, document, name, id) || this;
        _this._percent = 0;
        return _this;
    }
    ProgressElement.prototype.createView = function () {
        var v = _super.prototype.createView.call(this);
        v.progressView = document.createElement('div');
        v.progressView.className = 'bar';
        v.appendChild(v.progressView);
        v.progressBar = document.createElement('div');
        v.progressView.appendChild(v.progressBar);
        v.infoView = document.createElement('div');
        v.infoView.className = 'info';
        v.appendChild(v.infoView);
        return v;
    };
    ProgressElement.prototype.updatePercentView = function () {
        this._view.progressBar.style.width = this._percent + '%';
        this._view.infoView.innerText = this._percent + '%';
    };
    ProgressElement.prototype.set = function (key, value) {
        _super.prototype.set.call(this, key, value);
        if (key == 'percent') {
            this._percent = parseFloat(value || '0');
            this.updatePercentView();
        }
        else if (key == 'show-info') {
            if (V_1.booleanValue(value)) {
                this._view.infoView.style.display = 'block';
            }
            else {
                this._view.infoView.style.display = 'none';
            }
        }
        else if (key == 'border-radius') {
            this._view.style.borderRadius = V_1.pixelStringValue(value);
        }
        else if (key == 'font-size') {
            this._view.infoView.style.fontSize = V_1.pixelStringValue(value);
        }
        else if (key == 'stroke-width') {
            this._view.progressBar.style.height = V_1.pixelStringValue(value);
        }
        else if (key == 'color' || key == 'activeColor') {
            this._view.progressBar.style.backgroundColor = value;
        }
        else if (key == 'backgroundColor') {
            this._view.style.backgroundColor = value;
        }
    };
    return ProgressElement;
}(ViewElement_1.ViewElement));
exports.ProgressElement = ProgressElement;
