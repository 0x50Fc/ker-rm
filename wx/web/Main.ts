import { Data, Evaluate, EvaluateScript } from './Data';
import { Element, ElementEvent } from './Element';
import { Event } from './Event';
import { StyleSheet } from "./Style";
import { PageOptions, PageData, PageView, PageViewContext, IfBlock, PageElement, Page as PageObject, AttributeMap } from "./Page";
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
import { ComponentElement } from './ComponentElement';

function getComponentId(element: Element | undefined): number | undefined {

    if (element === undefined) {
        return undefined;
    }

    if (element instanceof ComponentElement) {
        return element.id;
    }

    return getComponentId(element.parent);
}

function ElementOnEvent(element: Element, prefix: string, name: string, value: string): void {

    element.on(name, (event: Event): void => {

        if (event instanceof ElementEvent) {

            postMessage({
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
        } else if (key.startsWith("data-")) {
            let name = key.substr(5);
            let v = attributes[key];
            if (typeof v == 'string') {
                element.setData(name, v);
            } else if (v instanceof Evaluate) {
                let fn = (name: string, element: Element, evaluate: Evaluate): void => {
                    data.on(evaluate, (value: any, changdKeys: string[]): void => {
                        element.setData(name, value);
                    });
                };
                fn(name, element, v);
            }
        } else {
            let v = attributes[key];
            if (typeof v == 'string') {
                element.set(key, v);
            } else if (v instanceof Evaluate) {
                let fn = (key: string, element: Element, evaluate: Evaluate): void => {
                    data.on(evaluate, (value: any, changdKeys: string[]): void => {
                        if (value === undefined) {
                            element.set(key, undefined);
                        } else if (typeof value == 'object') {
                            element.set(key, JSON.stringify(value));
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

function CreateForElement(forKey: string, element: Element, data: Data, name: string, attributes: AttributeMap, context: PageViewContext, children: PageView, basePath: string | undefined): void {

    let evaluate = attributes[forKey];

    if (evaluate === undefined) {
        return;
    }

    if (!(evaluate instanceof Evaluate)) {
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
            let v = items.pop()!;
            v.element.remove();
            v.element.recycle();
            v.data.recycle();
        }

        context.end();

    });

}

function CreateIfElement(element: Element, data: Data, name: string, attributes: AttributeMap, context: PageViewContext, children: PageView, basePath: string | undefined): void {

    let evaluate = attributes["wx:if"];

    if (evaluate === undefined) {
        return;
    }

    if (!(evaluate instanceof Evaluate)) {
        return;
    }

    let before = context.page.document.createElement("element");
    before.appendTo(element);

    delete attributes["wx:if"];

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
                    ElementSetAttributes(item.element, item.elementData, item.attributes);
                    before.before(item.element);
                    context.begin();
                    item.children(item.element, item.elementData, context);
                    context.end();
                    item.elementData.setParent(item.data);
                    item.elementData!.changedKeys([]);
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

function CreateElifElement(element: Element, data: Data, name: string, attributes: AttributeMap, context: PageViewContext, children: PageView, basePath: string | undefined): void {

    let scope = context.scope();

    if (scope.ifblock !== undefined) {

        let evaluate = attributes["wx:elif"];

        if (evaluate === undefined) {
            return;
        }

        if (!(evaluate instanceof Evaluate)) {
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

function CreateElseElement(element: Element, data: Data, name: string, attributes: AttributeMap, context: PageViewContext, children: PageView, basePath: string | undefined): void {

    let scope = context.scope();

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

export function CreateElement(element: Element, data: Data, name: string, attributes: AttributeMap, context: PageViewContext, children: PageView, basePath: string | undefined): void {

    if (attributes["wx:for"] !== undefined) {
        CreateForElement("wx:for", element, data, name, attributes, context, children, basePath);
    } else if (attributes["wx:for-items"] !== undefined) {
        CreateForElement("wx:for-items", element, data, name, attributes, context, children, basePath);
    } else if (attributes["wx:if"] !== undefined) {
        CreateIfElement(element, data, name, attributes, context, children, basePath);
    } else if (attributes["wx:elif"] !== undefined) {
        CreateElifElement(element, data, name, attributes, context, children, basePath);
    } else if (attributes["wx:else"] !== undefined) {
        CreateElseElement(element, data, name, attributes, context, children, basePath);
    } else {
        let e = context.page.document.createElement(name);
        e.basePath = basePath;
        ElementSetAttributes(e, data, attributes);
        element.append(e);
        context.begin();
        children(e, data, context);
        context.end();
    }

}

export function CreateTElement(element: Element, data: Data, attributes: AttributeMap, func: (element: Element, data: Data, context: PageViewContext, basePath: string | undefined) => void, context: PageViewContext, basePath: string | undefined): void {

    let v = new Data();

    func(element, v, context, basePath);

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

export function CreateCElement(element: Element, data: Data, name: string, attributes: AttributeMap, children: PageView, context: PageViewContext, path: string | undefined): void {

    let basePath = path;

    if (basePath !== undefined) {
        let i = basePath.lastIndexOf("/");
        if (i >= 0) {
            basePath = basePath.substr(0, i);
        }
    }

    CreateElement(element, data, "component", attributes, context, (element: Element, data: Data, context: PageViewContext): void => {

        if (element instanceof ComponentElement) {
            element.componentName = name;
            element.postAddMessage(path);
            children(element, element.data, context);
        }

    }, basePath);
}

export function CreateEvaluate(evaluateScript: EvaluateScript, keys: string[][]): Evaluate {
    return new Evaluate(evaluateScript, keys);
}

interface TWindow {
    __basePath?: string
}

var page = new PageObject((window as TWindow).__basePath || '');

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
page.document.addElementClass("component", ComponentElement);

export function Page(view: PageView, styleSheet: StyleSheet, options: PageOptions): void {
    postMessage({ page: 'readying' });
    view(page.document.documentElement, page.data, new PageViewContext(page));
    page.data.changedKeys([]);
    once((): void => { postMessage({ page: 'ready' }); });
}

export function setData(data: PageData, componentId: number | undefined): void {

    if (componentId === undefined) {

        for (var key in data) {
            page.data.set([key], data[key]);
        }

        console.info("[DATA]", data);

    } else {

        let e = page.document.element(componentId);

        if (e !== undefined && e instanceof ComponentElement) {

            let v = e.data;

            for (var key in data) {
                v.set([key], data[key]);
            }

            console.info("[COMPONENT] [DATA]", data);
        }

    }


}

export function sendEvent(id: number, name: string, data: any): void {
    let element = page.document.element(id);
    if (element != undefined) {
        element.onEvent(name, data);
    }
}
