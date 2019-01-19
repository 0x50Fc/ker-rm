
declare namespace ker {

    class Event {
        data: any
        returnValue: any
    }
    
    type EventFunction = (name: string, event: Event) => void
    
    class EventEmitter {
        on(name: string, func: EventFunction): void
        off(name?: string, func?: EventFunction): void
        has(name: string): boolean
        emit(name: string, event: Event): void
    }
    
    type ElementKey = number
    
    interface AttributeMap {
        [key: string]: string
    }
    
    interface Element {
        readonly elementId: ElementKey
        readonly name: string
        readonly document: Document
        readonly firstChild: Element | undefined
        readonly lastChild: Element | undefined
        readonly nextSibling: Element | undefined
        readonly prevSibling: Element | undefined
        readonly parent: Element | undefined
        append(element: Element): void
        appendTo(element: Element): void
        before(element: Element): void
        beforeTo(element: Element): void
        after(element: Element): void
        afterTo(element: Element): void
        remove(): void
        set(key: string, value: string | undefined): void
        get(key: string): string | undefined
        attributes(): AttributeMap
        setObject(key: string, object: any): void
        object(key: string): any
        dispatchEvent(name: string, event: Event): void
    }
    
    interface StyleElement extends Element {
        status: string | undefined
        addStatus(s: string): void
        removeStatus(s: string): void
        hasStatus(s: string): boolean
        changedStatus(): void
    }
    
    interface LayoutElement extends StyleElement {
        setFrame(x: number, y: number, width: number, height: number): void
        setContentSize(width: number, height: number): void
    }
    
    type LayoutFunction = (context: LayoutContext, element: LayoutElement) => void
    
    class LayoutContext {
        setSize(width: number, height: number): void
        setUnit(name: string, scale: number, rate: number): void
        setLayout(name: string, func: LayoutFunction): void
        layout(element: LayoutElement): void
    }
    
    class Document extends EventEmitter {
        rootElement: Element | undefined
        createElement(name: string): Element
        elementsByName(name: string): Element[]
        set(name: string, library: string): void
        startTransaction(): void
        commitTransaction(): ArrayBuffer
        setTransaction(data: ArrayBuffer): void
        toString(): string
    }
    
    
    interface Library {
        [key: string]: any
    }
    
    interface Worker {
        terminate(): void
        postMessage(data: any): void
    }
    
    interface UICanvas extends EventEmitter {
        getContext(name: string): CanvasRenderingContext2D | WebGLRenderingContext
        width: number
        height: number
    }
    
    enum ImageState {
        None = 0,
        Loading = 1,
        Error = 2,
        Loaded = 3
    }
    
    interface Image extends EventEmitter {
        readonly src: string
        readonly width: number
        readonly height: number
        readonly state: ImageState
    }
    
    interface UIContext extends EventEmitter {
        exec(path: string, library?: Library): void
        getTextContent(path: string): string
        createWorker(path: string): Worker
        createCanvas(): UICanvas
        createImage(src: string): Image
    }
    
    class UIAttributedText {
        clear(): void
        appendText(text: string, font: string, color: string): void
        appendImage(image: Image, width?: number, height?: number, top?: number, left?: number, bottom?: number, right?: number): void
    }
    
    interface Size {
        width: number;
        height: number
    }
    
    interface SqliteResultSet {
        getColumnCount(): number
        getColumnName(index: number): string
        getValue(index: number): any
        next(): boolean
        close(): void
    }
    
    interface Sqlite {
        exec(sql: String, args: any[]): void
        query(sql: String, args: any[]): SqliteResultSet
        lastInsertRowId(): number
        close(): void
    }
    
    class UIViewConfiguration {
    
    }
    
    enum UIWebViewUserScriptInjectionTime {
        Start = 0,
        End = 1
    }
    
    enum UIWebViewActionPolicy {
        Cancel = 0,
        Allow = 1
    }
    
    class UIWebViewConfiguration extends UIViewConfiguration {
        addUserScript(code: string, injectionTime: UIWebViewUserScriptInjectionTime): void
        addUserAction(pattern: string, policy: UIWebViewActionPolicy): void
    }
    
    enum UISubviewPosition {
        Front = 0,
        Back = 1
    }
    
    interface Point {
        x: number,
        y: number
    }
    
    interface UIView extends EventEmitter {
        set(key: string, value?: string): void
        setFrame(x: number, y: number, width: number, height: number): void
        setContentSize(width: number, height: number): void
        setContentOffset(x: number, y: number): void
        addSubview(v: UIView, position?: UISubviewPosition): void
        removeView(): void
        evaluateJavaScript(code: string): void
        setContent(content: string, contentType?: string, basePath?: string): void
        setAttributedText(text: UIAttributedText | undefined): void
        setImage(image: Image | undefined): void
        contentOffset(): Point
        createCanvas(worker?: Worker): UICanvas
    }
    
    type StorageLoadCallback = (value: string | undefined) => void
    type StorageLoadKeysCallback = (keys: string[]) => void
    
    interface Storage {
        get(key: string): string | undefined
        set(key: string, value: string): void
        load(key: string, cb: StorageLoadCallback): void
        clear(): void
        keys(): string[]
        loadKeys(cb: StorageLoadKeysCallback): void
    }
    
    interface UIApp extends UIContext {
        open(uri: string, animated: boolean): void
        back(delta: number, animated: boolean): void
        getAttributedTextContentSize(text: UIAttributedText, maxWidth: number): Size
        createSqlite(path: string): Sqlite
        createView(name: string, configuration: UIViewConfiguration): UIView
        readonly appkey: string
        readonly storage: Storage
    }
    
    interface UIViewElement extends LayoutElement {
        readonly view: UIView | undefined
        obtainView(context: UIViewContext): void
        recycleView(): void
    }
    
    class UIViewContext extends LayoutContext {
        constructor(app:UIApp)
        view: UIView | undefined
        obtainView(element: UIViewElement): void
    }
    
    interface UISpanElement extends StyleElement {
    
    }
    
    interface UIImgElement extends StyleElement {
    
    }
    
    interface UITextElement extends UIViewElement {
        readonly text: UIAttributedText | undefined
    }
    
    
    interface UIPageOptions {
        [key:string]:any
    }
    
    interface UIPage extends EventEmitter {
        readonly app:UIApp
        readonly view:UIView
        readonly width:number
        readonly height:number
        setOptions(options:UIPageOptions):void
        close(animated:boolean):void
    }

    interface UIPageObject {
        [key: string]: any
        path: string
        view?: string
        document?: Document
        data?: Data
        onload?: (document: Document, data: Data) => void
        onunload?: () => void
    }

}


declare var app: ker.UIApp
declare var userAgent: string
declare var platform: string
declare var page: ker.UIPage

