//
//  document.cc
//  Ker
//
//  Created by zhanghailong on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "document.h"
#include "element.h"

namespace kk {
    
    DocumentCommand::DocumentCommand(DocumentCommandType type):_type(type) {
        
    }
    
    void DocumentCommand::encode(Archiver * archiver) {
        archiver->encode((kk::Int32) _type);
    }
    
    void DocumentCommand::decode(UnArchiver * unArchiver) {

    }
    
    kk::Strong<DocumentCommand> DocumentCommand::newCommand(DocumentCommandType type) {
        switch (type) {
            case DocumentCommandTypeAlloc:
                return new DocumentAllocElementCommand();
                break;
            case DocumentCommandTypeSetRoot:
                return new DocumentSetRootElementCommand();
                break;
            case DocumentCommandTypeSet:
                return new DocumentSetElementCommand();
                break;
            case DocumentCommandTypeAppend:
                return new DocumentAppendElementCommand();
                break;
            case DocumentCommandTypeBefore:
                return new DocumentBeforeElementCommand();
                break;
            case DocumentCommandTypeAfter:
                return new DocumentAfterElementCommand();
                break;
            case DocumentCommandTypeRemove:
                return new DocumentRemoveElementCommand();
                break;
            case DocumentCommandTypeKey:
                return new DocumentKeyElementCommand();
                break;
            default:
                break;
        }
        return nullptr;
    }
    
    DocumentAllocElementCommand::DocumentAllocElementCommand():DocumentCommand(DocumentCommandTypeAlloc),elementId(0) {
        
    }
    
    void DocumentAllocElementCommand::encode(Archiver * archiver) {
        DocumentCommand::encode(archiver);
        archiver->encode(elementId);
        archiver->encode(name.c_str());
    }
    
    void DocumentAllocElementCommand::decode(UnArchiver * unArchiver) {
        DocumentCommand::decode(unArchiver);
        if(unArchiver->next() == ArchiverTypeInt64) {
            elementId = unArchiver->int64Value;
        }
        if(unArchiver->next() == ArchiverTypeString) {
            name = unArchiver->stringValue;
        }
    }
   
    DocumentSetRootElementCommand::DocumentSetRootElementCommand():DocumentCommand(DocumentCommandTypeSetRoot) ,elementId(0){
        
    }
    
    void DocumentSetRootElementCommand::encode(Archiver * archiver) {
        DocumentCommand::encode(archiver);
        archiver->encode(elementId);
    }
    
    void DocumentSetRootElementCommand::decode(UnArchiver * unArchiver) {
        DocumentCommand::decode(unArchiver);
        if(unArchiver->next() == ArchiverTypeInt64) {
            elementId = unArchiver->int64Value;
        }
    }
    
    DocumentAppendElementCommand::DocumentAppendElementCommand()
        :DocumentCommand(DocumentCommandTypeAppend),elementId(0),subelementId(0) {
        
    }
    
    void DocumentAppendElementCommand::encode(Archiver * archiver) {
        DocumentCommand::encode(archiver);
        archiver->encode(elementId);
        archiver->encode(subelementId);
    }
    
    void DocumentAppendElementCommand::decode(UnArchiver * unArchiver) {
        DocumentCommand::decode(unArchiver);
        if(unArchiver->next() == ArchiverTypeInt64) {
            elementId = unArchiver->int64Value;
        }
        if(unArchiver->next() == ArchiverTypeInt64) {
            subelementId = unArchiver->int64Value;
        }
    }
    
    DocumentBeforeElementCommand::DocumentBeforeElementCommand():DocumentCommand(DocumentCommandTypeBefore),elementId(0),siblingId(0) {
        
    }
    
    void DocumentBeforeElementCommand::encode(Archiver * archiver) {
        DocumentCommand::encode(archiver);
        archiver->encode(elementId);
        archiver->encode(siblingId);
    }
    
    void DocumentBeforeElementCommand::decode(UnArchiver * unArchiver) {
        DocumentCommand::decode(unArchiver);
        if(unArchiver->next() == ArchiverTypeInt64) {
            elementId = unArchiver->int64Value;
        }
        if(unArchiver->next() == ArchiverTypeInt64) {
            siblingId = unArchiver->int64Value;
        }
    }
    
    DocumentAfterElementCommand::DocumentAfterElementCommand():DocumentCommand(DocumentCommandTypeAfter),elementId(0),siblingId(0) {
        
    }
    
    void DocumentAfterElementCommand::encode(Archiver * archiver) {
        DocumentCommand::encode(archiver);
        archiver->encode(elementId);
        archiver->encode(siblingId);
    }
    
