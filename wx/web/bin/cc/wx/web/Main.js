"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const Data_1 = require("./Data");
const Element_1 = require("./Element");
const Page_1 = require("./Page");
const ViewElement_1 = require("./ViewElement");
let elementClass = {
    "view": ViewElement_1.ViewElement
};
function CreateElementWithName(name) {
    let fn = elementClass[name];
    if (fn === undefined) {
        return new Element_1.Element();
    }
    return new fn();
}
function postMessage(data) {
    let w = window;
    if (w.webkit && w.webkit.messageHandlers && w.webkit.messageHandlers.kk) {
        w.webkit.messageHandlers.kk.postMessage(data);
    }
}
function ElementOnEvent(element, prefix, name, value) {
    element.on(name, (event) => {
        if (event instanceof Element_1.ElementEvent) {
            postMessage({
                event: value,
                data: event.data
            });
            if (prefix.endsWith("catch")) {
                event.cancelBubble = true;
            }
        }
    });
}
function ElementSetAttributes(element, data, attributes) {
    for (let key in attributes) {
        if (key.startsWith("wx:")) {
        }
        else if (key.startsWith("bind:")) {
            ElementOnEvent(element, key.substr(0, 4), key.substr(5), attributes[key]);
        }
        else if (key.startsWith("bind")) {
            ElementOnEvent(element, key.substr(0, 4), key.substr(4), attributes[key]);
        }
        else if (key.startsWith("catch:")) {
            ElementOnEvent(element, key.substr(0, 5), key.substr(6), attributes[key]);
        }
        else if (key.startsWith("catch")) {
            ElementOnEvent(element, key.substr(0, 5), key.substr(6), attributes[key]);
        }
        else if (key.startsWith("capture-bind:")) {
            ElementOnEvent(element, key.substr(0, 12), key.substr(13), attributes[key]);
        }
        else if (key.startsWith("capture-bind")) {
            ElementOnEvent(element, key.substr(0, 12), key.substr(12), attributes[key]);
        }
        else if (key.startsWith("capture-catch:")) {
            ElementOnEvent(element, key.substr(0, 13), key.substr(14), attributes[key]);
        }
        else if (key.startsWith("capture-catch")) {
            ElementOnEvent(element, key.substr(0, 13), key.substr(13), attributes[key]);
        }
        else {
            let v = attributes[key];
            let evaluate = Data_1.Data.evaluateScript(v);
            if (evaluate === undefined) {
                element.set(key, v);
            }
            else {
                let fn = (key, element, evaluate) => {
                    data.on(evaluate, (value, changdKeys) => {
                        element.set(key, value + '');
                    });
                };
                fn(key, element, evaluate);
            }
        }
    }
}
function CreateForElement(element, data, name, attributes, context, children) {
    debugger;
    let evaluate = Data_1.Data.evaluateScript(attributes["wx:for"]);
    if (evaluate === undefined) {
        return;
    }
    delete attributes["wx:for"];
    let before = new Element_1.Element();
    before.appendTo(element);
    let index = attributes["wx:for-index"] || "index";
    let item = attributes["wx:for-item"] || "item";
    let items = [];
    data.on(evaluate, (object, changedKeys) => {
        var i = 0;
        context.begin();
        if (object instanceof Array) {
            for (let d of object) {
                let v;
                if (i < items.length) {
                    v = items[i];
                }
                else {
                    v = {
                        data: new Data_1.Data(),
                        element: CreateElementWithName(name)
                    };
                    v.data.setParent(data);
                    ElementSetAttributes(element, v.data, attributes);
                    before.before(v.element);
                }
                v.data.set([index], i);
                v.data.set([item], d);
                i++;
            }
        }
        while (i < items.length) {
            let v = items.pop();
            v.element.remove();
            v.element.recycle();
            v.data.recycle();
        }
        context.end();
    });
}
function CreateIfElement(element, data, name, attributes, context, children) {
    let evaluate = Data_1.Data.evaluateScript(attributes["wx:if"]);
    if (evaluate === undefined) {
        return;
    }
    let before = new Element_1.Element();
    before.appendTo(element);
    let scope = context.scope();
    let block;
    block = new Page_1.IfBlock({
        element: undefined,
        name: name,
        attributes: attributes,
        children: children,
        evaluate: evaluate,
        data: data,
        elementData: undefined
    }, (value, changedKeys) => {
        let e;
        for (let item of block.elements) {
            if (e === undefined) {
                if (item.evaluate !== undefined) {
                    let v = item.evaluate.evaluateScript(item.data.object);
                    if (v) {
                        e = item;
                    }
                }
                else {
                    e = item;
                }
            }
            if (e == item) {
                if (item.element === undefined) {
                    item.element = CreateElementWithName(item.name);
                    item.elementData = new Data_1.Data();
                    ElementSetAttributes(item.element, item.data, item.attributes);
                    context.begin();
                    item.children(item.element, item.elementData, context);
                    context.end();
                    item.elementData.setParent(item.data);
                }
                else {
                    before.before(item.element);
                    item.elementData.changedKeys([]);
                }
            }
            else if (item.element !== undefined) {
                item.element.remove();
                item.element.recycle();
                item.elementData.recycle();
                item.element = undefined;
                item.elementData = undefined;
            }
        }
    });
    scope.ifblock = block;
    data.on(evaluate, block.func);
}
function CreateElifElement(element, data, name, attributes, context, children) {
    let scope = context.scope();
    if (scope.ifblock !== undefined) {
        let evaluate = Data_1.Data.evaluateScript(attributes["wx:elif"]);
        if (evaluate === undefined) {
            return;
        }
        scope.ifblock.elements.push({
            element: undefined,
            name: name,
            attributes: attributes,
            children: children,
            evaluate: evaluate,
            data: data,
            elementData: undefined
        });
        data.on(evaluate, scope.ifblock.func);
    }
}
function CreateElseElement(element, data, name, attributes, context, children) {
    let scope = context.scope();
    if (scope.ifblock !== undefined) {
        scope.ifblock.elements.push({
            element: undefined,
            name: name,
            attributes: attributes,
            children: children,
            evaluate: undefined,
            data: data,
            elementData: undefined
        });
        scope.ifblock = undefined;
    }
}
function CreateElement(element, data, name, attributes, context, children) {
    if (attributes["wx:for"] !== undefined) {
        CreateForElement(element, data, name, attributes, context, children);
    }
    else if (attributes["wx:if"] !== undefined) {
        CreateIfElement(element, data, name, attributes, context, children);
    }
    else if (attributes["wx:elif"] !== undefined) {
        CreateElifElement(element, data, name, attributes, context, children);
    }
    else if (attributes["wx:else"] !== undefined) {
        CreateElseElement(element, data, name, attributes, context, children);
    }
    else {
        let e = CreateElementWithName(name);
        ElementSetAttributes(e, data, attributes);
        element.append(e);
        context.begin();
        children(e, data, context);
        context.end();
    }
}
exports.CreateElement = CreateElement;
var page = new Page_1.Page();
function Page(view, styleSheet, options) {
    postMessage({ page: 'readying' });
    view(page.element, page.data, new Page_1.PageViewContext());
    postMessage({ page: 'ready' });
}
exports.Page = Page;
function setData(data) {
    debugger;
    for (var key in data) {
        page.data.set([key], data[key]);
    }
    console.info("[DATA]", data);
}
exports.setData = setData;
//# sourceMappingURL=Main.js.map