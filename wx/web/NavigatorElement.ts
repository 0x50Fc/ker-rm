import { ViewElement } from './ViewElement';
import { postMessage } from './IPC';
import { Document as KKDocument } from './Document';

export class NavigatorElement extends ViewElement {

    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);
        this._hoverStartTime = 50;
        this._hoverStayTime = 600;
        this.set("hover-class","navigator-hover");
    }

    public doEvent(event: Event, name: string, detail: any): void {
        super.doEvent(event, name, detail);

        var v = this.get("url");

        if (name == "tap" && v) {
            postMessage({ page: "open", url: v });
        }
    }
}
