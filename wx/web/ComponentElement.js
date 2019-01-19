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
var BlockElement_1 = require("./BlockElement");
var Data_1 = require("./Data");
var Element_1 = require("./Element");
var IPC_1 = require("./IPC");
var ComponentElement = /** @class */ (function (_super) {
    __extends(ComponentElement, _super);
    function ComponentElement(document, name, id) {
        var _this = _super.call(this, document, name, id) || this;
        _this._data = new Data_1.Data();
        return _this;
    }
    Object.defineProperty(ComponentElement.prototype, "data", {
        get: function () {
            return this._data;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ComponentElement.prototype, "componentName", {
        get: function () {
            return this._componentName;
        },
        set: function (v) {
            this._componentName = v;
        },
        enumerable: true,
        configurable: true
    });
    ComponentElement.prototype.postAddMessage = function (path) {
        IPC_1.postMessage({
            component: 'add',
            path: path,
            id: this._id
        });
    };
    ComponentElement.prototype.onWillRemoveFromParent = function (element) {
        _super.prototype.onWillRemoveFromParent.call(this, element);
        IPC_1.postMessage({
            component: 'remove',
            id: this._id
        });
    };
    ComponentElement.prototype.set = function (key, value) {
        _super.prototype.set.call(this, key, value);
        IPC_1.postMessage({
            component: 'set',
            id: this._id,
            name: key,
            value: value
        });
    };
    ComponentElement.prototype.emit = function (name, event) {
        _super.prototype.emit.call(this, name, event);
        if (event instanceof Element_1.ElementEvent) {
            event.cancelBubble = true;
        }
    };
    ComponentElement.prototype.onEvent = function (name, data) {
        var event = new Element_1.ElementEvent(this);
        event.data = data;
        _super.prototype.emit.call(this, name, event);
        if (!event.cancelBubble) {
            if (this.parent) {
                this.parent.emitBubble(name, event);
            }
            else if (this.document) {
                this.document.emit(name, event);
            }
        }
    };
    return ComponentElement;
}(BlockElement_1.BlockElement));
exports.ComponentElement = ComponentElement;
