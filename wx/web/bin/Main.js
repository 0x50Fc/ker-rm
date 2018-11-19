"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const Data_1 = require("./Data");
const Element_1 = require("./Element");
const Page_1 = require("./Page");
const ViewElement_1 = require("./ViewElement");
const ImageElement_1 = require("./ImageElement");
const IPC_1 = require("./IPC");
const InputElement_1 = require("./InputElement");
const ScrollViewElement_1 = require("./ScrollViewElement");
const SwiperElement_1 = require("./SwiperElement");
const MovableViewElement_1 = require("./MovableViewElement");
const IconElement_1 = require("./IconElement");
const TextElement_1 = require("./TextElement");
const RichTextElement_1 = require("./RichTextElement");
const ProgressElement_1 = require("./ProgressElement");
const ButtonElement_1 = require("./ButtonElement");
const CheckboxElement_1 = require("./CheckboxElement");
const FormElement_1 = require("./FormElement");
const LabelElement_1 = require("./LabelElement");
const PickerElement_1 = require("./PickerElement");
const PickerViewElement_1 = require("./PickerViewElement");
const RadioElement_1 = require("./RadioElement");
const SliderElement_1 = require("./SliderElement");
const SwitchElement_1 = require("./SwitchElement");
const TextareaElement_1 = require("./TextareaElement");
const NavigatorElement_1 = require("./NavigatorElement");
const CanvasElement_1 = require("./CanvasElement");
const once_1 = require("./once");
function ElementOnEvent(element, prefix, name, value) {
    element.on(name, (event) => {
        if (event instanceof Element_1.ElementEvent) {
            IPC_1.postMessage({
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
    let evaluate = Data_1.Data.evaluateScript(attributes["wx:for"]);
    if (evaluate === undefined) {
        return;
    }
    delete attributes["wx:for"];
    let before = context.page.document.createElement("element");
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
                        element: context.page.document.createElement(name)
                    };
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
    let before = context.page.document.createElement("element");
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
                    item.element = context.page.document.createElement(item.name);
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
        let e = context.page.document.createElement(name);
        ElementSetAttributes(e, data, attributes);
        element.append(e);
        context.begin();
        children(e, data, context);
        context.end();
    }
}
exports.CreateElement = CreateElement;
var page = new Page_1.Page();
page.document.addElementClass("view", ViewElement_1.ViewElement);
page.document.addElementClass("input", InputElement_1.InputElement);
page.document.addElementClass("image", ImageElement_1.ImageElement);
page.document.addElementClass("scroll-view", ScrollViewElement_1.ScrollViewElement);
page.document.addElementClass("swiper", SwiperElement_1.SwiperElement);
page.document.addElementClass("movable-view", MovableViewElement_1.MovableViewElement);
page.document.addElementClass("icon", IconElement_1.IconElement);
page.document.addElementClass("text", TextElement_1.TextElement);
page.document.addElementClass("rich-text", RichTextElement_1.RichTextElement);
page.document.addElementClass("progress", ProgressElement_1.ProgressElement);
page.document.addElementClass("button", ButtonElement_1.ButtonElement);
page.document.addElementClass("checkbox", CheckboxElement_1.CheckboxElement);
page.document.addElementClass("form", FormElement_1.FormElement);
page.document.addElementClass("label", LabelElement_1.LabelElement);
page.document.addElementClass("picker", PickerElement_1.PickerElement);
page.document.addElementClass("picker-view", PickerViewElement_1.PickerViewElement);
page.document.addElementClass("radio", RadioElement_1.RadioElement);
page.document.addElementClass("slider", SliderElement_1.SliderElement);
page.document.addElementClass("switch", SwitchElement_1.SwitchElement);
page.document.addElementClass("textarea", TextareaElement_1.TextareaElement);
page.document.addElementClass("navigator", NavigatorElement_1.NavigatorElement);
page.document.addElementClass("canvas", CanvasElement_1.CanvasElement);
function Page(view, styleSheet, options) {
    IPC_1.postMessage({ page: 'readying' });
    view(page.document.documentElement, page.data, new Page_1.PageViewContext(page));
    once_1.once(() => { IPC_1.postMessage({ page: 'ready' }); });
}
exports.Page = Page;
function setData(data) {
    for (var key in data) {
        page.data.set([key], data[key]);
    }
    console.info("[DATA]", data);
}
exports.setData = setData;
function sendEvent(id, name, data) {
    let element = page.document.element(id);
    if (element != undefined) {
        element.onEvent(name, data);
    }
}
exports.sendEvent = sendEvent;
//# sourceMappingURL=Main.js.map