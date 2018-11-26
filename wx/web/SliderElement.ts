import { ViewElement } from './ViewElement';
import { booleanValue, pixelStringValue } from './V';
import { Document as KKDocument } from './Document';
import { once } from './once';

interface ViewHTMLElement extends HTMLElement {
    progressView?: HTMLElement
    progressBar?: HTMLElement
    infoView?: HTMLElement
    curView?: HTMLElement
}

export class SliderElement extends ViewElement {

    protected _min: number;
    protected _max: number;
    protected _step: number;
    protected _value: number;

    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);
        this._min = 0;
        this._max = 100;
        this._step = 1;
        this._value = 0;
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
        v.curView = document.createElement('div');
        v.curView!.className = 'cur';
        v.progressView!.appendChild(v.curView!);
        return v;
    }

    protected updatePercentView(): void {
        let v = (this._value - this._min) * 100 / (this._max - this._min);
        (this._view as ViewHTMLElement).progressBar!.style.width = v + '%';
        (this._view as ViewHTMLElement).infoView!.innerText = this._value + '';
        (this._view as ViewHTMLElement).curView!.style.left = v + '%';
        this._updatePercentViewing = false;
    }

    protected _updatePercentViewing: boolean = false;

    protected setUpdatePercentView(): void {

        if (this._updatePercentViewing) {
            return;
        }

        this._updatePercentViewing = true;

        let v = this;

        once((): void => {
            v.updatePercentView();
        });
    }

    public set(key: string, value: string | undefined) {
        super.set(key, value);

        if (key == 'value') {
            this._value = parseFloat(value || '0');
            this.setUpdatePercentView();
        } else if (key == 'min') {
            this._min = parseFloat(value || '0');
            this.setUpdatePercentView();
        } else if (key == 'max') {
            this._max = parseFloat(value || '0');
            this.setUpdatePercentView();
        } else if (key == 'step') {
            this._step = parseFloat(value || '0');
            this.setUpdatePercentView();
        } else if (key == 'show-value') {
            if (booleanValue(value)) {
                (this._view as ViewHTMLElement).infoView!.style.display = 'block';
                this.setUpdatePercentView();
            } else {
                (this._view as ViewHTMLElement).infoView!.style.display = 'none';
            }
        } else if (key == 'block-color') {
            (this._view as ViewHTMLElement).curView!.style.backgroundColor = value!;
        } else if (key == 'block-size') {
            let v = parseFloat(value || '28');
            (this._view as ViewHTMLElement).curView!.style.width = v + 'px';
            (this._view as ViewHTMLElement).curView!.style.height = v + 'px';
            (this._view as ViewHTMLElement).curView!.style.borderRadius = (v * 0.5) + 'px';
            (this._view as ViewHTMLElement).curView!.style.marginLeft = - (v * 0.5) + 'px';
            (this._view as ViewHTMLElement).curView!.style.marginTop = - (v * 0.5) + 'px';
        } else if (key == 'selected-color' || key == 'activeColor') {
            (this._view as ViewHTMLElement).progressBar!.style.backgroundColor = value!;
        } else if (key == 'color' || key == 'backgroundColor') {
            (this._view as ViewHTMLElement).progressView!.style.backgroundColor = value!;
        } else if (key == 'backgroundColor') {
            (this._view as ViewHTMLElement).style.backgroundColor = value!;
        }
    }

    protected _touchPageX: number = 0;
    protected _touchCurX: number = 0;
    protected _changing: boolean = false;

    public doEvent(event: Event, name: string, detail: any): void {
        super.doEvent(event, name, detail);

        if (name == "touchstart") {

            let width = (this._view as ViewHTMLElement).progressView!.clientWidth;
            let x = (event as TouchEvent).touches[0].pageX;
            let curX = (this._value - this._min) * width / (this._max - this._min);

            if (Math.abs(x - curX) <= (this._view as ViewHTMLElement).progressView!.clientWidth * 0.5) {
                this._changing = true;
                this._touchPageX = x;
                this._touchCurX = curX;
            }

        } else if (name == "touchmove" && this._changing) {

            let width = (this._view as ViewHTMLElement).progressView!.clientWidth;
            let x = (event as TouchEvent).touches[0].pageX;
            var curX = this._touchCurX + (x - this._touchPageX);

            if (curX < 0) {
                curX = 0;
            }

            if (curX > width) {
                curX = width;
            }

            let v = (curX * (this._max - this._min)) / width;

            v = Math.floor(v / this._step) * this._step + this._min;

            if (this._value != v) {
                this._value = v;
                this.updatePercentView();
                this.doElementEvent("changing", { value: v });
            }

        } else if ((name == "touchend" || name == "touchcancel") && this._changing) {

            this.doElementEvent("change", { value: this._value });

            this._changing = false;
        }
    }

    public getValue(): any {
        return this.get("value") || 0;
    }

    public setValue(value: any): void {
        this.set("value",value || 0);
    }

}
