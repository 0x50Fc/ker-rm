import { ViewElement } from './ViewElement';
import { booleanValue, pixelStringValue } from './V';
import { Document as KKDocument } from './Document';

interface ViewHTMLElement extends HTMLElement {
    progressView?: HTMLElement
    progressBar?: HTMLElement
    infoView?: HTMLElement
}

export class ProgressElement extends ViewElement {

    protected _percent:number;

    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);
        this._percent = 0;
    }

    protected createView(): HTMLElement {
        let v = super.createView() as ViewHTMLElement;
        v.progressView = document.createElement('div');
        v.progressView!.className = 'bar';
        v.appendChild(v.progressView!);
        v.progressBar = document.createElement('div');
        v.progressView!.appendChild(v.progressBar!);
        v.infoView = document.createElement('div');
        v.infoView!.className = 'info';
        v.appendChild(v.infoView!);
        return v;
    }

    protected updatePercentView():void {
        (this._view as ViewHTMLElement).progressBar!.style.width = this._percent + '%';
        (this._view as ViewHTMLElement).infoView!.innerText = this._percent + '%';
    }

    public set(key: string, value: string | undefined) {
        super.set(key, value);

        if (key == 'percent') {
            this._percent = parseFloat(value || '0');
            this.updatePercentView();
        } else if (key == 'show-info') {
            if(booleanValue(value)) {
                (this._view as ViewHTMLElement).infoView!.style.display = 'block';
            } else {
                (this._view as ViewHTMLElement).infoView!.style.display = 'none';
            }
        } else if (key == 'border-radius') {
            this._view.style.borderRadius = pixelStringValue(value);
        } else if (key == 'font-size') {
            (this._view as ViewHTMLElement).infoView!.style.fontSize = pixelStringValue(value);
        } else if (key == 'stroke-width') {
            (this._view as ViewHTMLElement).progressBar!.style.height = pixelStringValue(value);
        } else if (key == 'color' || key == 'activeColor') {
            (this._view as ViewHTMLElement).progressBar!.style.backgroundColor = value!;
        } else if (key == 'backgroundColor') {
            (this._view as ViewHTMLElement).style.backgroundColor = value!;
        }
    }
}
