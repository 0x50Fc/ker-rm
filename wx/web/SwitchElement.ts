import { ViewElement } from './ViewElement';
import { Document as KKDocument } from './Document';
import { booleanValue } from './V';
import { Element } from './Element';

interface ViewHTMLElement extends HTMLElement {
    checkedView?: HTMLElement
}

export class SwitchElement extends ViewElement {

    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);

    }

    protected createView(): HTMLElement {
        let v = super.createView() as ViewHTMLElement;
        v.checkedView = document.createElement('div');
        v.checkedView.className = "cur";
        v.appendChild(v.checkedView!);
        return v;
    }

    public set(key: string, value: string | undefined) {
        super.set(key, value);

        if (key == 'disabled') {
            if (booleanValue(value)) {
                this._view.setAttribute("disabled", "disabled");
            } else {
                this._view.removeAttribute("disabled");
            }
        } else if (key == 'checked') {
            if (booleanValue(value)) {
                this._view.setAttribute("checked", "checked");
            } else {
                this._view.removeAttribute("checked");
            }
        }
    }

    public doEvent(event: Event, name: string, detail: any): void {
        super.doEvent(event, name, detail);

        if (name == 'tap') {
            if (!this._view.getAttribute("disabled")) {

                let v = booleanValue(this.get("checked"));

                if (v) {
                    this.set("checked", undefined);
                    v = false;
                } else {
                    this.set("checked", "true");
                    v = true;
                }

                this.doElementEvent("change", { value: v });

                event.stopPropagation();
            }
        }
    }

    public getValue():any {
        return booleanValue(this.get("checked"));
    }

    public setValue(value:any):void {
        if(booleanValue(value as string|undefined)) {
            this.set("checked","true");
        } else {
            this.set("checked",undefined);
        }
    }
}
