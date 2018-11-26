import { ViewElement } from './ViewElement';
import { Element } from './Element';
import { Document as KKDocument } from './Document';
import { SwitchElement } from './SwitchElement';
import { InputElement } from './InputElement';
import { CheckboxGroupElement } from './CheckboxElement';
import { SliderElement } from './SliderElement';
import { RadioGroupElement } from './RadioElement';
import { PickerElement } from './PickerElement';
import { once } from './once';

interface FormValueMap {
    [key: string]: any
}

interface ElementValue {
    getValue(): any
    setValue(value: any): void
}

function Each(element: Element, func: (name: string, element: ElementValue) => void): void {

    if (element instanceof SwitchElement
        || element instanceof InputElement
        || element instanceof CheckboxGroupElement
        || element instanceof SliderElement
        || element instanceof RadioGroupElement
        || element instanceof PickerElement) {

        let name = element.get("name");

        if (name !== undefined) {
            let v: ElementValue = element as ElementValue;
            func(name, v);
        }
    } else {
        let p = element.firstChild;
        while (p) {
            Each(p, func);
            p = p.nextSibling;
        }
    }
}

export class FormElement extends ViewElement {

    protected _formValues?: FormValueMap;

    public doAction(action: string): void {

        switch (action) {
            case 'submit':
                this.doSubmit();
                break;
            case 'reset':
                this.doReset();
                break;
        }
    }

    protected defaultValues(): void {

        if (this._formValues === undefined) {
            this._formValues = {};
            this.getValues(this._formValues, this);
        }
    }

    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);

        let v = this;
        once((): void => {
            v.defaultValues();
        });

    }

    public doSubmit(): void {

        let values: FormValueMap = {};

        Each(this, (name: string, element: ElementValue): void => {
            values[name] = element.getValue();
        });

        this.doElementEvent("submit", { value: values, formId: this.get("id") });
    }


    protected getValues(values: FormValueMap, element: Element): void {

        Each(this, (name: string, element: ElementValue): void => {
            values[name] = element.getValue();
        });
    }

    protected setValues(values: FormValueMap, element: Element): void {

        Each(this, (name: string, element: ElementValue): void => {
            element.setValue(values[name]);
        });

    }

    public doReset(): void {

        if (this._formValues !== undefined) {
            this.setValues(this._formValues, this);
        }

        this.doElementEvent("reset", {});
    }

}
