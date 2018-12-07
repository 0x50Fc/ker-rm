import { Element as KKElement } from './Element';
import { ViewElement } from './ViewElement';

export class BlockElement extends KKElement {

    protected onDidAddChildren(element: KKElement): void {
        super.onDidAddChildren(element);
        if (element instanceof ViewElement) {
            let p = this.parent;
            if (p instanceof ViewElement) {
                p.view.appendChild(element.view);
            } else if (p) {
                document.body.appendChild(element.view);
            }
        }
    }

    protected onWillRemoveFromParent(element: KKElement): void {
        super.onWillRemoveFromParent(element);
        let p = this.firstChild;
        while (p) {
            if (p instanceof ViewElement) {
                let v = p.view;
                let pv = v.parentElement;
                if (pv) {
                    pv.removeChild(v);
                }
            }
            p = p.nextSibling;
        }
    }

}