    void DocumentAfterElementCommand::decode(UnArchiver * unArchiver) {
        DocumentCommand::decode(unArchiver);
        if(unArchiver->next() == ArchiverTypeInt64) {
            elementId = unArchiver->int64Value;
        }
        if(unArchiver->next() == ArchiverTypeInt64) {
            siblingId = unArchiver->int64Value;
        }
    }
    
    DocumentRemoveElementCommand::DocumentRemoveElementCommand():DocumentCommand(DocumentCommandTypeRemove),elementId(0) {
        
    }
    
    void DocumentRemoveElementCommand::encode(Archiver * archiver) {
        DocumentCommand::encode(archiver);
        archiver->encode(elementId);
    }
    
    void DocumentRemoveElementCommand::decode(UnArchiver * unArchiver) {
        DocumentCommand::decode(unArchiver);
        if(unArchiver->next() == ArchiverTypeInt64) {
            elementId = unArchiver->int64Value;
        }
    }
    
    DocumentKeyElementCommand::DocumentKeyElementCommand():DocumentCommand(DocumentCommandTypeKey),key(0) {
        
    }
    
    void DocumentKeyElementCommand::encode(Archiver * archiver) {
        DocumentCommand::encode(archiver);
        archiver->encode(key);
        archiver->encode(name.c_str());
    }
    
    void DocumentKeyElementCommand::decode(UnArchiver * unArchiver) {
        DocumentCommand::decode(unArchiver);
        if(unArchiver->next() == ArchiverTypeInt64) {
            key = unArchiver->int64Value;
        }
        if(unArchiver->next() == ArchiverTypeString) {
            name = unArchiver->stringValue;
        }
    }
    
    DocumentSetElementCommand::DocumentSetElementCommand():DocumentCommand(DocumentCommandTypeSet),elementId(0),key(0) {
        
    }
    
    void DocumentSetElementCommand::encode(Archiver * archiver) {
        DocumentCommand::encode(archiver);
        archiver->encode(elementId);
        archiver->encode(key);
        archiver->encode(value.c_str());
    }
    
    void DocumentSetElementCommand::decode(UnArchiver * unArchiver) {
        DocumentCommand::decode(unArchiver);
        if(unArchiver->next() == ArchiverTypeInt64) {
            elementId = unArchiver->int64Value;
        }
        if(unArchiver->next() == ArchiverTypeInt64) {
            key = unArchiver->int64Value;
        }
        if(unArchiver->next() == ArchiverTypeString) {
            value = unArchiver->stringValue;
        }
    }
    
    static std::map<String,DocumentCreateElementFunc> gLibrary;
    
    Document::Document():_autoKey(0) {
        
    }
    
    Document::~Document() {
        
    }
    
    Element * Document::rootElement() {
        return (Element *) _rootElement;
    }
    
    void Document::setRootElement(Element * element) {
        
        _rootElement = element;
        
        kk::Strong<DocumentSetRootElementCommand> cmd = new DocumentSetRootElementCommand();
        
        if(element != nullptr) {
            cmd->elementId = element->elementId();
        }
        
        execCommand(cmd);
        
    }
    
    Strong<Element> Document::element(ElementKey elementId) {
        Strong<Element> v;
        auto i = _elements.find(elementId);
        if(i != _elements.end()) {
            Weak<Element> & vv = i->second;
            Element * e = vv;
            if(e == nullptr) {
                _elements.erase(i);
            } else {
                v = e;
            }
        }
        return v;
    }
    
    Strong<Element> Document::createElement(CString name,ElementKey elementId) {
        
        Strong<Element> v;
        
        DocumentCreateElementFunc func;
        
        if(elementId == 0) {
            elementId = ++ _autoKey;
        } else if(elementId > _autoKey) {
            _autoKey = elementId;
        }
        
        {
            std::map<String,DocumentCreateElementFunc>::iterator i = _createElementFuncs.find(name);
            if(i != _createElementFuncs.end()) {
                func = i->second;
            }
        }
        
        if(func == nullptr) {
            auto i = gLibrary.find(name);
            if(i != gLibrary.end()) {
                func = i->second;
            }
        }
        
        if(func == nullptr && _defaultCreateElementFunc == nullptr) {
            auto i = gLibrary.find("");
            if(i != gLibrary.end()) {
                func = i->second;
            }
        }
        
        if(func != nullptr) {
            v = func(this,name,elementId);
        } else if(_defaultCreateElementFunc != nullptr) {
            v = _defaultCreateElementFunc(this,name,elementId);
        }
        
        if(v == nullptr) {
            v = new Element(this,name,elementId);
        }
        
        _elements[elementId] = (Element *) v;
        
        {
            auto i = _elementsByName.find(name);
            if(i == _elementsByName.end()) {
                _elementsByName[name] = std::list<Weak<Element>>();
                i = _elementsByName.find(name);
            }
            auto & vs = i->second;
            vs.push_back((Element *) v);
        }
        
        {
            kk::Strong<DocumentAllocElementCommand> cmd = new DocumentAllocElementCommand();
            
            cmd->elementId = elementId;
            cmd->name = name;
            
            execCommand(cmd);
        }
        
        return v;
        
    }
    
