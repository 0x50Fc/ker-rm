import { Element as KKElement, ElementEvent as ElementEvent } from './Element';
import { Document as KKDocument } from './Document';

interface DataSet {
    [key: string]: string
}

function getDataSet(element: Element): DataSet {
    let v: DataSet = {};
    let length = element.attributes.length;
    for (var i = 0; i < length; i++) {
        var attr = element.attributes.item(i)!;
        if (attr.localName.startsWith("data-")) {
            v[attr.localName.substr(5)] = attr.value;
        }
    }
    return v;
}

interface Touch {
    identifier: number
    pageX: number
    pageY: number
    clientX: number
    clientY: number
}

function getTouches(touches: TouchList): Touch[] {
    let vs: Touch[] = [];
    let length = touches.length;
    for (var i = 0; i < length; i++) {
        let touch = touches.item(i)!;
        vs.push({
            identifier: touch.identifier,
            pageX: touch.pageX,
            pageY: touch.pageY,
            clientX: touch.clientX,
            clientY: touch.clientY
        });
    }
    return vs;
}

export class ViewElement extends KKElement {

    protected _view: HTMLElement;

    public get view(): HTMLElement {
        return this._view;
    }

    protected createView(): HTMLElement {
        return document.createElement("wx-" + this._name);
    }

    protected doEvent(event: Event, name: string, detail: any): void {
        let e = new ElementEvent(this);
        e.data = {
            type: name,
            timeStamp: event.timeStamp,
            target: event.target ? {
                id: (event.target as Element).id,
                dataset: getDataSet(event.target as Element)
            } : undefined,
            currentTarget: event.currentTarget ? {
                id: (event.currentTarget as Element).id,
                dataset: getDataSet(event.target as Element)
            } : undefined,
            detail: detail,
            touches: event instanceof TouchEvent ? getTouches(event.touches) : undefined,
            changedTouches: event instanceof TouchEvent ? getTouches(event.changedTouches) : undefined,
        };
        this.emit(name, e);
    }

    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);

        let timeStamp: number = 0;
        let element: ViewElement = this;

        this._view = this.createView();
        this._view.addEventListener("touchstart", (event: Event): void => {
            timeStamp = event.timeStamp;
            element.doEvent(event, "touchstart", {});
            event.stopPropagation();
        });
        this._view.addEventListener("touchmove", (event: Event): void => {
            element.doEvent(event, "touchmove", {});
            event.stopPropagation();
        });
        this._view.addEventListener("touchcancel", (event: Event): void => {
            element.doEvent(event, "touchcancel", {});
            event.stopPropagation();
        });
        this._view.addEventListener("touchend", (event: Event): void => {
            element.doEvent(event, "touchend", {});
            if (event.timeStamp - timeStamp > 350) {
                if (element.has("longpress")) {
                    element.doEvent(event, "longpress", {});
                    return;
                }
                element.doEvent(event, "longtap", {});
            }
            element.doEvent(event, "tap", {});
            event.stopPropagation();
        });
        this._view.addEventListener("touchforcechange", (event: Event): void => {
            element.doEvent(event, "touchforcechange", {});
            event.stopPropagation();
        });
    }

    public set(key: string, value: string | undefined) {
        super.set(key, value);

        if (key == 'class') {
            this._view.className = value === undefined ? '' : value as string;
        } else if (key == 'style') {
            if (value === undefined) {
                this._view.removeAttribute(key);
            } else {
                this._view.setAttribute('style', value as string);
            }
        } else if (key == '#text') {
            this._view.textContent = value === undefined ? '' : value as string;
        } else if (key == 'id') {
            if (value === undefined) {
                this._view.removeAttribute("id");
            } else {
                this._view.setAttribute("id", value);
            }
        } else if (key.startsWith("data-")) {
            if (value === undefined) {
                this._view.removeAttribute(key);
            } else {
                this._view.setAttribute(key, value);
            }

        }
    }

    protected onDidAddToParent(element: KKElement): void {
        super.onDidAddToParent(element);
        if (element instanceof ViewElement) {
            element.view.appendChild(this._view);
        } else {
            document.body.appendChild(this._view);
        }
    }

    protected onWillRemoveFromParent(element: KKElement): void {
        super.onWillRemoveFromParent(element);
        let p = this._view.parentElement;
        if (p) {
            p.removeChild(this._view);
        }
    }

    public recycle(): void {
        super.recycle();
    }

}
