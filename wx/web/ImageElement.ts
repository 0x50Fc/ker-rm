import { ViewElement } from './ViewElement';

export class ImageElement extends ViewElement {

    public get imageView(): HTMLElement {
        return this._view.firstElementChild as HTMLElement;
    }

    protected createView(): HTMLElement {
        var v = document.createElement("wx-" + this._name);
        var img = document.createElement("img");
        v.appendChild(img);
        return v;
    }

    public set(key: string, value: string | undefined) {
        super.set(key, value);

        if (key == 'src') {
            if (value === undefined) {
                this.imageView.removeAttribute("src");
            } else {
                this.imageView.setAttribute("src", this.resolveURI(value));
            }
        }
    }

}
