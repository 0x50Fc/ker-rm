"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var Data_1 = require("./Data");
var Document_1 = require("./Document");
var IfBlock = /** @class */ (function () {
    function IfBlock(element, func) {
        this.elements = [element];
        this.func = func;
    }
    return IfBlock;
}());
exports.IfBlock = IfBlock;
var PageViewScope = /** @class */ (function () {
    function PageViewScope() {
    }
    return PageViewScope;
}());
exports.PageViewScope = PageViewScope;
var PageViewContext = /** @class */ (function () {
    function PageViewContext(page) {
        this._page = page;
        this._scopes = [new PageViewScope()];
    }
    Object.defineProperty(PageViewContext.prototype, "page", {
        get: function () {
            return this._page;
        },
        enumerable: true,
        configurable: true
    });
    PageViewContext.prototype.begin = function () {
        this._scopes.push(new PageViewScope());
    };
    PageViewContext.prototype.end = function () {
        this._scopes.pop();
    };
    PageViewContext.prototype.scope = function () {
        return this._scopes[this._scopes.length - 1];
    };
    return PageViewContext;
}());
exports.PageViewContext = PageViewContext;
var Page = /** @class */ (function () {
    function Page(basePath) {
        this._document = new Document_1.Document(basePath);
        this._data = new Data_1.Data();
    }
    Object.defineProperty(Page.prototype, "document", {
        get: function () {
            return this._document;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Page.prototype, "data", {
        get: function () {
            return this._data;
        },
        enumerable: true,
        configurable: true
    });
    return Page;
}());
exports.Page = Page;