    Strong<Element> Document::createElement(CString name) {
        return createElement(name, 0);
    }
    
    void Document::set(DocumentCreateElementFunc && func) {
        _defaultCreateElementFunc = func;
    }
    
    void Document::set(CString name,DocumentCreateElementFunc && func) {
        _createElementFuncs[name] = func;
    }
    
    kk::CString Document::key(ElementKey key) {
        std::map<ElementKey,String>::iterator i = _elementKeys.find(key);
        if(i != _elementKeys.end()) {
            return i->second.c_str();
        }
        return nullptr;
    }
    
    ElementKey Document::elementKey(CString name) {
        
        std::map<String,ElementKey>::iterator i = _keys.find(name);
        
        if(i == _keys.end()) {
            
            ElementKey key = ++ _autoKey;
            
            _keys[name] = key;
            _elementKeys[key] = name;
            
            {
                kk::Strong<DocumentKeyElementCommand> cmd = new DocumentKeyElementCommand();
                
                cmd->key = key;
                cmd->name = name;
                
                execCommand(cmd);
                
            }
            
            return key;
        }
        
        return i->second;
    }
    
    void Document::startTransaction() {
        _archiver = new MemArchiver();
    }
    
    kk::Strong<ArrayBuffer> Document::commitTransaction() {
        kk::Strong<ArrayBuffer> v;
        if(_archiver != nullptr) {
            size_t n = 0;
            const void * data = _archiver->data(&n);
            if(data && n > 0) {
                v = new ArrayBuffer((void *)data, (kk::Uint) n);
            }
            _archiver = nullptr;
        }
        return v;
    }
    
    void Document::setTransaction(ArrayBuffer * data) {
        if(data == nullptr) {
            return ;
        }
        
        kk::Strong<MemUnArchiver> a = new MemUnArchiver(data);
        
        ArchiverType type = ArchiverTypeNil;
        
        std::list<Strong<Element>> strongs;
        
        do {
            
            type = a->next();
            
            if(type == ArchiverTypeObject) {
                
                type = a->next();
                
                if(type == ArchiverTypeInt32) {
                    
                    kk::Strong<DocumentCommand> v = DocumentCommand::newCommand((DocumentCommandType) a->int32Value);
                    
                    if(v != nullptr) {
                        v->decode(a);
                        dispatchCommand(v,strongs);
                    }
                    
                    a->endObject();
                    
                    continue;
                    
                } else if(type == ArchiverTypeDone) {
                    continue;
                } else {
                    a->endObject();
                    continue;
                }
                
            }
            
        } while (type == ArchiverTypeObject);
    }
    
    
    
    void Document::set(CString name,CString library) {
        
        if(name != nullptr && library != nullptr) {
            return;
        }
        
        std::map<String,DocumentCreateElementFunc>::iterator i = gLibrary.find(library);
        
        if(i != gLibrary.end()) {
            set(name,std::move(i->second));
        }
    }
    
    
    void Document::library(CString name,DocumentCreateElementFunc && func) {
        gLibrary[name] = func;
    }
    
    void Document::set(CString name,ElementKey key) {
        if(key > _autoKey) {
            _autoKey = key;
        }
        _keys[name] = key;
        _elementKeys[key] = name;
    }
    
    std::map<ElementKey,String> & Document::elementKeys() {
        return _elementKeys;
    }
    
    void Document::elementsByName(CString name,std::list<Strong<Element>> & elements) {
        auto i = _elementsByName.find(name);
        auto e = _elementsByName.end();
        if(i != e) {
            auto & vs = i->second;
            auto n = vs.begin();
            while(n != vs.end()) {
                auto & v = *n;
                Element * e = v;
                if(e == nullptr) {
                    n = vs.erase(n);
                } else {
                    elements.push_back(e);
                    n ++;
                }
            }
        }
    }
    
