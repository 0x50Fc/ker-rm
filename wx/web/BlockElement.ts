import { Element as KKElement  } from './Element';
import { ViewElement } from './ViewElement';

export class BlockElement extends KKElement {

    protected onWillRemoveFromParent(element: KKElement): void {
        super.onWillRemoveFromParent(element);
        let p = this.firstChild;
        while(p) {
            if(p instanceof ViewElement) {
                let v = p.view;
                let pv = v.parentElement;
                if(pv) {
                    pv.removeChild(v);
                }
            }
            p = p.nextSibling;
        }
    }
    
}
