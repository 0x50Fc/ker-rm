
declare namespace ker {

    interface UIPageObject {
        [key: string]: any
        path: string
        view?: string
        document?: Document
        data?: DataObject
        setData?: (object: DataObject) => void
        onload?: (document: Document) => void
        onunload?: () => void
    }

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

}

declare function Page(object: ker.UIPageObject,page:UIPage, setTimeout:any): void