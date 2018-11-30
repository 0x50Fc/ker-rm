import { Data, Evaluate, EvaluateScript } from './Data';
import { Element, ElementEvent } from './Element';
import { Event } from './Event';
import { StyleSheet } from "./Style";
import { PageOptions, PageData, PageView, PageViewContext, IfBlock, PageElement, Page as PageObject,AttributeMap } from "./Page";
import { ViewElement } from './ViewElement';
import { ImageElement } from './ImageElement';
import { postMessage } from './IPC';
import { InputElement } from './InputElement';
import { ScrollViewElement } from './ScrollViewElement';
import { SwiperElement, SwiperItemElement } from './SwiperElement';
import { MovableViewElement } from './MovableViewElement';
import { IconElement } from './IconElement';
import { TextElement } from './TextElement';
import { RichTextElement } from './RichTextElement';
import { ProgressElement } from './ProgressElement';
import { ButtonElement } from './ButtonElement';
import { CheckboxElement, CheckboxGroupElement } from './CheckboxElement';
import { FormElement } from './FormElement';
import { LabelElement } from './LabelElement';
import { PickerElement } from './PickerElement';
import { PickerViewElement } from './PickerViewElement';
import { RadioElement, RadioGroupElement } from './RadioElement';
import { SliderElement } from './SliderElement';
import { SwitchElement } from './SwitchElement';
import { TextareaElement } from './TextareaElement';
import { NavigatorElement } from './NavigatorElement';
import { CanvasElement } from './CanvasElement';
import { once } from './once';
import { BlockElement } from './BlockElement';


