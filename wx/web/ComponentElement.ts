import { BlockElement } from './BlockElement';
import { Document } from './Document';
import { Data } from './Data';
import { ElementEvent, Element } from './Element';
import { postMessage } from './IPC';

export class ComponentElement extends BlockElement {

    private _data: Data = new Data();
    private _componentName: string | undefined;

    public get data(): Data {
        return this._data;
    }

    public get componentName(): string | undefined {
        return this._componentName;
    }

    public set componentName(v: string | undefined) {
        this._componentName = v;
    }

    constructor(document: Document, name: string, id: number) {
        super(document, name, id);

    }

    public postAddMessage(path: string | undefined) {
        postMessage({
            component: 'add',
            path: path,
            id: this._id
        });
    }

    protected onWillRemoveFromParent(element: Element): void {
        super.onWillRemoveFromParent(element);
        postMessage({
            component: 'remove',
            id: this._id
        });
    }

    public set(key: string, value: string | undefined) {
        super.set(key, value);
        postMessage({
            component: 'set',
            id: this._id,
            name: key,
            value: value
        });
    }

    public emit(name: string, event: Event): void {
        super.emit(name, event);
        if (event instanceof ElementEvent) {
            event.cancelBubble = true;
        }
    }

    public onEvent(name: string, data: any): void {

        let event = new ElementEvent(this);
        event.data = data;

        super.emit(name, event);

        if (!event.cancelBubble) {
            if (this.parent) {
                this.parent.emitBubble(name, event);
            } else if (this.document) {
                this.document.emit(name, event);
            }
        }
    }

}
