import { PageViewInterface } from './Page';


interface View extends UIView {
    name?: string
    id?: string
    canvasId?: string
    width?: number
    height?: number
}

interface ViewSet {
    [key: string]: View
}

interface CanvasSet {
    [key: string]: UICanvas
}

export class ViewContext {
    private _viewInterface: PageViewInterface
    private _viewSet: ViewSet
    private _canvasViewSet: ViewSet
    private _canvasSet: CanvasSet

    constructor(viewInterface: PageViewInterface) {
        this._viewInterface = viewInterface;
        this._viewSet = {};
        this._canvasViewSet = {};
        this._canvasSet = {};
    }

    create(name: string, id: string): string {
        var v = this._viewSet[id];
        if (v !== undefined) {
            return id;
        }
        var v = app.createView(name, undefined) as View;
        v.id = id;
        v.name = name;
        this._viewSet[id] = v;
        return id;
    }

    set(id: string, name: string, value: string): void {
        var v = this._viewSet[id];
        if (v) {
            if (v.name == 'canvas' && name == 'canvas-id') {
                v.canvasId = value;
                this._canvasViewSet[v.canvasId] = v;
            }
            v.set(name, value);
        }
    }

    getCanvasContext(canvasId: string): UICanvasContext {
        var v: any = this._canvasSet[canvasId];
        if (v === undefined) {
            var view = this._canvasViewSet[canvasId];
            if (view !== undefined) {
                v = view.createCanvas();
                this._canvasSet[canvasId] = v;
            }
        }
        if (v !== undefined) {
            v = v.getContext('2d');
            if (v !== undefined) {
                if (v.draw === undefined) {
                    if (typeof screen == 'object' && screen.density !== undefined) {
                        v.scale(screen.density, screen.density);
                    }
                    v.draw = function () { };
                }
            }
            return v;
        }
    }

    setFrame(id: string, x: number, y: number, width: number, height: number): void {
        var v = this._viewSet[id];
        if (v) {
            v.setFrame(x, y, width, height);
            v.width = width;
            v.height = height;
        }
    }

    setContentSize(id: string, width: number, height: number): void {
        var v = this._viewSet[id];
        if (v) {
            v.setContentSize(width, height);
        }
    }
    on(id: string, name: string, func: EventFunction): void {
        var v = this._viewSet[id];
        if (v) {
            v.on(name, func);
        }

    }

    off(id: string, name: string): void {
        var v = this._viewSet[id];
        if (v) {
            v.off(name);
        }
    }

    add(id: string, pid?: string): void {
        var v = this._viewSet[id];
        if (v) {
            if (pid === undefined) {
                this._viewInterface.addSubview(v);
            } else {
                var p = this._viewSet[pid];
                if (p) {
                    p.addSubview(v);
                }
            }
        }
    }

    remove(id: string): void {
        var v = this._viewSet[id];
        if (v) {
            v.removeView();
            delete this._viewSet[id];
        }
    }

    recycle(): void {
        for (var id in this._viewSet) {
            var v = this._viewSet[id];
            v.removeView();
        }
        delete this._viewSet;
        delete this._viewInterface;
    }
}