function ElementOnEvent(element: Element, prefix: string, name: string, value: string): void {

    element.on(name, (event: Event): void => {

        if (event instanceof ElementEvent) {

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

function ElementSetAttributes(element: Element, data: Data, attributes: AttributeMap): void {

    for (let key in attributes) {
        if (key.startsWith("wx:")) {
        } else if (key.startsWith("bind:")) {
            ElementOnEvent(element, key.substr(0, 4), key.substr(5), attributes[key] as string);
        } else if (key.startsWith("bind")) {
            ElementOnEvent(element, key.substr(0, 4), key.substr(4), attributes[key] as string);
        } else if (key.startsWith("catch:")) {
            ElementOnEvent(element, key.substr(0, 5), key.substr(6), attributes[key] as string);
        } else if (key.startsWith("catch")) {
            ElementOnEvent(element, key.substr(0, 5), key.substr(6), attributes[key] as string);
        } else if (key.startsWith("capture-bind:")) {
            ElementOnEvent(element, key.substr(0, 12), key.substr(13), attributes[key] as string);
        } else if (key.startsWith("capture-bind")) {
            ElementOnEvent(element, key.substr(0, 12), key.substr(12), attributes[key] as string);
        } else if (key.startsWith("capture-catch:")) {
            ElementOnEvent(element, key.substr(0, 13), key.substr(14), attributes[key] as string);
        } else if (key.startsWith("capture-catch")) {
            ElementOnEvent(element, key.substr(0, 13), key.substr(13), attributes[key] as string);
        } else {
            let v = attributes[key];
            if(typeof v == 'string') {
                element.set(key, v);
            } else if(v instanceof Evaluate) {
                let fn = (key: string, element: Element, evaluate: Evaluate): void => {
                    data.on(evaluate, (value: any, changdKeys: string[]): void => {
                        if (value === undefined) {
                            element.set(key, undefined);
                        } else {
                            element.set(key, value + '');
                        }
                    });
                };
                fn(key, element, v);
            }
        }
    }
}

interface ForItem {
    element: Element
    data: Data
}

function CreateForElement(forKey: string, element: Element, data: Data, name: string, attributes: AttributeMap, context: PageViewContext, children: PageView): void {

    let evaluate = attributes[forKey];

    if (evaluate === undefined) {
        return;
    }

    if(!(evaluate instanceof Evaluate)) {
        return;
    }

    delete attributes[forKey];

    let before = context.page.document.createElement("element");
    before.appendTo(element);

    let index = attributes["wx:for-index"] as string || "index";
    let item = attributes["wx:for-item"] as string || "item";
    let items: ForItem[] = [];

    data.on(evaluate, (object: any, changedKeys: string[]): void => {

        var i = 0;

        context.begin();

        if (object instanceof Array) {
            for (let d of object) {
                let v: ForItem;
                if (i < items.length) {
                    v = items[i];
                } else {
                    v = {
                        data: new Data(),
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
            let v = items.pop()!;
            v.element.remove();
            v.element.recycle();
            v.data.recycle();
        }

        context.end();

    });

}

function CreateIfElement(element: Element, data: Data, name: string, attributes: AttributeMap, context: PageViewContext, children: PageView): void {

    let evaluate = attributes["wx:if"];

    if (evaluate === undefined) {
        return;
    }

    if(!(evaluate instanceof Evaluate)) {
        return;
    }

    let before = context.page.document.createElement("element");
    before.appendTo(element);

    let scope = context.scope();

    let block: IfBlock;

    block = new IfBlock({
        element: undefined,
        name: name,
        attributes: attributes,
        children: children,
        evaluate: evaluate,
        data: data,
        elementData: undefined
    }, (value: any, changedKeys: string[]): void => {

        let e: PageElement | undefined;

        for (let item of block.elements) {

            if (e === undefined) {
                if (item.evaluate !== undefined) {
                    let v = item.evaluate.exec(item.data.object);
                    if (v) {
                        e = item;
                    }
                } else {
                    e = item;
                }
            }

            if (e == item) {

                if (item.element === undefined) {
                    item.element = context.page.document.createElement(item.name);
                    item.elementData = new Data();
                    ElementSetAttributes(item.element, item.data, item.attributes);
                    context.begin();
                    item.children(item.element, item.elementData, context);
                    context.end();
                    item.elementData.setParent(item.data);
                } else {
                    before.before(item.element);
                    item.elementData!.changedKeys([]);
                }

            } else if (item.element !== undefined) {
                item.element.remove();
                item.element.recycle();
                item.elementData!.recycle();
                item.element = undefined;
                item.elementData = undefined;
            }

        }

    });

    scope.ifblock = block;

    data.on(evaluate, block.func);

}

function CreateElifElement(element: Element, data: Data, name: string, attributes: AttributeMap, context: PageViewContext, children: PageView): void {

    let scope = context.scope();

    if (scope.ifblock !== undefined) {

        let evaluate = attributes["wx:elif"];

        if (evaluate === undefined) {
            return;
        }

        if(!(evaluate instanceof Evaluate)) {
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

function CreateElseElement(element: Element, data: Data, name: string, attributes: AttributeMap, context: PageViewContext, children: PageView): void {

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

export function CreateElement(element: Element, data: Data, name: string, attributes: AttributeMap, context: PageViewContext, children: PageView): void {

    if (attributes["wx:for"] !== undefined) {
        CreateForElement("wx:for", element, data, name, attributes, context, children);
    } else if (attributes["wx:for-items"] !== undefined) {
        CreateForElement("wx:for-items", element, data, name, attributes, context, children);
    } else if (attributes["wx:if"] !== undefined) {
        CreateIfElement(element, data, name, attributes, context, children);
    } else if (attributes["wx:elif"] !== undefined) {
        CreateElifElement(element, data, name, attributes, context, children);
    } else if (attributes["wx:else"] !== undefined) {
        CreateElseElement(element, data, name, attributes, context, children);
    } else {
        let e = context.page.document.createElement(name);
        ElementSetAttributes(e, data, attributes);
        element.append(e);
        context.begin();
        children(e, data, context);
        context.end();
    }

}

export function CreateTElement(element: Element, data: Data, attributes: AttributeMap, func: (element: Element, data: Data, context: PageViewContext) => void, context: PageViewContext): void {

    let v = new Data();

    func(element, v, context);

    let evaluate = attributes['data'];

    if (evaluate !== undefined && evaluate instanceof Evaluate) {

        data.on(evaluate, (object: any, changedKeys: string[]): void => {

            context.begin();

            if (typeof object == 'object') {
                for (let key in object) {
                    v.set([key], object[key]);
                }
            }

            context.end();

        });
    } 

}

export function CreateEvaluate(evaluateScript:EvaluateScript,keys:string[][]):Evaluate {
    return new Evaluate(evaluateScript,keys);
}

var page = new PageObject();

page.document.addElementClass("view", ViewElement);
page.document.addElementClass("input", InputElement);
page.document.addElementClass("image", ImageElement);
page.document.addElementClass("scroll-view", ScrollViewElement);
page.document.addElementClass("swiper", SwiperElement);
page.document.addElementClass("swiper-item", SwiperItemElement);
page.document.addElementClass("movable-view", MovableViewElement);
page.document.addElementClass("icon", IconElement);
page.document.addElementClass("text", TextElement);
page.document.addElementClass("rich-text", RichTextElement);
page.document.addElementClass("progress", ProgressElement);
page.document.addElementClass("button", ButtonElement);
page.document.addElementClass("checkbox", CheckboxElement);
page.document.addElementClass("checkbox-group", CheckboxGroupElement);
page.document.addElementClass("form", FormElement);
page.document.addElementClass("label", LabelElement);
page.document.addElementClass("picker", PickerElement);
page.document.addElementClass("picker-view", PickerViewElement);
page.document.addElementClass("radio", RadioElement);
page.document.addElementClass("radio-group", RadioGroupElement);
page.document.addElementClass("slider", SliderElement);
page.document.addElementClass("switch", SwitchElement);
page.document.addElementClass("textarea", TextareaElement);
page.document.addElementClass("navigator", NavigatorElement);
page.document.addElementClass("canvas", CanvasElement);
page.document.addElementClass("block", BlockElement);

export function Page(view: PageView, styleSheet: StyleSheet, options: PageOptions): void {
    postMessage({ page: 'readying' });
    view(page.document.documentElement, page.data, new PageViewContext(page));
    page.data.changedKeys([]);
    once((): void => { postMessage({ page: 'ready' }); });
}

export function setData(data: PageData): void {

    for (var key in data) {
        page.data.set([key], data[key]);
    }

    console.info("[DATA]", data);

}

export function sendEvent(id: number, name: string, data: any): void {
    let element = page.document.element(id);
    if (element != undefined) {
        element.onEvent(name, data);
    }
}
