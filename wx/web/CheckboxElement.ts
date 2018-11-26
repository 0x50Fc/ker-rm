import { ViewElement } from './ViewElement';
import { Document as KKDocument } from './Document';
import { booleanValue } from './V';
import { Element, Each } from './Element';

interface ViewHTMLElement extends HTMLElement {
    checkedView?: HTMLElement
}

interface ValueSet {
    [key:string]:boolean
}

export class CheckboxGroupElement extends ViewElement {


    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);

    }


    public doChange(): void {
        this.doElementEvent("change", { value: this.getValue() });
    }

    public getValue(): any {
        let vs: string[] = [];
        Each(this,(element:Element):boolean=>{
            if(element instanceof CheckboxElement) {
                if(booleanValue(element.get("checked"))) {
                    let v = element.get("value");
                    if (v !== undefined) {
                        vs.push(v);
                    }
                }
                return false;
            }
            return true;
        });
        return vs;
    }

    public setValue(value: any): void {
        let vs:ValueSet = {};
        if(value instanceof Array) {
            for(let v of value) {
                vs[v] = true;
            }
        }
        Each(this,(element:Element):boolean=>{
            if(element instanceof CheckboxElement) {
                let v = element.get("value");
                if(v) {
                    if(vs[v]) {
                        element.set("checked","true");
                    } else {
                        element.set("checked",undefined);
                    }
                }
                return false;
            }
            return true;
        });
    }

}

export class CheckboxElement extends ViewElement {


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

                if(booleanValue(this.get("checked"))) {
                    this.set("checked",undefined);
                } else {
                    this.set("checked","true");
                }

                let p = this.parent;

                while (p) {

                    if (p instanceof CheckboxGroupElement) {
                        p.doChange();
                        break;
                    }
                    p = p.parent;
                }

                event.stopPropagation();
            }
        }
    }

}
