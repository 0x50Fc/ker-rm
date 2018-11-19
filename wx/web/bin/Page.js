"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const Data_1 = require("./Data");
const Document_1 = require("./Document");
class IfBlock {
    constructor(element, func) {
        this.elements = [element];
        this.func = func;
    }
}
exports.IfBlock = IfBlock;
class PageViewScope {
}
exports.PageViewScope = PageViewScope;
class PageViewContext {
    constructor(page) {
        this._page = page;
        this._scopes = [new PageViewScope()];
    }
    get page() {
        return this._page;
    }
    begin() {
        this._scopes.push(new PageViewScope());
    }
    end() {
        this._scopes.pop();
    }
    scope() {
        return this._scopes[this._scopes.length - 1];
    }
}
exports.PageViewContext = PageViewContext;
class Page {
    constructor() {
        this._document = new Document_1.Document();
        this._data = new Data_1.Data();
    }
    get document() {
        return this._document;
    }
    get data() {
        return this._data;
    }
}
exports.Page = Page;
//# sourceMappingURL=Page.js.map