    kk::Strong<Array<Any>> Document::elementsByName(CString name) {
        kk::Strong<Array<Any>> elements = new Array<Any>();
        auto i = _elementsByName.find(name);
        auto e = _elementsByName.end();
        if(i != e) {
            auto & vs = i->second;
            auto n = vs.begin();
            while(n != vs.end()) {
                auto & v = *n;
                Element * e = v;
                if(e == nullptr) {
                    n = vs.erase(n);
                } else {
                    elements->push(e);
                    n ++;
                }
            }
        }
        return elements;
    }
    
    
    String Document::toString() {
        String v;
        
        Element * e = rootElement();
        
        if(e) {
            return e->toString();
        }
        
        return v;
    }
    
   
    void Document::execCommand(DocumentCommand * command) {
        if(_archiver != nullptr) {
            _archiver->encode(dynamic_cast<ArchiverObject *>(command));
        }
    }
    
    void Document::dispatchCommand(DocumentCommand * command,std::list<Strong<Element>> & strongs) {
        
        {
            DocumentAllocElementCommand * cmd = dynamic_cast<DocumentAllocElementCommand *>(command);
            if(cmd) {
                kk::Strong<Element> e = createElement(cmd->name.c_str(), cmd->elementId);
                if(e != nullptr) {
                    strongs.push_back((Element *) e);
                }
                return;
            }
        }
        
        {
            DocumentSetRootElementCommand * cmd = dynamic_cast<DocumentSetRootElementCommand *>(command);
            if(cmd) {
                if(cmd->elementId == 0) {
                    setRootElement(nullptr);
                } else {
                    kk::Strong<Element> e = element(cmd->elementId);
                    setRootElement(e);
                }
                return;
            }
        }
        
        {
            DocumentSetElementCommand * cmd = dynamic_cast<DocumentSetElementCommand *>(command);
            if(cmd) {
                kk::Strong<Element> e = element(cmd->elementId);
                if(e != nullptr) {
                    if(cmd->value.empty()) {
                        e->set(cmd->key, nullptr);
                    } else {
                        e->set(cmd->key, cmd->value.c_str());
                    }
                }
                return;
            }
        }
        
        {
            DocumentAppendElementCommand * cmd = dynamic_cast<DocumentAppendElementCommand *>(command);
            if(cmd) {
                kk::Strong<Element> e = element(cmd->elementId);
                kk::Strong<Element> p = element(cmd->subelementId);
                if(e != nullptr && p != nullptr) {
                    e->append(p);
                }
                return;
            }
        }
        
        {
            DocumentBeforeElementCommand * cmd = dynamic_cast<DocumentBeforeElementCommand *>(command);
            if(cmd) {
                kk::Strong<Element> e = element(cmd->elementId);
                kk::Strong<Element> p = element(cmd->siblingId);
                if(e != nullptr && p != nullptr) {
                    e->before(p);
                }
                return;
            }
        }
        
        {
            DocumentAfterElementCommand * cmd = dynamic_cast<DocumentAfterElementCommand *>(command);
            if(cmd) {
                kk::Strong<Element> e = element(cmd->elementId);
                kk::Strong<Element> p = element(cmd->siblingId);
                if(e != nullptr && p != nullptr) {
                    e->after(p);
                }
                return;
            }
        }
        
        {
            DocumentRemoveElementCommand * cmd = dynamic_cast<DocumentRemoveElementCommand *>(command);
            if(cmd) {
                kk::Strong<Element> e = element(cmd->elementId);
                if(e != nullptr) {
                    strongs.push_back((Element *) e);
                    e->remove();
                }
                return;
            }
        }
        
        {
            DocumentKeyElementCommand * cmd = dynamic_cast<DocumentKeyElementCommand *>(command);
            if(cmd) {
                set(cmd->name.c_str(), cmd->key);
                return;
            }
        }
        
    }
    
    void Document::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushClass<Document>(ctx, [](duk_context * ctx)->void{
                
                kk::PutProperty<Document,Element *>(ctx, -1, "rootElement", &Document::rootElement,&Document::setRootElement);
                kk::PutStrongMethod<Document,Element,kk::CString>(ctx, -1, "createElement", &Document::createElement);
                kk::PutStrongMethod<Document,Array<Any>,kk::CString>(ctx, -1, "elementsByName", &Document::elementsByName);
                kk::PutMethod<Document,void,kk::CString,kk::CString>(ctx, -1, "set", &Document::set);
                kk::PutMethod<Document,void>(ctx, -1, "startTransaction", &Document::startTransaction);
                kk::PutStrongMethod<Document,ArrayBuffer>(ctx, -1, "commitTransaction", &Document::commitTransaction);
                kk::PutMethod<Document,void,ArrayBuffer *>(ctx, -1, "setTransaction", &Document::setTransaction);
                kk::PutMethod<Document,String>(ctx, -1, "toString", &Document::toString);
                
            });
            
        });
        
        
    }
    
}
