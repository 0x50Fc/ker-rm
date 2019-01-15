//
//  document.h
//  Ker
//
//  Created by zhanghailong on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef document_document_h
#define document_document_h

#include <core/event.h>
#include <core/archiver.h>

namespace kk {
    
    
    typedef Int64 ElementKey;
    
    class Element;
    class Document;
    
    enum {
        DocumentCommandTypeAlloc,
        DocumentCommandTypeSetRoot,
        DocumentCommandTypeSet,
        DocumentCommandTypeAppend,
        DocumentCommandTypeBefore,
        DocumentCommandTypeAfter,
        DocumentCommandTypeRemove,
        DocumentCommandTypeKey
    };
    
    typedef kk::Int32 DocumentCommandType;
    
    class DocumentCommand : public ArchiverObject , public Object  {
    public:
        DocumentCommand(DocumentCommandType type);
        virtual void encode(Archiver * archiver);
        virtual void decode(UnArchiver * unArchiver);
        static kk::Strong<DocumentCommand> newCommand(DocumentCommandType type);
    protected:
        DocumentCommandType _type;
    };
    
    class DocumentAllocElementCommand : public DocumentCommand {
    public:
        DocumentAllocElementCommand();
        virtual void encode(Archiver * archiver);
        virtual void decode(UnArchiver * unArchiver);
    public:
        ElementKey elementId;
        kk::String name;
    };
    
    class DocumentSetRootElementCommand : public DocumentCommand {
    public:
        DocumentSetRootElementCommand();
        virtual void encode(Archiver * archiver);
        virtual void decode(UnArchiver * unArchiver);
    public:
        ElementKey elementId;
    };
    
    class DocumentAppendElementCommand : public DocumentCommand {
    public:
        DocumentAppendElementCommand();
        virtual void encode(Archiver * archiver);
        virtual void decode(UnArchiver * unArchiver);
    public:
        ElementKey elementId;
        ElementKey subelementId;
    };
    
    class DocumentBeforeElementCommand : public DocumentCommand {
    public:
        DocumentBeforeElementCommand();
        virtual void encode(Archiver * archiver);
        virtual void decode(UnArchiver * unArchiver);
    public:
        ElementKey elementId;
        ElementKey siblingId;
    };
    
    class DocumentAfterElementCommand : public DocumentCommand {
    public:
        DocumentAfterElementCommand();
        virtual void encode(Archiver * archiver);
        virtual void decode(UnArchiver * unArchiver);
    public:
        ElementKey elementId;
        ElementKey siblingId;
    };
    
    class DocumentRemoveElementCommand : public DocumentCommand {
    public:
        DocumentRemoveElementCommand();
        virtual void encode(Archiver * archiver);
        virtual void decode(UnArchiver * unArchiver);
    public:
        ElementKey elementId;
    };
    
    class DocumentKeyElementCommand : public DocumentCommand {
    public:
        DocumentKeyElementCommand();
        virtual void encode(Archiver * archiver);
        virtual void decode(UnArchiver * unArchiver);
    public:
        ElementKey key;
        kk::String name;
    };
    
    class DocumentSetElementCommand : public DocumentCommand {
    public:
        DocumentSetElementCommand();
        virtual void encode(Archiver * archiver);
        virtual void decode(UnArchiver * unArchiver);
    public:
        ElementKey elementId;
        ElementKey key;
        kk::String value;
    };
    
    typedef std::function<Element *(Document *,kk::CString,ElementKey)> DocumentCreateElementFunc;
    
    class Document : public EventEmitter {
    public:
        Document();
        virtual ~Document();
        virtual Element * rootElement();
        virtual void setRootElement(Element * element);
        virtual kk::Strong<Element> createElement(CString name);
        virtual kk::Strong<Element> createElement(CString name,ElementKey elementId);
        virtual kk::Strong<Element> element(ElementKey elementId);
        virtual void elementsByName(CString name,std::list<kk::Strong<Element>> & elements);
        virtual kk::Strong<Array<Any>> elementsByName(CString name);
        virtual void set(DocumentCreateElementFunc && func);
        virtual void set(CString name,DocumentCreateElementFunc && func);
        virtual void set(CString name,CString library);
        virtual void set(CString name,ElementKey key);
        virtual ElementKey elementKey(CString name);
        virtual kk::CString key(ElementKey key);
        virtual void startTransaction();
        virtual kk::Strong<ArrayBuffer> commitTransaction();
        virtual void setTransaction(ArrayBuffer * data);
        
        virtual std::map<ElementKey,String> & elementKeys();
        
        virtual String toString();
        
        virtual void execCommand(DocumentCommand * command);
        
        static void library(CString name,DocumentCreateElementFunc && func);
        
        static void Openlib();
        
        Ker_CLASS(Document,EventEmitter,"Document")
        

    protected:
        
        virtual void dispatchCommand(DocumentCommand * command,std::list<Strong<Element>> & strongs);
        
        ElementKey _autoKey;
        kk::Strong<Element> _rootElement;
        std::map<String,DocumentCreateElementFunc> _createElementFuncs;
        std::map<ElementKey,String> _elementKeys;
        std::map<String,ElementKey> _keys;
        std::map<ElementKey,kk::Weak<Element>> _elements;
        std::map<String,std::list<Weak<Element>>> _elementsByName;
        DocumentCreateElementFunc _defaultCreateElementFunc;
        kk::Strong<MemArchiver> _archiver;
    };
    
    
}

#endif /* document_h */
