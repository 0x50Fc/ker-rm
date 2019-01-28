

namespace ker {

    export interface ViewObject {
        [key: string]: any
    }

    export interface ViewAttributeSet {
        [key: string]: string | Evaluate
    }

    type PageViewScopeItem = (enabled: boolean) => boolean
    type PageViewScope = PageViewScopeItem[]
    type PageViewContext = PageViewScope[]

    export type ViewElementFuntion = (element: Element, data: Data, name: string, attrs: ViewAttributeSet, children: (element: Element, data: Data) => void)=>void
    export type ViewEvaluateFuntion = (func: EvaluateScript, keys: string[]) => Evaluate 
    
    export function View(document: Document, object:ViewObject, cb: (V:ViewElementFuntion,E:ViewEvaluateFuntion)=>void): void {

        let pageViewContext: PageViewContext = [];

        function v_AttributeEvaluate(element: Element, data: Data, key: string, evaluate: Evaluate): void {
            data.on(evaluate, (value: any, changedKeys?: DataKeySet): void => {
                if (key == 'data') {
                    let e = new Event();
                    e.data = value;
                    element.emit(key, e);
                } else if (value !== undefined) {
                    element.set(key, value + '');
                }
            });
        }
    
        function v_AttributeEvent(element: Element, name: string, key: string): void {
            element.on(name, (event: Event, name: string): void => {
                let func = object[key];
                if (typeof func == 'function') {
                    func.call(object, event, name);
                }
            });
        }
    
        function v_AttributeSet(element: Element, data: Data, name: string, attrs: ViewAttributeSet): void {
    
            for (let key in attrs) {
                let v = attrs[key];
                if (key.substr(0, 2) == 'on') {
                    if (typeof v == 'string') {
                        v_AttributeEvent(element, key.substr(2), v);
                    }
                } else if (v instanceof Evaluate) {
                    v_AttributeEvaluate(element, data, key, v);
                } else {
                    element.set(key, v);
                }
            }
        };
    
        function v_createElement(name: string): Element {
            return document.createElement(name);
        };
    
        function v_For(element: Element, data: Data, name: string, attrs: ViewAttributeSet, children: (element: Element, data: Data) => void): void {
            let v = attrs["kk:for"];
            if (v instanceof Evaluate) {
                delete attrs["kk:for"];
                let indexKey = attrs["kk:index"] as string || 'index';
                let itemKey = attrs["kk:item"] as string || 'item';
                let elements: Element[] = [];
                let datas: Data[] = [];
                let before = v_createElement("element");
                element.append(before);
                data.on(v, (value: any, changedKeys?: DataKeySet): void => {
                    let index = 0;
                    let forItem = (item: any): void => {
                        let e = index < elements.length ? elements[index] : undefined;
                        let d = index < datas.length ? datas[index] : undefined;
                        if (e === undefined) {
                            e = v_createElement(name);
                            before.before(e);
                            d = new Data(data.global);
                            d.setParent(data);
                            v_AttributeSet(e, d, name, attrs);
                            pageViewContext.push([]);
                            children(e, d);
                            pageViewContext.pop();
                            d!.set([indexKey], index, false);
                            d!.set([itemKey], item, false);
                            d!.changeKeys();
                        } else {
                            d!.begin();
                            d!.set([indexKey], index, false);
                            d!.set([itemKey], item, false);
                            d!.commit();
                        }
                        index++;
                    };
                    if (value instanceof Array) {
                        for (let v of value) {
                            forItem(v);
                        }
                    }
                    while (index < elements.length) {
                        let e = elements.pop()!;
                        let d = datas.pop()!;
                        if (e instanceof UIViewElement) {
                            e.recycleView();
                        }
                        e.off();
                        e.remove();
                        d.recycle();
                    }
                });
            }
        };
    
        function v_IfElseIfElse(evaluate: Evaluate | undefined, element: Element, data: Data, name: string, attrs: ViewAttributeSet, children: (element: Element, data: Data) => void): void {
    
            let scope: PageViewScope = pageViewContext[pageViewContext.length - 1];
            let before = document.createElement("element");
            let e: Element | undefined;
            let d: Data | undefined;
    
            element.append(before);
    
            scope.push((enabled: boolean): boolean => {
                if (enabled && (evaluate === undefined || (evaluate as Evaluate).exec(data.object, data.global))) {
    
                    if (e === undefined) {
                        e = v_createElement(name);
                        before.before(e);
                        d = new Data(data.global);
                        d.setParent(data);
                        v_AttributeSet(e, d, name, attrs);
                        pageViewContext.push([]);
                        children(e, d);
                        pageViewContext.pop();
                    }
    
                    d.changeKeys();
    
                    return true;
                } else if (e !== undefined) {
                    if (e instanceof UIViewElement) {
                        e.recycleView();
                    }
                    e.off();
                    e.remove();
                    e = undefined;
                    d!.recycle();
                    d = undefined;
                }
    
                return false;
            });
    
            if (evaluate !== undefined) {
                data.on(evaluate, (value: any, changedKeys?: DataKeySet): void => {
                    let enabled = true;
                    for (let item of scope) {
                        if (item(enabled)) {
                            enabled = false;
                        }
                    }
                });
            }
    
        };
    
        function v_Element(element: Element, data: Data, name: string, attrs: ViewAttributeSet, children: (element: Element, data: Data) => void): void {
            if (name == "body") {
                v_AttributeSet(element, data, name, attrs);
                pageViewContext.push([]);
                children(element, data);
                pageViewContext.pop();
            } else if (attrs["kk:for"]) {
                v_For(element, data, name, attrs, children);
            } else if (attrs["kk:if"]) {
                let v = attrs["kk:if"];
                if (v instanceof Evaluate) {
                    delete attrs["kk:if"];
                    v_IfElseIfElse(v, element, data, name, attrs, children);
                }
            } else if (attrs["kk:elseif"]) {
                let v = attrs["kk:elseif"];
                if (v instanceof Evaluate) {
                    delete attrs["kk:elseif"];
                    v_IfElseIfElse(v, element, data, name, attrs, children);
                }
            } else if (attrs["kk:else"]) {
                delete attrs["kk:else"];
                v_IfElseIfElse(undefined, element, data, name, attrs, children);
            } else {
                let e = v_createElement(name);
                element.append(e);
                v_AttributeSet(e, data, name, attrs);
                pageViewContext.push([]);
                children(e, data);
                pageViewContext.pop();
            }
        };

        pageViewContext.push([]);
        
        cb(v_Element,(func: EvaluateScript, keys: string[]):Evaluate=>{
            return new Evaluate(func,keys);
        });

        pageViewContext.pop();

    }
}