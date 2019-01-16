//
//  element.h
//  Ker
//
//  Created by zhanghailong on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef document_element_h
#define document_element_h

#include <document/document.h>

namespace kk {
    
    class ElementEvent : public Event {
    public:
        ElementEvent(Element * element);
        virtual Element * element();
        virtual kk::Boolean cancelBubble();
        virtual void setCancelBubble(kk::Boolean v);
        
        static void Openlib();
        
        Ker_CLASS(ElementEvent,Event,"ElementEvent")
        
    protected:
        kk::Strong<Element> _element;
        kk::Boolean _cancelBubble;
    };
    
    class Element : public EventEmitter {
    public:
        Element(Document * document,CString name, ElementKey elementId);
        virtual ~Element();
        virtual ElementKey elementId();
        virtual CString name();
        virtual Document * document();
        
        virtual Element * firstChild();
        virtual Element * lastChild();
        virtual Element * nextSibling();
        virtual Element * prevSibling();
        virtual Element * parent();
        virtual void append(Element * element);
        virtual void before(Element * element);
        virtual void after(Element * element);
        virtual void remove();
        virtual void appendTo(Element * element);
        virtual void beforeTo(Element * element);
        virtual void afterTo(Element * element);
        
        virtual CString get(ElementKey key);
        virtual CString get(CString key);
        virtual void set(ElementKey key,CString value);
        virtual void set(CString key,CString value);
        virtual std::map<String,String> & attributes();
        virtual Strong<TObject<kk::String,kk::String>> attributesObject();
        
        
        virtual void dispatchEvent(CString name,Event * event);
        
        virtual void emit(CString name,Event * event);
        virtual kk::Boolean hasBubble(CString name);
        
        virtual kk::Object * object(CString key);
        virtual void setObject(CString key,kk::Object * object);
        
        virtual String toString();
        
        Ker_CLASS(Element,EventEmitter,"Element")
        
        static void Openlib();
        
    protected:
        virtual void onDidAddChildren(Element * element);
        virtual void onDidAddToParent(Element * element);
        virtual void onWillRemoveChildren(Element * element);
        virtual void onWillRemoveFromParent(Element * element);

    protected:
        kk::Weak<Document> _document;
        ElementKey _elementId;
        String _name;
        
        kk::Strong<Element> _firstChild;
        kk::Strong<Element> _lastChild;
        kk::Strong<Element> _nextSibling;
        kk::Weak<Element> _prevSibling;
        kk::Weak<Element> _parent;
        
        std::map<String,String> _attributes;
        std::map<String,Strong<Object>> _objects;
        
        kk::Int _depth;
    };
    
    
}

#endif /* element_h */
