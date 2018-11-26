import { ViewElement } from './ViewElement';
import { booleanValue, pixelValue } from './V';
import { Document as KKDocument } from './Document';
import { clearAnimation, startAnimation } from './Anim';

interface WebKitStyle {
    webkitOverflowScrolling?: string
}

export class ScrollViewElement extends ViewElement {

    protected _upperThreshold: number;
    protected _lowerThreshold: number;
    protected _enabledScrollX: boolean;
    protected _enabledScrollY: boolean;
    protected _thgresholdin: boolean;
    protected _scrollWithAnimation: boolean;

    public doEvent(event: Event, name: string, detail: any): void {
        super.doEvent(event, name, detail);


    }

    protected _scrollAnimateId:number = 0;

    protected scrollTo(x: number, y: number, animated: boolean): void {
        
        if(this._scrollAnimateId) {
            clearAnimation(this._scrollAnimateId);
            this._scrollAnimateId = 0;
        }

        if (animated) {
            let view = this._view;
            let x0 = view.scrollLeft;
            let y0 = view.scrollTop;
            let dx = x - x0;
            let dy = y - y0;
            this._scrollAnimateId = startAnimation(300, (v: number): void => {
                view.scrollTo(x0 + dx * v, y0 + dy * v);
            });
        } else {
            this._view.scrollTo(x, y);
        }
    }

    protected onScroll(x: number, y: number, event: Event): void {

        var sBottom = this._view.scrollHeight - this._view.clientHeight;
        var sRight = this._view.scrollWidth - this._view.clientWidth;

        if (this._enabledScrollY) {
            if (y < this._upperThreshold) {
                if (!this._thgresholdin) {
                    this._thgresholdin = true;
                    this.doEvent(event, "scrolltoupper", {});;
                }
            } else if (y > sBottom - this._lowerThreshold) {
                if (!this._thgresholdin) {
                    this._thgresholdin = true;
                    this.doEvent(event, "scrolltolower", {});;
                }
            } else {
                this._thgresholdin = false;
            }
        } else if (this._enabledScrollX) {
            if (x < this._upperThreshold) {
                if (!this._thgresholdin) {
                    this._thgresholdin = true;
                    this.doEvent(event, "scrolltoupper", {});;
                }
            } else if (x > sRight - this._lowerThreshold) {
                if (!this._thgresholdin) {
                    this._thgresholdin = true;
                    this.doEvent(event, "scrolltolower", {});;
                }
            } else {
                this._thgresholdin = false;
            }
        }
    }

    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);

        this._upperThreshold = 50;
        this._lowerThreshold = 50;
        this._enabledScrollX = false;
        this._enabledScrollY = false;
        this._thgresholdin = false;
        this._scrollWithAnimation = false;

        let element: ScrollViewElement = this;
        let delta = {
            x: 0,
            y: 0,
            deltaX: 0,
            deltaY: 0
        }

        this._view.addEventListener("scroll", (event: Event): void => {

            element.onScroll(element.scrollLeft, element.scrollTop, event);

            delta.deltaX = element.scrollLeft - delta.x;
            delta.deltaY = element.scrollTop - delta.y;
            delta.x = element.scrollLeft;
            delta.y = element.scrollTop;
            element.doEvent(event, "scroll", {
                scrollLeft: element.scrollLeft,
                scrollTop: element.scrollTop,
                scrollWidth: element.scrollWidth,
                scrollHeight: element.scrollHeight,
                deltaX: delta.deltaX,
                deltaY: delta.deltaY
            });
            event.stopPropagation();
        });
    }

    public get scrollLeft(): number {
        return this._view.scrollLeft;
    }

    public get scrollTop(): number {
        return this._view.scrollTop;
    }

    public get scrollWidth(): number {
        return this._view.scrollWidth;
    }

    public get scrollHeight(): number {
        return this._view.scrollHeight;
    }

    public set(key: string, value: string | undefined) {
        super.set(key, value);

        if (key == 'scroll-x') {
            if (booleanValue(value)) {
                this._view.style.overflowX = 'auto';
                this._enabledScrollX = true;
            } else {
                this._view.style.overflowX = 'hidden';
                this._enabledScrollX = false;
            }
        } else if (key == 'scroll-y') {
            if (booleanValue(value)) {
                this._view.style.overflowY = 'auto';
                this._enabledScrollY = true;
            } else {
                this._view.style.overflowY = 'hidden';
                this._enabledScrollY = false;
            }
        } else if (key == 'upper-threshold') {
            this._upperThreshold = pixelValue(value);
        } else if (key == 'lower-threshold') {
            this._lowerThreshold = pixelValue(value);
        } else if (key == 'scroll-into-view') {
            if (value) {
                var v = document.getElementById(value);
                if (v && v.parentElement == this._view) {
                    if (this._enabledScrollY) {
                        this.scrollTo(this._view.scrollLeft, v.offsetTop, this._scrollWithAnimation);
                    } else if (this._enabledScrollX) {
                        this.scrollTo(v.offsetLeft, this._view.scrollTop, this._scrollWithAnimation);
                    }
                }
            }

        } else if (key == 'scroll-top') {
            if (this._enabledScrollY) {
                this.scrollTo(this._view.scrollLeft, pixelValue(value), this._scrollWithAnimation);
            }
        } else if (key == 'scroll-left') {
            if (this._enabledScrollX) {
                this.scrollTo(pixelValue(value), this._view.scrollTop, this._scrollWithAnimation);
            }
        }
    }
}
