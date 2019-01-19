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
//<modal title="{{dialog.title}}" hidden="{{dialog.hidden}}" no-cancel bindconfirm="confirm">{{dialog.content}}</modal>
var ModalElement = /** @class */ (function (_super) {
    __extends(ModalElement, _super);
    function ModalElement(document, name, id) {
        return _super.call(this, document, name, id) || this;
    }
    ModalElement.prototype.createView = function () {
        var v = _super.prototype.createView.call(this);
        this._boxView = document.createElement("div");
        this._boxView.className = "wx-modal-box";
        this._titleView = document.createElement("div");
        this._titleView.className = "wx-modal-title";
        this._contentView = document.createElement("div");
        this._contentView.className = "wx-modal-content";
        this._bottomView = document.createElement("div");
        this._bottomView.className = "wx-modal-bottom";
        this._cancelView = document.createElement("div");
        this._cancelView.className = "wx-modal-cancel";
        this._confirmView = document.createElement("div");
        this._confirmView.className = "wx-modal-confirm";
        v.appendChild(this._boxView);
        this._boxView.appendChild(this._titleView);
        this._boxView.appendChild(this._contentView);
        this._boxView.appendChild(this._bottomView);
        this._bottomView.appendChild(this._cancelView);
        this._bottomView.appendChild(this._confirmView);
        this._cancelView.innerText = '取消';
        this._confirmView.innerText = '确定';
        var element = this;
        this._confirmView.addEventListener("mouseup", function (event) {
            element.doEvent(event, "confirm", {});
            element.view.style.display = 'none';
        });
        this._cancelView.addEventListener("mouseup", function (event) {
            element.doEvent(event, "cancel", {});
            element.view.style.display = 'none';
        });
        return v;
    };
    ModalElement.prototype.set = function (key, value) {
        if (key == "title") {
            this._titleView.innerText = value || '';
            this._titleView.style.display = value ? 'block' : 'none';
        }
        else if (key == '#text') {
            this._contentView.innerText = value || '';
            this._contentView.style.display = value ? 'block' : 'none';
        }
        else if (key == 'no-cancel') {
            this._cancelView.style.display = 'none';
            this._confirmView.style.width = '100%';
        }
        else if (key == 'hidden') {
            if (V_1.booleanValue(value)) {
                this.view.style.display = 'none';
            }
            else {
                this.view.style.display = 'block';
            }
        }
        else {
            _super.prototype.set.call(this, key, value);
        }
    };
    return ModalElement;
}(ViewElement_1.ViewElement));
exports.ModalElement = ModalElement;
