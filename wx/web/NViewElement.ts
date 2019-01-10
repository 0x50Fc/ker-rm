import { ViewElement } from './ViewElement';
import { Element as KKElement, Element } from './Element';
import { postMessage } from './IPC';
import { once } from './once';
import { Document as KKDocument } from './Document';
import { nativePixelValue } from './V';

var _elements: ElementMap = {};

function add(element: NViewElement): void {
    _elements[element.id] = element;
}

function remove(element: NViewElement): void {
    delete _elements[element.id];
}

setInterval(function () {

    for(var id in _elements) {
        let e = _elements[id];
        if(e.needsDisplay) {
            e.display();
        }
    }

}, 1000 / 30);

interface ElementMap {
    [key: number]: NViewElement
}

interface Frame {
    x: number;
    y: number;
    width: number;
    height: number;
}

export class NViewElement extends ViewElement {

    protected createView(): HTMLElement {
        var v = document.createElement("wx-" + this._name);
        postMessage({
            view: 'create',
            id: this._id,
            name: this._name
        });
        return v;
    }

    protected _displaying: boolean = false;
    protected _displayFrame: Frame = { x: 0, y: 0, width: 0, height: 0 };

    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);
        add(this);
    }

    public get displaying(): boolean {
        return this._displaying;
    }

    public get needsDisplay(): boolean {
        
        if (this._displaying) {
            return false;
        }

        var p: HTMLElement | undefined = this._view as HTMLElement;
        var x: number = 0;
        var y: number = 0;

        while (p !== undefined && p != document.body) {

            x += p.offsetLeft;
            y += p.offsetTop;

            p = p.offsetParent as (HTMLElement | undefined);
        }

        return (x != this._displayFrame.x || y != this._displayFrame.y 
            || this._view.clientWidth != this._displayFrame.width
            || this._view.clientHeight != this._displayFrame.height);
    }

    public display(): void {

        var p: HTMLElement | undefined = this._view as HTMLElement;
        var x: number = 0;
        var y: number = 0;

        while (p !== undefined && p != document.body) {

            x += p.offsetLeft;
            y += p.offsetTop;

            p = p.offsetParent as (HTMLElement | undefined);
        }

        this._displayFrame.x = x;
        this._displayFrame.y = y;
        this._displayFrame.width = this._view.clientWidth;
        this._displayFrame.height = this._view.clientHeight;

        postMessage({
            view: 'setFrame',
            id: this._id,
            x: nativePixelValue(x),
            y: nativePixelValue(y),
            width: nativePixelValue(this._displayFrame.width),
            height: nativePixelValue(this._displayFrame.height)
        });

        this._displaying = false;
    }

    public setNeedsDisplay(): void {
        if (this._displaying) {
            return;
        }
        this._displaying = true;
        var v = this;
        once(function () {
            v.display();
        });
    }

    protected hasNativeKey(key: string): boolean {
        return true;
    }

    public set(key: string, value: string | undefined) {
        super.set(key, value);
        if (this.hasNativeKey(key)) {
            postMessage({
                view: 'set',
                id: this._id,
                name: key,
                value: value
            });
        }
        this.setNeedsDisplay();
    }

    protected onDidAddToParent(element: KKElement): void {
        super.onDidAddToParent(element);

        var pid: number | undefined = undefined;

        if (element instanceof NViewElement) {
            pid = element._id;
        }

        postMessage({
            view: 'add',
            id: this._id,
            pid: pid
        });

        this.setNeedsDisplay();
    }

    protected onWillRemoveFromParent(element: KKElement): void {
        super.onWillRemoveFromParent(element);

        postMessage({
            view: 'remove',
            id: this._id
        });
    }

    public recycle(): void {

        remove(this);

        postMessage({
            view: 'remove',
            id: this._id
        });

        super.recycle();

    }


}
