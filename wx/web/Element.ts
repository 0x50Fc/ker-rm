import { Event } from './Event';
import { EventEmitter } from './EventEmitter';
import { int } from './declare';
import { Document } from './Document';

export class ElementEvent extends Event {
    constructor(element?: Element) {
        super();
        this.element = element;
    }
    public cancelBubble: boolean = false;
    public element: Element | undefined;
    public data: any;
}

export interface AttributeMap {
    [key: string]: string
}

export class Element extends EventEmitter {

    protected _name: string;
    protected _id: number;
    protected _document: Document;

    private _firstChild: Element | undefined;
    private _lastChild: Element | undefined;
    private _nextSibling: Element | undefined;
    private _prevSibling: Element | undefined;
    private _parent: Element | undefined;
    private _attributes: AttributeMap = {};
    private _levelId: int = 0;
    private _depth: int = 0;
    private _autoLevelId: int = 0;
    private _basePath: string | undefined;

    public get basePath(): string | undefined {
        if (this._basePath === undefined) {
            if (this._parent !== undefined) {
                return this._parent.basePath;
            }
            if (this._document !== undefined) {
                return this._document.basePath;
            }
        }
        return this._basePath;
    }

    public set basePath(v: string | undefined) {
        this._basePath = v;
    }

    constructor(document: Document, name: string, id: number) {
        super();
        this._name = name;
        this._id = id;
        this._document = document;
    }

    public setData(key: string, value: any): void {

    }

    public get document(): Document {
        return this._document;
    }

    public get id(): number {
        return this._id;
    }

    public get name(): string {
        return this._name;
    }
    public get levelId(): int {
        return this._levelId;
    }

    public get depth(): int {
        return this._depth;
    }

    public get firstChild(): Element | undefined {
        return this._firstChild;
    }

    public get lastChild(): Element | undefined {
        return this._lastChild;
    }

    public get nextSibling(): Element | undefined {
        return this._nextSibling;
    }

    public get prevSibling(): Element | undefined {
        return this._prevSibling;
    }

    public get parent(): Element | undefined {
        return this._parent;
    }

    protected onDidAddToParent(element: Element): void {
    }

    protected onDidAddChildren(element: Element): void {
        let e: Element = element;
        e._depth = this._depth + 1;
        e._levelId = this._autoLevelId + 1;
        this._autoLevelId = e._levelId;
        e.onDidAddToParent(this);
    }

    public append(element: Element): void {

        var e: Element = element;

        e.remove();

        if (this._lastChild !== undefined) {
            this._lastChild._nextSibling = e;
            e._prevSibling = this._lastChild;
            this._lastChild = e;
            e._parent = this;
        } else {
            this._firstChild = e;
            this._lastChild = e;
            e._parent = this;
        }

        this.onDidAddChildren(element);
    }

    before(element: Element): void {

        var e: Element = element;

        e.remove();

        if (this._prevSibling !== undefined) {
            this._prevSibling._nextSibling = e;
            e._prevSibling = this._prevSibling;
            e._nextSibling = this;
            e._parent = this._parent;
            this._prevSibling = e;
        } else if (this._parent) {
            e._nextSibling = this;
            e._parent = this._parent;
            this._prevSibling = e;
            this._parent._firstChild = e;
        }

        if (this._parent !== undefined) {

            this._parent.onDidAddChildren(element);

        }
    }

    after(element: Element): void {

        var e: Element = element;

        e.remove();

        if (this._nextSibling !== undefined) {
            this._nextSibling._prevSibling = e;
            e._nextSibling = this._nextSibling;
            e._prevSibling = this;
            e._parent = this._parent;
            this._nextSibling = e;
        } else if (this._parent) {
            e._prevSibling = this;
            e._parent = this._parent;
            this._nextSibling = e;
            this._parent._lastChild = e;
        }

        if (this._parent !== undefined) {

            this._parent.onDidAddChildren(element);

        }

    }

    protected onWillRemoveFromParent(element: Element): void {
    }

    protected onWillRemoveChildren(element: Element): void {

        let e: Element = element;
        e._depth = 0;
        e._levelId = 0;

        e.onWillRemoveFromParent(this);
    }

    public remove(): void {

        if (this._prevSibling !== undefined && this._parent !== undefined) {
            this._parent.onWillRemoveChildren(this);
            this._prevSibling._nextSibling = this._nextSibling;
            if (this._nextSibling !== undefined) {
                this._nextSibling._prevSibling = this._prevSibling;
            } else {
                this._parent._lastChild = this._prevSibling;
            }
        } else if (this._parent !== undefined) {
            this._parent.onWillRemoveChildren(this);
            this._parent._firstChild = this._nextSibling;
            if (this._nextSibling) {
                this._nextSibling._prevSibling = undefined;
            } else {
                this._parent._lastChild = undefined;
            }
        }

        if (this._parent) {

            this._parent = undefined;
            this._prevSibling = undefined;
            this._nextSibling = undefined;

        }

    }

    public appendTo(element: Element): void {
        element.append(this);
    }

    public beforeTo(element: Element): void {
        element.before(this);
    }

    public afterTo(element: Element): void {
        element.after(this);
    }

    public get(key: string): string | undefined {
        return this._attributes[key];
    }

    public set(key: string, value: string | undefined) {
        if (value === undefined) {
            delete this._attributes[key];
        } else {
            this._attributes[key] = value;
        }
    }

    public get attributes(): AttributeMap {
        return this._attributes;
    }

    public emitBubble(name: string, event: Event): void {
        if (event instanceof ElementEvent) {
            var e = event as ElementEvent;
            if (e.element === undefined) {
                e.element = this;
            }
        }
        this.emit(name, event);
        if (event instanceof ElementEvent) {
            var e = event as ElementEvent;
            if (!e.cancelBubble) {
                if (this._parent !== undefined) {
                    this._parent.emit(name, event);
                } else if (this._document !== undefined) {
                    this._document.emit(name, event);
                }
            }
        }
    }

    public hasBubble(name: string): boolean {
        if (super.has(name)) {
            return true;
        }
        if (this._parent !== undefined) {
            return this._parent.hasBubble(name);
        }
        return false;
    }

    public recycle(): void {

        this._document.removeElement(this._id);

        var p = this._firstChild;

        while (p !== undefined) {
            let n = p._nextSibling;
            p.recycle();
            p = n;
        }

        this._parent = undefined;
        this._firstChild = undefined;
        this._lastChild = undefined;
        this._prevSibling = undefined;
        this._nextSibling = undefined;

    }

    public resolveURI(uri: string): string {
        if (uri.indexOf("://") > 0) {
            return uri;
        }
        var basePath = this.basePath!;
        if (basePath.endsWith("/")) {
            return basePath + uri;
        } else if(basePath) {
            return basePath + '/' + uri;
        }
        return uri;
    }

    public onEvent(name: string, data: any): void {

    }

}

export function Each(element: Element, func: (element: Element) => boolean): void {

    if (func(element)) {

        let p = element.firstChild;

        while (p) {

            Each(p, func);

            p = p.nextSibling;
        }

    }

}
