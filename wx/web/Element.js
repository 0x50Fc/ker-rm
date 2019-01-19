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
var Event_1 = require("./Event");
var EventEmitter_1 = require("./EventEmitter");
var ElementEvent = /** @class */ (function (_super) {
    __extends(ElementEvent, _super);
    function ElementEvent(element) {
        var _this = _super.call(this) || this;
        _this.cancelBubble = false;
        _this.element = element;
        return _this;
    }
    return ElementEvent;
}(Event_1.Event));
exports.ElementEvent = ElementEvent;
var Element = /** @class */ (function (_super) {
    __extends(Element, _super);
    function Element(document, name, id) {
        var _this = _super.call(this) || this;
        _this._attributes = {};
        _this._levelId = 0;
        _this._depth = 0;
        _this._autoLevelId = 0;
        _this._name = name;
        _this._id = id;
        _this._document = document;
        return _this;
    }
    Object.defineProperty(Element.prototype, "basePath", {
        get: function () {
            if (this._basePath === undefined) {
                if (this._parent !== undefined) {
                    return this._parent.basePath;
                }
                if (this._document !== undefined) {
                    return this._document.basePath;
                }
            }
            return this._basePath;
        },
        set: function (v) {
            this._basePath = v;
        },
        enumerable: true,
        configurable: true
    });
    Element.prototype.setData = function (key, value) {
    };
    Object.defineProperty(Element.prototype, "document", {
        get: function () {
            return this._document;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Element.prototype, "id", {
        get: function () {
            return this._id;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Element.prototype, "name", {
        get: function () {
            return this._name;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Element.prototype, "levelId", {
        get: function () {
            return this._levelId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Element.prototype, "depth", {
        get: function () {
            return this._depth;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Element.prototype, "firstChild", {
        get: function () {
            return this._firstChild;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Element.prototype, "lastChild", {
        get: function () {
            return this._lastChild;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Element.prototype, "nextSibling", {
        get: function () {
            return this._nextSibling;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Element.prototype, "prevSibling", {
        get: function () {
            return this._prevSibling;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Element.prototype, "parent", {
        get: function () {
            return this._parent;
        },
        enumerable: true,
        configurable: true
    });
    Element.prototype.onDidAddToParent = function (element) {
    };
    Element.prototype.onDidAddChildren = function (element) {
        var e = element;
        e._depth = this._depth + 1;
        e._levelId = this._autoLevelId + 1;
        this._autoLevelId = e._levelId;
        e.onDidAddToParent(this);
    };
    Element.prototype.append = function (element) {
        var e = element;
        e.remove();
        if (this._lastChild !== undefined) {
            this._lastChild._nextSibling = e;
            e._prevSibling = this._lastChild;
            this._lastChild = e;
            e._parent = this;
        }
        else {
            this._firstChild = e;
            this._lastChild = e;
            e._parent = this;
        }
        this.onDidAddChildren(element);
    };
    Element.prototype.before = function (element) {
        var e = element;
        e.remove();
        if (this._prevSibling !== undefined) {
            this._prevSibling._nextSibling = e;
            e._prevSibling = this._prevSibling;
            e._nextSibling = this;
            e._parent = this._parent;
            this._prevSibling = e;
        }
        else if (this._parent) {
            e._nextSibling = this;
            e._parent = this._parent;
            this._prevSibling = e;
            this._parent._firstChild = e;
        }
        if (this._parent !== undefined) {
            this._parent.onDidAddChildren(element);
        }
    };
    Element.prototype.after = function (element) {
        var e = element;
        e.remove();
        if (this._nextSibling !== undefined) {
            this._nextSibling._prevSibling = e;
            e._nextSibling = this._nextSibling;
            e._prevSibling = this;
            e._parent = this._parent;
            this._nextSibling = e;
        }
        else if (this._parent) {
            e._prevSibling = this;
            e._parent = this._parent;
            this._nextSibling = e;
            this._parent._lastChild = e;
        }
        if (this._parent !== undefined) {
            this._parent.onDidAddChildren(element);
        }
    };
    Element.prototype.onWillRemoveFromParent = function (element) {
    };
    Element.prototype.onWillRemoveChildren = function (element) {
        var e = element;
        e._depth = 0;
        e._levelId = 0;
        e.onWillRemoveFromParent(this);
    };
    Element.prototype.remove = function () {
        if (this._prevSibling !== undefined && this._parent !== undefined) {
            this._parent.onWillRemoveChildren(this);
            this._prevSibling._nextSibling = this._nextSibling;
            if (this._nextSibling !== undefined) {
                this._nextSibling._prevSibling = this._prevSibling;
            }
            else {
                this._parent._lastChild = this._prevSibling;
            }
        }
        else if (this._parent !== undefined) {
            this._parent.onWillRemoveChildren(this);
            this._parent._firstChild = this._nextSibling;
            if (this._nextSibling) {
                this._nextSibling._prevSibling = undefined;
            }
            else {
                this._parent._lastChild = undefined;
            }
        }
        if (this._parent) {
            this._parent = undefined;
            this._prevSibling = undefined;
            this._nextSibling = undefined;
        }
    };
    Element.prototype.appendTo = function (element) {
        element.append(this);
    };
    Element.prototype.beforeTo = function (element) {
        element.before(this);
    };
    Element.prototype.afterTo = function (element) {
        element.after(this);
    };
    Element.prototype.get = function (key) {
        return this._attributes[key];
    };
    Element.prototype.set = function (key, value) {
        if (value === undefined) {
            delete this._attributes[key];
        }
        else {
            this._attributes[key] = value;
        }
    };
    Object.defineProperty(Element.prototype, "attributes", {
        get: function () {
            return this._attributes;
        },
        enumerable: true,
        configurable: true
    });
    Element.prototype.emitBubble = function (name, event) {
        if (event instanceof ElementEvent) {
            var e = event;
            if (e.element === undefined) {
                e.element = this;
            }
        }
        this.emit(name, event);
        if (event instanceof ElementEvent) {
            var e = event;
            if (!e.cancelBubble) {
                if (this._parent !== undefined) {
                    this._parent.emit(name, event);
                }
                else if (this._document !== undefined) {
                    this._document.emit(name, event);
                }
            }
        }
    };
    Element.prototype.hasBubble = function (name) {
        if (_super.prototype.has.call(this, name)) {
            return true;
        }
        if (this._parent !== undefined) {
            return this._parent.hasBubble(name);
        }
        return false;
    };
    Element.prototype.recycle = function () {
        this._document.removeElement(this._id);
        var p = this._firstChild;
        while (p !== undefined) {
            var n = p._nextSibling;
            p.recycle();
            p = n;
        }
        this._parent = undefined;
        this._firstChild = undefined;
        this._lastChild = undefined;
        this._prevSibling = undefined;
        this._nextSibling = undefined;
    };
    Element.prototype.resolveURI = function (uri) {
        if (uri.indexOf("://") > 0) {
            return uri;
        }
        var basePath = this.basePath;
        if (basePath.endsWith("/")) {
            return basePath + uri;
        }
        else if (basePath) {
            return basePath + '/' + uri;
        }
        return uri;
    };
    Element.prototype.onEvent = function (name, data) {
    };
    return Element;
}(EventEmitter_1.EventEmitter));
exports.Element = Element;
function Each(element, func) {
    if (func(element)) {
        var p = element.firstChild;
        while (p) {
            Each(p, func);
            p = p.nextSibling;
        }
    }
}
exports.Each = Each;
