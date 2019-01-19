"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var Data_1 = require("./Data");
var Element_1 = require("./Element");
var Page_1 = require("./Page");
var ViewElement_1 = require("./ViewElement");
var ImageElement_1 = require("./ImageElement");
var IPC_1 = require("./IPC");
var InputElement_1 = require("./InputElement");
var ScrollViewElement_1 = require("./ScrollViewElement");
var SwiperElement_1 = require("./SwiperElement");
var MovableViewElement_1 = require("./MovableViewElement");
var IconElement_1 = require("./IconElement");
var TextElement_1 = require("./TextElement");
var RichTextElement_1 = require("./RichTextElement");
var ProgressElement_1 = require("./ProgressElement");
var ButtonElement_1 = require("./ButtonElement");
var CheckboxElement_1 = require("./CheckboxElement");
var FormElement_1 = require("./FormElement");
var LabelElement_1 = require("./LabelElement");
var PickerElement_1 = require("./PickerElement");
var PickerViewElement_1 = require("./PickerViewElement");
var RadioElement_1 = require("./RadioElement");
var SliderElement_1 = require("./SliderElement");
var SwitchElement_1 = require("./SwitchElement");
var TextareaElement_1 = require("./TextareaElement");
var NavigatorElement_1 = require("./NavigatorElement");
var CanvasElement_1 = require("./CanvasElement");
var once_1 = require("./once");
var BlockElement_1 = require("./BlockElement");
var ComponentElement_1 = require("./ComponentElement");
var ModalElement_1 = require("./ModalElement");
function getComponentId(element) {
    if (element === undefined) {
        return undefined;
    }
    if (element instanceof ComponentElement_1.ComponentElement) {
        return element.id;
    }
    return getComponentId(element.parent);
}
function ElementOnEvent(element, prefix, name, value) {
    element.on(name, function (event) {
        if (event instanceof Element_1.ElementEvent) {
            IPC_1.postMessage({
                event: value,
                data: event.data,
                componentId: getComponentId(element.parent)
            });
            if (prefix.endsWith("catch")) {
                event.cancelBubble = true;
            }
        }
    });
}
function ElementSetAttributes(element, data, attributes) {
    for (var key in attributes) {
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
        else if (key.startsWith("data-")) {
            var name_1 = key.substr(5);
            var v = attributes[key];
            if (typeof v == 'string') {
                element.setData(name_1, v);
            }
            else if (v instanceof Data_1.Evaluate) {
                var fn = function (name, element, evaluate) {
                    data.on(evaluate, function (value, changdKeys) {
                        element.setData(name, value);
                    });
                };
                fn(name_1, element, v);
            }
        }
        else {
            var v = attributes[key];
            if (typeof v == 'string') {
                element.set(key, v);
            }
            else if (v instanceof Data_1.Evaluate) {
                var fn = function (key, element, evaluate) {
                    data.on(evaluate, function (value, changdKeys) {
                        if (value === undefined) {
                            element.set(key, undefined);
                        }
                        else if (typeof value == 'object') {
                            element.set(key, JSON.stringify(value));
                        }
                        else {
                            element.set(key, value + '');
                        }
                    });
                };
                fn(key, element, v);
            }
        }
    }
}
function CreateForElement(forKey, element, data, name, attributes, context, children, basePath) {
    var evaluate = attributes[forKey];
    if (evaluate === undefined) {
        return;
    }
    if (!(evaluate instanceof Data_1.Evaluate)) {
        return;
    }
    delete attributes[forKey];
    var before = context.page.document.createElement("element");
    before.appendTo(element);
    var index = attributes["wx:for-index"] || "index";
    var item = attributes["wx:for-item"] || "item";
    var items = [];
    data.on(evaluate, function (object, changedKeys) {
        var i = 0;
        context.begin();
        if (object instanceof Array) {
            for (var _i = 0, object_1 = object; _i < object_1.length; _i++) {
                var d = object_1[_i];
                var v = void 0;
                if (i < items.length) {
                    v = items[i];
                }
                else {
                    v = {
                        data: new Data_1.Data(),
                        element: context.page.document.createElement(name)
                    };
                    v.element.basePath = basePath;
                    ElementSetAttributes(v.element, v.data, attributes);
                    before.before(v.element);
                    context.begin();
                    children(v.element, v.data, context);
                    context.end();
                    v.data.setParent(data);
                    items.push(v);
                }
                v.data.set([index], i, false);
                v.data.set([item], d, false);
                v.data.changedKeys([]);
                i++;
            }
        }
        while (i < items.length) {
            var v = items.pop();
            v.element.remove();
            v.element.recycle();
            v.data.recycle();
        }
        context.end();
    });
}
function CreateIfElement(element, data, name, attributes, context, children, basePath) {
    var evaluate = attributes["wx:if"];
    if (evaluate === undefined) {
        return;
    }
    if (!(evaluate instanceof Data_1.Evaluate)) {
        return;
    }
    var before = context.page.document.createElement("element");
    before.appendTo(element);
    delete attributes["wx:if"];
    var scope = context.scope();
    var block;
    block = new Page_1.IfBlock({
        element: undefined,
        name: name,
        attributes: attributes,
        children: children,
        evaluate: evaluate,
        data: data,
        elementData: undefined
    }, function (value, changedKeys) {
        var e;
        for (var _i = 0, _a = block.elements; _i < _a.length; _i++) {
            var item = _a[_i];
            if (e === undefined) {
                if (item.evaluate !== undefined) {
                    var v = item.evaluate.exec(item.data.object);
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
                    item.element = context.page.document.createElement(item.name);
                    item.elementData = new Data_1.Data();
                    ElementSetAttributes(item.element, item.elementData, item.attributes);
                    before.before(item.element);
                    context.begin();
                    item.children(item.element, item.elementData, context);
                    context.end();
                    item.elementData.setParent(item.data);
                    item.elementData.changedKeys([]);
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
function CreateElifElement(element, data, name, attributes, context, children, basePath) {
    var scope = context.scope();
    if (scope.ifblock !== undefined) {
        var evaluate = attributes["wx:elif"];
        if (evaluate === undefined) {
            return;
        }
        if (!(evaluate instanceof Data_1.Evaluate)) {
            return;
        }
        delete attributes["wx:elif"];
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
function CreateElseElement(element, data, name, attributes, context, children, basePath) {
    var scope = context.scope();
    if (scope.ifblock !== undefined) {
        delete attributes["wx:else"];
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
function CreateElement(element, data, name, attributes, context, children, basePath) {
    if (attributes["wx:for"] !== undefined) {
        CreateForElement("wx:for", element, data, name, attributes, context, children, basePath);
    }
    else if (attributes["wx:for-items"] !== undefined) {
        CreateForElement("wx:for-items", element, data, name, attributes, context, children, basePath);
    }
    else if (attributes["wx:if"] !== undefined) {
        CreateIfElement(element, data, name, attributes, context, children, basePath);
    }
    else if (attributes["wx:elif"] !== undefined) {
        CreateElifElement(element, data, name, attributes, context, children, basePath);
    }
    else if (attributes["wx:else"] !== undefined) {
        CreateElseElement(element, data, name, attributes, context, children, basePath);
    }
    else {
        var e = context.page.document.createElement(name);
        e.basePath = basePath;
        ElementSetAttributes(e, data, attributes);
        element.append(e);
        context.begin();
        children(e, data, context);
        context.end();
    }
}
exports.CreateElement = CreateElement;
function CreateTElement(element, data, attributes, func, context, basePath) {
    var v = new Data_1.Data();
    func(element, v, context, basePath);
    var evaluate = attributes['data'];
    if (evaluate !== undefined && evaluate instanceof Data_1.Evaluate) {
        data.on(evaluate, function (object, changedKeys) {
            context.begin();
            if (typeof object == 'object') {
                for (var key in object) {
                    v.set([key], object[key]);
                }
            }
            context.end();
        });
    }
}
exports.CreateTElement = CreateTElement;
function CreateCElement(element, data, name, attributes, children, context, path) {
    var basePath = path;
    if (basePath !== undefined) {
        var i = basePath.lastIndexOf("/");
        if (i >= 0) {
            basePath = basePath.substr(0, i);
        }
    }
    CreateElement(element, data, "component", attributes, context, function (element, data, context) {
        if (element instanceof ComponentElement_1.ComponentElement) {
            element.componentName = name;
            element.postAddMessage(path);
            children(element, element.data, context);
        }
    }, basePath);
}
exports.CreateCElement = CreateCElement;
function CreateEvaluate(evaluateScript, keys) {
    return new Data_1.Evaluate(evaluateScript, keys);
}
exports.CreateEvaluate = CreateEvaluate;
var page = new Page_1.Page(window.__basePath || '');
page.document.addElementClass("view", ViewElement_1.ViewElement);
page.document.addElementClass("input", InputElement_1.InputElement);
page.document.addElementClass("image", ImageElement_1.ImageElement);
page.document.addElementClass("scroll-view", ScrollViewElement_1.ScrollViewElement);
page.document.addElementClass("swiper", SwiperElement_1.SwiperElement);
page.document.addElementClass("swiper-item", SwiperElement_1.SwiperItemElement);
page.document.addElementClass("movable-view", MovableViewElement_1.MovableViewElement);
page.document.addElementClass("icon", IconElement_1.IconElement);
page.document.addElementClass("text", TextElement_1.TextElement);
page.document.addElementClass("rich-text", RichTextElement_1.RichTextElement);
page.document.addElementClass("progress", ProgressElement_1.ProgressElement);
page.document.addElementClass("button", ButtonElement_1.ButtonElement);
page.document.addElementClass("checkbox", CheckboxElement_1.CheckboxElement);
page.document.addElementClass("checkbox-group", CheckboxElement_1.CheckboxGroupElement);
page.document.addElementClass("form", FormElement_1.FormElement);
page.document.addElementClass("label", LabelElement_1.LabelElement);
page.document.addElementClass("picker", PickerElement_1.PickerElement);
page.document.addElementClass("picker-view", PickerViewElement_1.PickerViewElement);
page.document.addElementClass("radio", RadioElement_1.RadioElement);
page.document.addElementClass("radio-group", RadioElement_1.RadioGroupElement);
page.document.addElementClass("slider", SliderElement_1.SliderElement);
page.document.addElementClass("switch", SwitchElement_1.SwitchElement);
page.document.addElementClass("textarea", TextareaElement_1.TextareaElement);
page.document.addElementClass("navigator", NavigatorElement_1.NavigatorElement);
page.document.addElementClass("canvas", CanvasElement_1.CanvasElement);
page.document.addElementClass("block", BlockElement_1.BlockElement);
page.document.addElementClass("component", ComponentElement_1.ComponentElement);
page.document.addElementClass("modal", ModalElement_1.ModalElement);
function Page(view, styleSheet, options) {
    IPC_1.postMessage({ page: 'readying' });
    view(page.document.documentElement, page.data, new Page_1.PageViewContext(page));
    page.data.changedKeys([]);
    once_1.once(function () { IPC_1.postMessage({ page: 'ready' }); });
}
exports.Page = Page;
function setPageData(page, data) {
    var changedKeySet = {};
    var changedKeys = [];
    for (var key in page) {
        var ks = key.split(".");
        if (!changedKeySet[ks[0]]) {
            changedKeys.push(ks[0]);
        }
        data.set(ks, page[key], false);
    }
    if (changedKeys.length == 0) {
        data.changedKeys([]);
    }
    else {
        for (var _i = 0, changedKeys_1 = changedKeys; _i < changedKeys_1.length; _i++) {
            var key_1 = changedKeys_1[_i];
            data.changedKeys([key_1]);
        }
    }
}
function setData(data, componentId) {
    if (componentId === undefined) {
        setPageData(data, page.data);
        console.info("[DATA]", data);
    }
    else {
        var e = page.document.element(componentId);
        if (e !== undefined && e instanceof ComponentElement_1.ComponentElement) {
            setPageData(data, e.data);
            console.info("[COMPONENT] [DATA]", data);
        }
    }
}
exports.setData = setData;
function sendEvent(id, name, data) {
    var element = page.document.element(id);
    if (element != undefined) {
        element.onEvent(name, data);
    }
}
exports.sendEvent = sendEvent;
