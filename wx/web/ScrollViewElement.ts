import { ViewElement  } from './ViewElement';
import { booleanValue } from './V';
import { Document as KKDocument } from './Document';

interface WebKitStyle {
    webkitOverflowScrolling?:string
}

export class ScrollViewElement extends ViewElement {

    constructor(document:KKDocument,name:string,id:number) {
        super(document,name,id);

        let element: ScrollViewElement = this;
        let delta = {
            x : 0,
            y : 0,
            deltaX: 0,
            deltaY: 0
        }
        this._view = this.createView();
        this._view.addEventListener("scroll", (event: Event): void => {
            delta.deltaX = element.scrollLeft - delta.x;
            delta.deltaY = element.scrollTop - delta.y;
            delta.x = element.scrollLeft;
            delta.y = element.scrollTop;
            element.doEvent(event, "scroll",{
                scrollLeft : element.scrollLeft,
                scrollTop : element.scrollTop,
                scrollWidth : element.scrollWidth,
                scrollHeight : element.scrollHeight,
                deltaX : delta.deltaX,
                deltaY : delta.deltaY 
            });
            event.stopPropagation();
        });
    }

    public get scrollLeft():number {
        return this._view.scrollLeft;
    }

    public get scrollTop():number {
        return this._view.scrollTop;
    }

    public get scrollWidth():number {
        return this._view.scrollWidth;
    }

    public get scrollHeight():number {
        return this._view.scrollHeight;
    }

    public set(key: string, value: string | undefined) {
        super.set(key, value);

        if (key == 'scroll-x') {
            if(booleanValue(value)) {
                this._view.style.overflowX = 'auto';
            } else {
                this._view.style.overflowX = 'hidden';
            }
        } else if (key == 'scroll-y') {
            if(booleanValue(value)) {
                this._view.style.overflowY = 'auto';
            } else {
                this._view.style.overflowY = 'hidden';
            }
        } 
    }
}
