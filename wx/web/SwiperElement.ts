import { ViewElement } from './ViewElement';
import { Element as KKElement, ElementEvent } from './Element';
import { booleanValue } from './V';
import { once } from './once';
import { BlockElement } from './BlockElement';
import { Document as KKDocument } from './Document';
import { clearAnimation, startAnimation } from './Anim';

interface ViewHTMLElement extends HTMLElement {
    dotsView?: HTMLElement
    contentView?: HTMLElement
    scrollView?: HTMLElement
}

export class SwiperItemElement extends ViewElement {

}

export class SwiperElement extends ViewElement {

    protected _duration: number;
    protected _interval: number;
    protected _autoplay: boolean;

    public get contentView(): HTMLElement {
        return (this._view as ViewHTMLElement).contentView!;
    }

    public get width(): number {
        return this._view.clientWidth;
    }

    public get height(): number {
        return this._view.clientHeight;
    }

    protected _scrollAnimationId: number = 0;
    protected _index: number = 0;

    public scrollTo(x: number, y: number, animated: boolean): void {

        if (this._scrollAnimationId) {
            clearAnimation(this._scrollAnimationId);
            this._scrollAnimationId = 0;
        }

        if (animated) {
            let element = this;
            let view = this.scrollView;
            let x0 = view.scrollLeft;
            let y0 = view.scrollTop;
            let dx = x - x0;
            let dy = y - y0;
            this._scrollAnimationId = startAnimation(this._duration, (v: number): void => {
                view.scrollTo(x0 + dx * v, y0 + dy * v);
                if (v >= 1) {
                    element._scrollAnimationId = 0;
                }
            });
        } else {
            this._view.scrollTo(x, y);
        }

    }

    public scrollToPage(index: number, animated: boolean, source: string): void {

        if (this._vertical) {
            var b = this._view.clientHeight;
            this.scrollTo(this.scrollView.scrollLeft, (index * b), animated);
        } else {
            var b = this._view.clientWidth;
            this.scrollTo((index * b), this.scrollView.scrollTop, animated);
        }

        let view = this.dotsView;
        let items = view.children;
        let n = items.length;

        for (let i = 0; i < n; i++) {
            let item = items[i];
            if (i == index) {
                item.className = "wx-active";
            } else {
                item.className = "";
            }
        }

        if (this._index != index) {
            this._index = index;
            this.doElementEvent("change", { current: index, source: source });
        }

    }

    protected _touching: boolean = false;

    protected onTouchStart(event: TouchEvent): void {
        this._touching = true;
    }

    protected onTouchEnd(event: TouchEvent): void {

        this._touching = false;

        if (this._items.length == 0) {
            return;
        }

        if (this._vertical) {
            let index = Math.floor(this.scrollView.scrollTop / this._view.clientHeight);
            if (this._vScrollY > 5) {
                index++;
            }
            if (index < 0) {
                index = 0;
            }
            if (index > this._items.length) {
                index = this._items.length - 1;
            }
            this.scrollToPage(index, true, "touch");
        } else {
            let index = Math.floor(this.scrollView.scrollLeft / this._view.clientWidth);

            if (this._vScrollX > 5) {
                index++;
            }
            if (index < 0) {
                index = 0;
            }
            if (index > this._items.length) {
                index = this._items.length - 1;
            }
            this.scrollToPage(index, true, "touch");
        }

    }

    protected _preScrollX: number = 0;
    protected _preScrollY: number = 0;
    protected _vScrollX: number = 0;
    protected _vScrollY: number = 0;

