//
//  element.cc
//  Ker
//
//  Created by zhanghailong on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "element.h"

namespace kk  {
    
    ElementEvent::ElementEvent(Element * element):Event(element),_cancelBubble(false) {
        
    }
    
    Element * ElementEvent::element() {
        return _target;
    }
    
    kk::Boolean ElementEvent::cancelBubble() {
        return _cancelBubble;
    }
    
    void ElementEvent::setCancelBubble(kk::Boolean v) {
        _cancelBubble = v;
    }
    
    Object * ElementEvent::dataSet() {
        return _dataSet;
    }
    
    void ElementEvent::setDataSet(Object * object) {
        _dataSet = object;
    }
    
    void ElementEvent::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushClass<ElementEvent,Element *>(ctx, [](duk_context * ctx)->void{
                
                kk::PutProperty<ElementEvent,Element *>(ctx, -1, "element", &ElementEvent::element);
                kk::PutProperty<ElementEvent,kk::Boolean>(ctx, -1, "cancelBubble", &ElementEvent::cancelBubble,&ElementEvent::setCancelBubble);
                kk::PutProperty<ElementEvent,kk::Object *>(ctx, -1, "dataSet", &ElementEvent::dataSet,&ElementEvent::setDataSet);
            });
            
        });
        
    }
    
    
    Element::Element(Document * document,CString name, ElementKey elementId)
        :EventEmitter(), _document(document),_name(name),_elementId(elementId),_depth(0) {
        
    }
    
    Element::~Element() {
        
    }
    
    ElementKey Element::elementId() {
        return _elementId;
    }
    
    CString Element::name() {
        return _name.c_str();
    }
    
    Document * Element::document() {
        return (Document *) _document.get();
    }
    
    
    Element * Element::firstChild() {
        return (Element *) _firstChild.get();
    }
    
    Element * Element::lastChild() {
        return (Element *) _lastChild.get();
    }
    
    Element * Element::nextSibling() {
        return (Element *) _nextSibling.get();
    }
    
    Element * Element::prevSibling() {
        return (Element *) _prevSibling.get();
    }
    
    Element * Element::parent() {
        return (Element *) _parent.get();
    }
    
    void Element::append(Element * element) {
        
        if(element == nullptr) {
            return;
        }
        
        kk::Strong<Element> e(element);
        
        element->remove();
        
        Element * lastChild = this->lastChild();
        
        if(lastChild) {
            lastChild->_nextSibling = element;
            element->_prevSibling = lastChild;
            _lastChild = element;
            element->_parent = this;
        } else {
            _firstChild = _lastChild = element;
            element->_parent = this;
        }
        
        {
            Document * v = document();
            
            if(v != nullptr) {
                
                kk::Strong<DocumentAppendElementCommand> cmd = new DocumentAppendElementCommand();
                cmd->elementId = _elementId;
                if(element != nullptr) {
                    cmd->subelementId = element->elementId();
                }
                v->execCommand(cmd);
                
            }
            
        }
        
        onDidAddChildren(element);
        
    }
    
    void Element::before(Element * element) {
        
        if(element == nullptr) {
            return;
        }
        
        kk::Strong<Element> e(element);
        
        element->remove();
        
        Element * prevSibling = this->prevSibling();
        Element * parent = this->parent();
        
        if(prevSibling) {
            prevSibling->_nextSibling = element;
            element->_prevSibling = prevSibling;
            element->_nextSibling = this;
            element->_parent = parent;
            _prevSibling = element;
        } else if(parent) {
            element->_nextSibling = this;
            element->_parent = parent;
            _prevSibling = element;
            parent->_firstChild = element;
        }
        
        if(parent) {
            
            {
                Document * v = document();
                
                if(v != nullptr) {
                    
                    kk::Strong<DocumentBeforeElementCommand> cmd = new DocumentBeforeElementCommand();
                    cmd->elementId = _elementId;
                    if(element != nullptr) {
                        cmd->siblingId = element->elementId();
                    }
                    v->execCommand(cmd);
                    
                }
                
            }
            
            parent->onDidAddChildren(element);
        }
    }
    
    void Element::after(Element * element) {
        
        if(element == nullptr){
            return;
        }
        
        kk::Strong<Element> e(element);
        
        element->remove();
        
        Element * nextSibling = this->nextSibling();
        Element * parent = this->parent();
        
        if(nextSibling) {
            nextSibling->_prevSibling = element;
            element->_nextSibling = nextSibling;
            element->_prevSibling = this;
            element->_parent = parent;
            _nextSibling = element;
        } else if(parent) {
            element->_prevSibling = this;
            element->_parent = parent;
            _nextSibling = element;
            parent->_lastChild = element;
        }
        
        if(parent) {
            
            {
                Document * v = document();
                
                if(v != nullptr) {
                    
                    kk::Strong<DocumentAfterElementCommand> cmd = new DocumentAfterElementCommand();
                    cmd->elementId = _elementId;
                    if(element != nullptr) {
                        cmd->siblingId = element->elementId();
                    }
                    v->execCommand(cmd);
                    
                    
                }
                
            }
            
            parent->onDidAddChildren(element);
        }
    }
    
    void Element::remove() {
        
        kk::Strong<Element> e(this);
        
        Element * prevSibling = this->prevSibling();
        Element * nextSibling = this->nextSibling();
        Element * parent = this->parent();
        
        if(prevSibling) {
            parent->onWillRemoveChildren(this);
            prevSibling->_nextSibling = nextSibling;
            if(nextSibling) {
                nextSibling->_prevSibling = prevSibling;
            } else {
                parent->_lastChild = prevSibling;
            }
        } else if(parent) {
            parent->onWillRemoveChildren(this);
            parent->_firstChild = nextSibling;
            if(nextSibling) {
                nextSibling->_prevSibling = (Element *) nullptr;
            } else {
                parent->_lastChild = (Element *) nullptr;
            }
        }
        
        _parent = nullptr;
        _prevSibling = nullptr;
        _nextSibling = nullptr;
        
        if(parent) {
            {
                Document * v = document();
                
                if(v != nullptr) {
                    
                    kk::Strong<DocumentRemoveElementCommand> cmd = new DocumentRemoveElementCommand();
                    cmd->elementId = _elementId;
                    v->execCommand(cmd);
                    
                }
                
            }
        }
    }
    
    void Element::appendTo(Element * element) {
        if(element != nullptr) {
            element->append(this);
        }
    }
    
    void Element::beforeTo(Element * element) {
        if(element != nullptr) {
            element->before(this);
        }
    }
    
    void Element::afterTo(Element * element) {
        if(element != nullptr) {
            element->after(this);
        }
    }
    
    void Element::onDidAddChildren(Element * element) {
        element->onDidAddToParent(this);
        element->_depth = _depth + 1;
    }
    
    void Element::onWillRemoveChildren(Element * element) {
        element->onWillRemoveFromParent(this);
        element->_depth = 0;
    }
    
    void Element::onDidAddToParent(Element * element) {
        
    }
    
    void Element::onWillRemoveFromParent(Element * element) {
        
    }
    
    void Element::set(ElementKey key,CString value) {
        Document * v = document();
        if(v == nullptr) {
            return;
        }
        set(v->key(key),value);
    }
    
    CString Element::get(CString key) {
        std::map<String,String>::iterator i = _attributes.find(key);
        if(i != _attributes.end()) {
            String &v = i->second;
            return v.c_str();
        }
        return nullptr;
    }
    
    CString Element::get(ElementKey key) {
        Document * v = document();
        if(v == nullptr) {
            return nullptr;
        }
        return get(v->key(key));
    }
    
    void Element::set(CString key,CString value) {
        
        if(value == nullptr) {
            
            std::map<String,String>::iterator i = _attributes.find(key);
            
            if(i != _attributes.end()) {
                _attributes.erase(i);
            }
            
        } else {
            _attributes[key] = value;
        }
        
        {
            Document * v = document();
            
            if(v != nullptr) {
                
                kk::Strong<DocumentSetElementCommand> cmd = new DocumentSetElementCommand();
                cmd->elementId = _elementId;
                cmd->key = v->elementKey(key);
                
                if(value != nullptr) {
                    cmd->value = value;
                }
                
                v->execCommand(cmd);
                
            }
            
        }
    }
    
    std::map<String,String> & Element::attributes() {
        return _attributes;
    }
    
    void Element::dispatchEvent(CString name,Event * event) {
        
        if(name == nullptr || event == nullptr) {
            return;
        }
        
        Any & r = event->returnValue();
    
        Element * p = lastChild();
        
        while(p) {
            
            p->dispatchEvent(name, event);
            
            if(r.type == TypeBoolean && r.booleanValue == false) {
                return ;
            }
            
            p = p->prevSibling();
        }
        
    }
    
    void Element::emit(CString name,Event * event) {
        
        ElementEvent * e = dynamic_cast<ElementEvent *>(event);
        
        EventEmitter::emit(name, event);
        
        if(e && !e->cancelBubble()) {
            Element * p = parent();
            if(p) {
                p->emit(name, event);
            } else  {
                Document * doc = document();
                if(doc) {
                    doc->emit(name,event);
                }
            }
        }
        
    }
    
    kk::Boolean Element::hasBubble(CString name) {
        
        if(EventEmitter::has(name)) {
            return true;
        }
        
        Element * p = parent();
        
        if(p != nullptr) {
            return p->hasBubble(name);
        }
        
        Document * doc = document();
        
        if(doc != nullptr) {
            return doc->has(name);
        }
        
        return false;
    }
    
    kk::Object * Element::object(CString key) {
        
        auto i = _objects.find(key);
        
        if(i != _objects.end()) {
            return i->second.get();
        }
        
        return nullptr;
    }
    
    void Element::setObject(CString key,kk::Object * object) {
        if(object == nullptr) {
            auto i = _objects.find(key);
            if(i != _objects.end()) {
                _objects.erase(i);
            }
        } else {
            _objects[key] = object;
        }
    }
    
    Strong<TObject<kk::String,kk::String>> Element::attributesObject() {
        Strong<TObject<kk::String,kk::String>> v = new TObject<kk::String,kk::String>(_attributes);
        return v;
    }
    
    kk::Strong<Object> Element::dataSet() {
        kk::Strong<Object> v = new TObject<kk::String, kk::String>();
        TObject<kk::String, kk::String> * object = v;
        
        auto i = _attributes.begin();
        auto e = _attributes.end();
        
        while(i != e) {
            if(kk::CStringHasPrefix(i->first.c_str(), "data-")) {
                (*object)[i->first.substr(5)] = i->second;
            }
            i ++;
        }
        
        return v;
    }
    
    String Element::toString() {
        String v;
        
        for(int i=0;i<_depth;i++) {
            v.append("\t");
        }
        
        v.append("<").append(_name);
        
        {
            auto i = _attributes.begin();
            auto e = _attributes.end();
            
            while(i != e) {
                
                if(!kk::CStringHasPrefix(i->first.c_str(), "#")) {
                    v.append(" ").append(i->first).append("=\"").append(i->second).append("\"");
                }
                
                i ++;
            }
        }
        
        v.append(">");
        
        Element * e = firstChild();
        
        if(e) {
            
            while(e) {
                
                v.append("\n");
                
                kk::String s = e->toString();
                
                v.append(s);
                
                e = e->nextSibling();
                
            }
            
            v.append("\n");
            
            for(int i=0;i<_depth;i++) {
                v.append("\t");
            }
            
        } else {
            CString vv = get("#text");
            if(vv) {
                v.append(vv);
            }
        }
        
        v.append("</").append(_name).append(">");
        
        return v;
    }
    
    void Element::recycle() {
        off();
    }
    
    void Element::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushInterface<Element>(ctx, [](duk_context * ctx)->void{
                
                kk::PutProperty<Element,ElementKey>(ctx, -1, "elementId", &Element::elementId);
                kk::PutProperty<Element,kk::CString>(ctx, -1, "name", &Element::name);
                kk::PutProperty<Element,kk::Document *>(ctx, -1, "document", &Element::document);
                kk::PutProperty<Element,kk::Element *>(ctx, -1, "firstChild", &Element::firstChild);
                kk::PutProperty<Element,kk::Element *>(ctx, -1, "lastChild", &Element::lastChild);
                kk::PutProperty<Element,kk::Element *>(ctx, -1, "nextSibling", &Element::nextSibling);
                kk::PutProperty<Element,kk::Element *>(ctx, -1, "prevSibling", &Element::prevSibling);
                kk::PutProperty<Element,kk::Element *>(ctx, -1, "parent", &Element::parent);
                kk::PutMethod<Element,void,kk::Element *>(ctx, -1, "append", &Element::append);
                kk::PutMethod<Element,void,kk::Element *>(ctx, -1, "appendTo", &Element::appendTo);
                kk::PutMethod<Element,void,kk::Element *>(ctx, -1, "before", &Element::before);
                kk::PutMethod<Element,void,kk::Element *>(ctx, -1, "beforeTo", &Element::beforeTo);
                kk::PutMethod<Element,void,kk::Element *>(ctx, -1, "after", &Element::after);
                kk::PutMethod<Element,void,kk::Element *>(ctx, -1, "afterTo", &Element::afterTo);
                kk::PutMethod<Element,void>(ctx, -1, "remove", &Element::remove);
                kk::PutMethod<Element,void,kk::CString,kk::CString>(ctx, -1, "set", &Element::set);
                kk::PutMethod<Element,kk::CString,kk::CString>(ctx, -1, "get", &Element::get);
                kk::PutStrongMethod<Element,kk::TObject<kk::String,kk::String>>(ctx, -1, "attributes", &Element::attributesObject);
                kk::PutMethod<Element,void,kk::CString,kk::Object *>(ctx, -1, "setObject", &Element::setObject);
                kk::PutMethod<Element,kk::Object *,kk::CString>(ctx, -1, "object", &Element::object);
                kk::PutMethod<Element,void,kk::CString,Event *>(ctx, -1, "dispatchEvent", &Element::dispatchEvent);
                kk::PutMethod<Element,kk::String>(ctx, -1, "toString", &Element::toString);
                kk::PutMethod<Element,void>(ctx, -1, "recycle", &Element::recycle);
                
                
            });
            
        });
        
        
    }
    
}
