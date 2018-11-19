"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const Data_1 = require("./Data");
const Element_1 = require("./Element");
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
    constructor() {
        this._scopes = [new PageViewScope()];
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
        this._element = new Element_1.Element();
        this._data = new Data_1.Data();
    }
    get element() {
        return this._element;
    }
    get data() {
        return this._data;
    }
}
exports.Page = Page;
//# sourceMappingURL=Page.js.map