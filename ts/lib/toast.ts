
namespace ker {

    class ToastView {
        private _view: UIView
        private _viewContext: UIViewContext
        private _document: Document
        private _element: LayoutElement
        private _viewElement: UIViewElement
        private _data: Data;

        constructor() {
            this._data = new Data(global);
            this._view = app.createView("view");
            this._viewContext = new UIViewContext(app);
            this._viewContext.view = this._view;
            this._document = new Document();
            this._element = this._document.createElement("layout") as LayoutElement
            this._document.rootElement = this._element;

            View(this._document, {}, (V: ViewElementFuntion, E: ViewEvaluateFuntion): void => {
                V(this._element, this._data, "view", {
                    'max-width': '400rpx',
                    'padding': '20rpx',
                    'background-color': 'rgba(0,0,0,0.75)',
                    'border-radius': '8rpx'
                }, (element: Element, data: Data): void => {
                    this._viewElement = element as UIViewElement
                    V(element, data, "text", {
                        '#text': E(function (title) { return title; }, ['title']),
                        'font': '28rpx',
                        'color': '#fff'
                    }, (element: Element, data: Data): void => { });
                });
            });
        }

        layout(): void {

            this._viewContext.setSize(screen.width, screen.height);
            this._viewContext.setUnit("px", screen.density, 0);

            this._element.setFrame(0, 0, screen.width, screen.height);
            this._viewContext.layout(this._element);
            this._viewContext.obtainView(this._viewElement);
        }

        setData(data: DataObject): void {
            this._data.setData(data);
            this.layout();
        }

        get view():UIView{
            return this._view
        }
        
    }


    let views: ToastView[] = [];

    screen.on("change", (event: Event): void => {
        for(let v of views) {
            v.layout();
        }
    });

    export interface ShowToastObject {

    }

    export function showToast(object: ShowToastObject): void {

    }

    export interface HideToastObject {

    }

    export function hideToast(object: HideToastObject): void {
        if (views.length > 0) {
            let view = views.pop();
            app.hideView
        }
    }
}
