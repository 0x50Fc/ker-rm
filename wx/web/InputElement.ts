import { NViewElement } from './NViewElement';
import { postMessage } from './IPC';

export class InputElement extends NViewElement {

    protected _value: string = "";
    protected _placeholderView: HTMLElement | undefined;

    protected createView(): HTMLElement {
        var v = super.createView();
        this._placeholderView = document.createElement('span');
        v.appendChild(this._placeholderView);
        postMessage({
            view: 'on',
            id: this._id,
            name: 'change'
        });
        return v;
    }

    public display(): void {
        super.display();

        let s = window.getComputedStyle(this._view);

        postMessage({
            view: 'set',
            id: this._id,
            name: 'padding',
            value: [s.paddingTop, s.paddingRight, s.paddingBottom, s.paddingLeft].join(' '),
        });

    }

    protected hasNativeKey(key: string): boolean {
        if (key == "placeholder" || key.startsWith("placeholder-")) {
            return false;
        }
        return super.hasNativeKey(key);
    }

    public set(key: string, value: string | undefined) {
        super.set(key, value);
        if (key == 'placeholder') {
            this._placeholderView!.textContent = value as string;
        } else if (key == 'placeholder-style') {
            this._placeholderView!.setAttribute("style", value as string);
        } else if (key == 'placeholder-class') {
            this._placeholderView!.className = value as string;
        } else if (key == 'value') {
            this.value = value || '';
        }
    }


    public set value(value: string) {
        this._value = value;
        if (this._value) {
            this._placeholderView!.style.visibility = 'hidden';
        } else {
            this._placeholderView!.style.visibility = 'visible';
        }
    }

    public get value(): string {
        return this._value;
    }

    public onEvent(name: string, data: any): void {
        if (name == "change") {
            this.value = data.value || '';
        }
    }

    public getValue(): any {
        return this.value;
    }

    public setValue(value: any): void {
        this.value = value;
        postMessage({
            view: 'set',
            id: this._id,
            name: 'value',
            value: value
        });
    }

}
