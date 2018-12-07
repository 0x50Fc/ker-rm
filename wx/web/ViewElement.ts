import { Element as KKElement, ElementEvent as ElementEvent } from './Element';
import { Document as KKDocument } from './Document';
import { BlockElement } from './BlockElement';
import { parseStyleValue } from './V';
import { ComponentElement } from './ComponentElement';

interface Touch {
    identifier: number
    pageX: number
    pageY: number
    clientX: number
    clientY: number
}

interface DataSet {
    [key: string]: any
}

interface DataSetElement extends Element {
    __kk_dataSet?: DataSet
}

function getDataSet(element: Element): DataSet | undefined {
    let v = (element as DataSetElement);
    if(v.__kk_dataSet === undefined) {
        if(element.parentElement !== undefined) {
            return getDataSet(element.parentElement!);
        }
    } else {
        return v.__kk_dataSet;
    }
    return undefined;
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

    protected _hoverStartTime: number;
    protected _hoverStayTime: number;
    protected _hoverTimeoutId: any;
    protected _hoverStopPropagation: boolean;
    protected _hover: boolean;
    protected _view: HTMLElement;


    public get view(): HTMLElement {
        return this._view;
    }

    public get contentView(): HTMLElement {
        return this._view;
    }

    protected createView(): HTMLElement {
        return document.createElement("wx-" + this._name);
    }

    protected setHover(hover: boolean): void {
        if (this._hoverTimeoutId) {
            clearTimeout(this._hoverTimeoutId);
            this._hoverTimeoutId = undefined;
        }
        if (hover) {
            var v = this.get("hover-class");
            if (v && v != 'none') {
                this._view.className = this.resolveClassName((this.get("class") || '') + ' ' + v);
                return;
            }
        }
        this._view.className = this.resolveClassName(this.get("class"));
        this._hover = hover;
    }

    public doElementEvent(name: string, detail: any): void {

        let e = new ElementEvent(this);
        let target = {
            id: this._view.id,
            dataset: getDataSet(this._view)
        };
        e.data = {
            type: name,
            target: target,
            currentTarget: target,
            detail: detail
        };
        this.emit(name, e);
    }

    public doEvent(event: Event, name: string, detail: any): void {
        let e = new ElementEvent(this);
        e.data = {
            type: name,
            timeStamp: event.timeStamp,
            target: event.target ? {
                id: (event.target as Element).id,
                dataset: getDataSet(event.target as HTMLElement)
            } : undefined,
            currentTarget: event.currentTarget ? {
                id: (event.currentTarget as Element).id,
                dataset: getDataSet(event.currentTarget as HTMLElement)
            } : undefined,
            detail: detail,
            touches: event instanceof TouchEvent ? getTouches(event.touches) : undefined,
            changedTouches: event instanceof TouchEvent ? getTouches(event.changedTouches) : undefined,
        };
        this.emit(name, e);
        if (e.cancelBubble) {
            event.stopPropagation();
        }
        if (name == "touchstart") {
            if (this._hoverTimeoutId) {
                clearTimeout(this._hoverTimeoutId);
            }
            let v = this;
            this._hoverTimeoutId = setTimeout(function () {
                v.setHover(true);
            }, this._hoverStartTime);
        } else if (name == "touchend" || name == "touchcancel") {
            if (this._hoverTimeoutId) {
                clearTimeout(this._hoverTimeoutId);
            }
            let v = this;
            this._hoverTimeoutId = setTimeout(function () {
                v.setHover(false);
            }, this._hoverStayTime);
        }
    }

    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);

        this._hoverStartTime = 50;
        this._hoverStayTime = 400;
        this._hoverStopPropagation = false;
        this._hover = false;

        let timeStamp: number = 0;
        let element: ViewElement = this;

        this._view = this.createView();
        (this._view as DataSetElement).__kk_dataSet = {};
        this._view.addEventListener("touchstart", (event: Event): void => {
            timeStamp = event.timeStamp;
            element.doEvent(event, "touchstart", {});
        });
        this._view.addEventListener("touchmove", (event: Event): void => {
            element.doEvent(event, "touchmove", {});
        });
        this._view.addEventListener("touchcancel", (event: Event): void => {
            element.doEvent(event, "touchcancel", {});
        });
        this._view.addEventListener("touchend", (event: Event): void => {
            element.doEvent(event, "touchend", {});
        });
        this._view.addEventListener("touchforcechange", (event: Event): void => {
            element.doEvent(event, "touchforcechange", {});
        });
        this._view.addEventListener("mouseup", (event: Event): void => {
            if (event.timeStamp - timeStamp > 350) {
                if (element.has("longpress")) {
                    element.doEvent(event, "longpress", {});
                    return;
                }
                element.doEvent(event, "longtap", {});
            }
            element.doEvent(event, "tap", {});
        });
    }

    public resolveClassName(v: string | undefined): string {
        return v || '';
    }

    public setData(key: string, value: any): void {
        let e = (this._view as DataSetElement);
        let dataSet = e.__kk_dataSet;
        if (dataSet === undefined) {
            e.__kk_dataSet = dataSet = {};
        }
        if (value === undefined) {
            delete dataSet[key];
        } else {
            dataSet[key] = value;
        }
    }

    public set(key: string, value: string | undefined) {
        super.set(key, value);

        if (key == 'class') {
            this._view.className = this.resolveClassName(value);
        } else if (key == 'style') {
            if (value === undefined) {
                this._view.removeAttribute(key);
            } else {
                this._view.setAttribute('style', parseStyleValue(value));
            }
        } else if (key == '#text') {
            this._view.innerText = value === undefined ? '' : (value as string).replace('\\n', '\n');
        } else if (key == 'id') {
            if (value === undefined) {
                this._view.removeAttribute("id");
            } else {
                this._view.setAttribute("id", value);
            }
        } else if (key == 'hover-stop-propagation') {
            this._hoverStopPropagation = value == 'true';
        } else if (key == 'hover-start-time') {
            this._hoverStartTime = parseInt(value || '0');
        } else if (key == 'hover-stay-time') {
            this._hoverStayTime = parseInt(value || '0');
        }
    }

    protected onDidAddToParent(element: KKElement): void {
        super.onDidAddToParent(element);
        if (element instanceof ViewElement) {
            element.contentView.appendChild(this._view);
        } else if (element instanceof BlockElement) {
            if (element.parent instanceof ViewElement) {
                element.parent.contentView.appendChild(this._view);
            }
        } else {
            document.body.appendChild(this._view);
        }
    }

    protected onDidAddChildren(element: KKElement): void {
        super.onDidAddChildren(element);
        if (element instanceof BlockElement) {
            let p = element.firstChild;
            while (p) {
                if (p instanceof ViewElement) {
                    this.contentView.appendChild(p.view);
                }
                p = p.nextSibling;
            }
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