    protected onScroll(event: Event): void {
        let x = this.scrollView.scrollLeft;
        let y = this.scrollView.scrollTop;
        if (Math.abs(x - this._preScrollX) > 5) {
            this._vScrollX = x - this._preScrollX;
            this._preScrollX = x;
        }
        if (Math.abs(y - this._preScrollY) > 5) {
            this._vScrollY = y - this._preScrollY;
            this._preScrollY = y;
        }
    }

    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);

        this._interval = 5000;
        this._duration = 500;
        this._autoplay = false;

        let element: SwiperElement = this;

        this.scrollView.addEventListener("scroll", (event: Event): void => {
            element.onScroll(event);
            event.stopPropagation();
        });

        this.scrollView.addEventListener("touchstart", (event: Event): void => {
            element.onTouchStart(event as TouchEvent);
        });

        this.scrollView.addEventListener("touchend", (event: Event): void => {
            element.onTouchEnd(event as TouchEvent);
            event.preventDefault();
            event.stopPropagation();
        });

        this.scrollView.addEventListener("touchcancel", (event: Event): void => {
            element.onTouchEnd(event as TouchEvent);
            event.preventDefault();
            event.stopPropagation();
        });
    }

    public get dotsView(): HTMLElement {
        return (this._view as ViewHTMLElement).dotsView!;
    }

    public get scrollView(): HTMLElement {
        return (this._view as ViewHTMLElement).scrollView!;
    }

    protected createView(): HTMLElement {
        var v = document.createElement("wx-" + this._name) as ViewHTMLElement;
        v.scrollView = document.createElement("div");
        v.scrollView!.className = "wx-scroll";
        v.appendChild(v.scrollView!);
        v.contentView = document.createElement("div");
        v.contentView!.className = "wx-content";
        v.scrollView!.appendChild(v.contentView!);

        v.dotsView = document.createElement("div");
        v.dotsView!.className = "wx-dots";
        v.appendChild(v.dotsView!);

        return v;
    }

    public set(key: string, value: string | undefined) {
        super.set(key, value);

        if (key == "indicator-dots") {
            if (booleanValue(value)) {
                this.dotsView.style.display = 'block';
            } else {
                this.dotsView.style.display = 'none';
            }
        } else if (key == 'vertical') {
            this._vertical = booleanValue(value);
            let v = this.scrollView;
            if (this._vertical) {
                v.style.overflowX = 'hidden';
                v.style.overflowY = 'auto';
            } else {
                v.style.overflowX = 'auto';
                v.style.overflowY = 'hidden';
            }
            this.setNeedsUpdateItems();
        } else if (key == 'autoplay') {
            this._autoplay = booleanValue(value);
            this.setNeedsAutoplay();
        } else if (key == 'duration') {
            this._duration = parseFloat(value || '0');
        } else if (key == 'interval') {
            this._interval = parseFloat(value || '0');
            this.setNeedsAutoplay();
        }
    }

    protected _vertical: boolean = false;
    protected _items: SwiperItemElement[] = [];

    protected updateDotView(): void {

        let view = this.dotsView;
        let p = view.firstElementChild;
        let index = 0;
        let n = this._items.length;

        while (p) {

            if (index < n) {
                p.className = '';
            } else {
                let e = p.nextElementSibling;
                view.removeChild(p);
                p = e;
                index++;
                continue;
            }

            p = p.nextElementSibling;
            index++;
        }

        while (index < n) {
            let v = document.createElement('span');
            view.appendChild(v);
            index++;
        }

        if (this._vertical) {
            view.className = 'wx-dots wx-vertical';
            view.style.marginLeft = 'auto';
            let h = (9 * this._items.length);
            view.style.height = h + 'px';
            view.style.marginTop = (- h * 0.5) + 'px';
        } else {
            view.className = 'wx-dots';
            view.style.height = 'auto';
            view.style.marginTop = 'auto';
            view.style.marginLeft = (- view.clientWidth * 0.5) + 'px';
        }


    }

    protected updateItems(): void {

        let width: number = this.width;
        let height: number = this.height;
        let index: number = 0;
        let p = this.firstChild;
        let items: SwiperItemElement[] = [];
        let vertical = this._vertical;

        let each = (p: SwiperItemElement): void => {

            if (vertical) {
                p.view.style.top = (index * height) + 'px';
                p.view.style.left = '0px';
            } else {
                p.view.style.left = (index * width) + 'px';
                p.view.style.top = '0px';
            }

            p.view.style.width = width + 'px';
            p.view.style.height = height + 'px';

            items.push(p);
            index++;
        };

        while (p) {

            if (p instanceof SwiperItemElement) {
                each(p);
            } else if (p instanceof BlockElement) {
                let e = p.firstChild;
                while (e) {
                    if (e instanceof SwiperItemElement) {
                        each(e);
                    }
                    e = e.nextSibling;
                }
            }

            p = p.nextSibling;
        }

        this._items = items;

        if (vertical) {
            this.contentView.style.width = width + 'px';
            this.contentView.style.height = (height * index) + 'px';
            this.scrollView.style.width = (width + 20) + 'px';
            this.scrollView.style.height = height + 'px';
        } else {
            this.contentView.style.width = (width * index) + 'px';
            this.contentView.style.height = height + 'px';
            this.scrollView.style.width = width + 'px';
            this.scrollView.style.height = (height + 20) + 'px';
        }

        if (index >= items.length) {
            index = 0;
        }

        this.updateDotView();

        this.scrollToPage(index, false, "autoplay");

        this._updateItemsing = false;
    }

    protected _autoplayId: any;
    protected _autoplaying: boolean = false;

    protected autoplayNextPage(): void {

        if (this._touching) {
            return;
        }

        if(this._scrollAnimationId) {
            return;
        }

        let n = this._items.length;

        if (n == 0) {
            return;
        }

        this.scrollToPage((this._index + 1) % n, true, "autoplay");
    }

    protected setAutoplay(): void {

        if (this._autoplay) {

            clearInterval(this._autoplayId);

            let v = this;

            this._autoplayId = setInterval(function () {

                v.autoplayNextPage();

            }, this._interval);

        } else if (this._autoplayId) {
            clearInterval(this._autoplayId);
            this._autoplayId = undefined;
        }

        this._autoplaying = false;
    }

    protected setNeedsAutoplay(): void {

        if (this._autoplaying) {
            return;
        }

        this._autoplaying = true;

        let v = this;

        once((): void => {
            v.setAutoplay();
        });
    }

    public recycle(): void {
        if (this._autoplayId) {
            clearInterval(this._autoplayId);
            this._autoplayId = undefined;
        }
        super.recycle();
    }

    protected _updateItemsing: boolean = false;

    protected setNeedsUpdateItems(): void {
        if (this._updateItemsing) {
            return;
        }
        this._updateItemsing = true;
        let v = this;
        once((): void => {
            v.updateItems();
        });
    }

    protected onDidAddChildren(element: KKElement): void {
        super.onDidAddChildren(element);
        this.setNeedsUpdateItems();
    }

    protected onWillRemoveChildren(element: KKElement): void {
        super.onWillRemoveChildren(element);
        this.setNeedsUpdateItems();
    }

}
