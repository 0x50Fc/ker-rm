import { ViewElement } from './ViewElement';
import { Document as KKDocument } from './Document';
import { booleanValue } from './V';
import { Element, Each } from './Element';


interface ViewHTMLElement extends HTMLElement {
    checkedView?: HTMLElement
}


export class RadioGroupElement extends ViewElement {


    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);

    }

    protected findCheckbox(element: Element, cur:RadioElement): void {
        if (element instanceof RadioElement) {
            if(element != cur) {
                element.set("checked",undefined);
            }
        } else {

            let p = element.firstChild;

            while (p) {
                this.findCheckbox(p, cur);
                p = p.nextSibling;
            }
        }
    }

    public doChange(element:RadioElement): void {

        this.findCheckbox(this, element);

        this.doElementEvent("change", { value: element.get("value") });

    }

    public getValue(): any {
        let v: any;
        Each(this,(element:Element):boolean=>{
            if(element instanceof RadioElement) {
                if(booleanValue(element.get("checked"))) {
                    v = element.get("value");
                }
                return false;
            }
            return v === undefined;
        });
        return v;
    }

    public setValue(value: any): void {
        Each(this,(element:Element):boolean=>{
            if(element instanceof RadioElement) {
                if(element.get("value") == value) {
                    element.set("checked","true");
                } else {
                    element.set("checked",undefined);
                }
                return false;
            }
            return true;
        });
    }

}


export class RadioElement extends ViewElement {


    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);

    }

    protected createView(): HTMLElement {
        let v = super.createView() as ViewHTMLElement;
        v.checkedView = document.createElement('div');
        v.appendChild(v.checkedView!);
        return v;
    }

    public set(key: string, value: string | undefined) {
        super.set(key, value);

        if (key == 'value') {
            if (value === undefined) {
                this._view.removeAttribute("value");
            } else {
                this._view.setAttribute("value", value);
            }
        } else if (key == 'disabled') {
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
        } else if (key == 'color') {
            (this._view as ViewHTMLElement).checkedView!.style.backgroundColor = value || '';
        }
    }

    public doEvent(event: Event, name: string, detail: any): void {
        super.doEvent(event, name, detail);

        if (name == 'tap') {
            if (!this._view.getAttribute("disabled")) {

                this.set("checked","true");

                let p = this.parent;

                while (p) {

                    if (p instanceof RadioGroupElement) {
                        p.doChange(this);
                        break;
                    }
                    p = p.parent;
                }

                event.stopPropagation();
            }
        }
    }

}
