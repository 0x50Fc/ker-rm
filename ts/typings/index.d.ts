
declare namespace ker {

    interface DataObject {
        [key: string]: any
    }

    interface DataKeySet {
        [key: string]: boolean
    }


    type EvaluateScript = any;

    class Evaluate {

        evaluateScript: EvaluateScript
        keys: string[]
        keySet: DataKeySet
        constructor(evaluateScript: EvaluateScript, keys: string[])
        exec(object: DataObject, global: DataObject): any
    }

    type DataFunction = (value: any, changedKeys: DataKeySet) => void;

    class IObject {
        [key: string]: any
        get(key: string): any
        set(key: string, value: any): void
    }

    class Data {

        constructor(global: DataObject);

        object: DataObject

        setData(data: DataObject, changed?: boolean): void

        begin(): void

        commit(): void

        cancel(): void

        changeKeys(keySet: DataKeySet): void

        on(keys: string[] | Evaluate, func: DataFunction, priority?: number): void

        off(keys: string[] | Evaluate, func: DataFunction | undefined): void

        setParent(parent: Data | undefined): void

        recycle(): void

        get(keys: string[]): any

        set(keys: string[], value: any, changed: boolean): void

        static get(object: any, keys: string[], index?: number): any

        static set(object: any, keys: string[], value: any, index?: number): void

    }


    interface ViewObject {
        [key: string]: any
    }

    type ViewElementFuntion = (element: Element, data: Data, name: string, attrs: ViewAttributeSet, children: (element: Element, data: Data) => void)=>void
    type ViewEvaluateFuntion = (func: EvaluateScript, keys: string[]) => Evaluate 
    
    function View(document: Document, object:ViewObject, cb: (V:ViewElementFuntion,E:ViewEvaluateFuntion)=>void): void 

    interface UIPageObject extends ViewObject {
        path: string
        view?: string
        document?: Document
        data?: DataObject
        setData?: (object: DataObject) => void
        onload?: (document: Document) => void
        onunload?: () => void
    }

    interface KerAudioStartRecordRes {
        readonly tempFilePath: string
    }

    interface KerAudioStartRecordObject {
        success?: (res: KerAudioStartRecordRes) => void
        fail?: (errmsg?: string) => void
        complete?: () => void
    }

    function startRecord(object: KerAudioStartRecordObject): void
    function stopRecord(): void

    class RequestRes {
        data: any
        statusCode: number
        header: HeaderSet
    }

    interface RequestObject {
        url: string
        data?: any
        header?: HeaderSet
        method?: string
        dataType?: string
        responseType?: string
        success?: (res: RequestRes) => void
        fail?: (errmsg?: string) => void
        complete?: () => void
    }

    class RequestTask {
        constructor(request: HttpRequest)
        onHeadersReceived(v: (header: HeaderSet) => void): void
        offHeadersReceived(v?: (header: HeaderSet) => void): void
        abort(): void
    }

    function request(object: RequestObject): RequestTask;

    function Page(object: UIPageObject, page: UIPage, setTimeout: any): void
}
