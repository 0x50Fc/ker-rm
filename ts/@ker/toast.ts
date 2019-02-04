
namespace ker {

    class ToastView extends Dialog {

        constructor(object:ViewObject) {
            super(object)
        }

        create() {
            super.create((element: Element, data: Data, V: ViewElementFuntion, E: ViewEvaluateFuntion): void => {
                V(element, data, "body", {
                    'max-width': '400rpx',
                    'padding': '20rpx',
                    'background-color': 'rgba(0,0,0,0.75)',
                    'border-radius': '8rpx'
                }, (element: Element, data: Data): void => {
                    V(element, data, "text", {
                        '#text': E(function (title) { return title; }, ['title']),
                        'font': '28rpx',
                        'color': '#fff'
                    }, (element: Element, data: Data): void => { });
                });
            });
        }

    }

    interface ViewSet {
        [key: string]: ToastView
    }

    let audoId: number = 0;
    let viewSet: ViewSet = {};

    export interface ShowToastObject {
        title: string
        duration?: number
        success?: () => void
        fail?: (errmsg?: string) => void
        complete?: () => void
    }

    export function showToast(object: ShowToastObject): void {

        if (object.duration === undefined) {
            object.duration = 1500;
        }

        let id = (++audoId) + '';
        let view = new ToastView({});
        view.create();
        view.setData({
            title: object.title
        });
        view.layout();
        view.show();
        viewSet[id] = view;

        setTimeout(function (): void {
            view.hide();
            view.recycle();
            delete viewSet[id];
        }, object.duration);

        if (object.success !== undefined) {
            object.success();
        }
        if (object.complete !== undefined) {
            object.complete();
        }
    }

    export interface HideToastObject {
        success?: () => void
        fail?: (errmsg?: string) => void
        complete?: () => void
    }

    export function hideToast(object: HideToastObject): void {

        let view: ToastView | undefined;
        let id: number = 0;

        for (let key in viewSet) {
            if (parseInt(key) > id) {
                id = parseInt(key);
                view = viewSet[key];
            }
        }

        if (view !== undefined) {
            view.hide();
            view.recycle();
            delete viewSet[id + ''];
        }

        if (object.success !== undefined) {
            object.success();
        }
        if (object.complete !== undefined) {
            object.complete();
        }

    }
}
