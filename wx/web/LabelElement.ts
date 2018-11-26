import { ViewElement  } from './ViewElement';
import { ButtonElement } from './ButtonElement';
import { CheckboxElement } from './CheckboxElement';
import { RadioElement } from './RadioElement';
import { SwitchElement } from './SwitchElement';

export class LabelElement extends ViewElement {


    public doEvent(event: Event, name: string, detail: any): void {
        super.doEvent(event, name, detail);

        if(name == "tap") {

            let p = this.firstChild;
            while(p) {
                if(p instanceof ButtonElement || p instanceof CheckboxElement || p instanceof RadioElement || p instanceof SwitchElement) {
                    p.doEvent(event,name,detail);
                    break;
                }
                p = p.nextSibling;
            }
        }
    }

}
