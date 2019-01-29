
namespace ker {

    export class Dialog {

        private _view: UIView
        private _viewContext: UIViewContext
        private _document: Document
        private _element: LayoutElement
        private _viewElement: UIViewElement
        private _data: Data;
        private _onLayout: EventFunction
        private _layouting: boolean
        private _object:ViewObject

        constructor(object: ViewObject) {
            this._object = object;
            this._data = new Data(object);
            this._view = app.createView("view");
            this._viewContext = new UIViewContext(app);
            this._viewContext.view = this._view;
            this._document = new Document();
            this._element = this._document.createElement("layout") as LayoutElement
            this._document.rootElement = this._element;
            this._viewElement = this._document.createElement("view") as UIViewElement;
            this._element.append(this._viewElement);
            this._layouting = false;
            this._onLayout = (event: Event): void => {
                this.setLayout();
            }
            screen.on("change", this._onLayout);
        }

        recycle(): void {
            screen.off("change", this._onLayout);
            this._data.off([]);
            this._viewElement.recycleView();
            this._view.removeView();
        }

        private layout(): void {
            this._viewContext.setSize(screen.width, screen.height);
            this._viewContext.setUnit("px", screen.density, 0);
            this._element.setFrame(0, 0, screen.width, screen.height);
            this._viewContext.layout(this._element);
            this._viewContext.obtainView(this._viewElement);
            this._layouting = false;
        }

        setLayout(): void {

            if (this._layouting) {
                return;
            }

            this._layouting = true;
            setTimeout((): void => {
                this.layout();
            }, 0);
        }

        setData(data: DataObject): void {
            this._data.setData(data);
            this.setLayout();
        }

        get view(): UIView {
            return this._view
        }

        get document(): Document {
            return this._document
        }

        create(func: (element: Element, data: Data, V: ViewElementFuntion, E: ViewEvaluateFuntion) => void): void {
            View(this._document, this._object, (V: ViewElementFuntion, E: ViewEvaluateFuntion): void => {
                func(this._viewElement, this._data, V, E)
            });
            this.setLayout();
        }

        open(path: string): void {
            View(this._document, this._object, (V: ViewElementFuntion, E: ViewEvaluateFuntion): void => {
                app.exec(path, {
                    element: this._viewElement,
                    data: this._data,
                    V: V,
                    E: E
                });
            });
            this.setLayout();
        }

        show():void {
            this._view.showToScreen();
        }

        hide():void {
            this._view.removeView();
        }
    }

}