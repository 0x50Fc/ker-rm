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
var Element_1 = require("./Element");
var events_1 = require("events");
var Document = /** @class */ (function (_super) {
    __extends(Document, _super);
    function Document(basePath) {
        var _this = _super.call(this) || this;
        _this._autoId = 0;
        _this._elementClass = {};
        _this._elements = {};
        _this._documentElement = new Element_1.Element(_this, "document", 0);
        _this._basePath = basePath;
        return _this;
    }
    Object.defineProperty(Document.prototype, "basePath", {
        get: function () {
            return this._basePath;
        },
        enumerable: true,
        configurable: true
    });
    Document.prototype.createElement = function (name) {
        var id = ++this._autoId;
        var fn = this._elementClass[name];
        var v;
        if (fn === undefined) {
            v = new Element_1.Element(this, name, id);
        }
        else {
            v = new fn(this, name, id);
        }
        this._elements[id] = v;
        return v;
    };
    Document.prototype.element = function (id) {
        return this._elements[id];
    };
    Document.prototype.removeElement = function (id) {
        delete this._elementClass[id];
    };
    Document.prototype.addElementClass = function (name, elementClass) {
        this._elementClass[name] = elementClass;
    };
    Object.defineProperty(Document.prototype, "documentElement", {
        get: function () {
            return this._documentElement;
        },
        enumerable: true,
        configurable: true
    });
    return Document;
}(events_1.EventEmitter));
exports.Document = Document;
