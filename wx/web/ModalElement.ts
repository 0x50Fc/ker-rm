import { ViewElement } from './ViewElement';
import { Document as KKDocument } from './Document';
import { FormElement } from './FormElement';
import { booleanValue } from './V';

//<modal title="{{dialog.title}}" hidden="{{dialog.hidden}}" no-cancel bindconfirm="confirm">{{dialog.content}}</modal>

export class ModalElement extends ViewElement {


    protected _boxView?:HTMLElement;
    protected _titleView?:HTMLElement;
    protected _contentView?:HTMLElement;
    protected _bottomView?:HTMLElement;
    protected _cancelView?:HTMLElement;
    protected _confirmView?:HTMLElement;
    
    constructor(document: KKDocument, name: string, id: number) {
        super(document, name, id);
    }

    protected createView(): HTMLElement {
        let v = super.createView();
        this._boxView = document.createElement("div");
        this._boxView.className = "wx-modal-box";
        this._titleView = document.createElement("div");
        this._titleView.className = "wx-modal-title";
        this._contentView = document.createElement("div");
        this._contentView.className = "wx-modal-content";
        this._bottomView = document.createElement("div");
        this._bottomView.className = "wx-modal-bottom";
        this._cancelView = document.createElement("div");
        this._cancelView.className = "wx-modal-cancel";
        this._confirmView = document.createElement("div");
        this._confirmView.className = "wx-modal-confirm";

        v.appendChild(this._boxView);

        this._boxView.appendChild(this._titleView);
        this._boxView.appendChild(this._contentView);
        this._boxView.appendChild(this._bottomView);

        this._bottomView.appendChild(this._cancelView);
        this._bottomView.appendChild(this._confirmView);

        this._cancelView.innerText = '取消';
        this._confirmView.innerText = '确定';

        let element = this;

        this._confirmView.addEventListener("mouseup", (event: Event): void => {
            element.doEvent(event,"confirm",{});
            element.view.style.display = 'none';
        });

        this._cancelView.addEventListener("mouseup", (event: Event): void => {
            element.doEvent(event,"cancel",{});
            element.view.style.display = 'none';
        });
        
        return v;
    }

    public set(key: string, value: string | undefined) {

        if(key == "title") {
            this._titleView!.innerText = value || '';
            this._titleView!.style.display = value ? 'block' : 'none';
        } else if(key == '#text') {
            this._contentView!.innerText = value || '';
            this._contentView!.style.display = value ? 'block' : 'none';
        } else if(key == 'no-cancel') {
            this._cancelView!.style.display = 'none';
            this._confirmView!.style.width = '100%';
        } else if(key == 'hidden') {
            if(booleanValue(value)) {
                this.view.style.display = 'none';
            } else {
                this.view.style.display = 'block';
            }
        } else {
            super.set(key, value);
        }

    }

}
