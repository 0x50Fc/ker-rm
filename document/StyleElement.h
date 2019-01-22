//
//  StyleElement.h
//  Ker
//
//  Created by zhanghailong on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef document_StyleElement_h
#define document_StyleElement_h


#include <document/element.h>

namespace kk {
    
    class StyleElement : public Element {
    public:
        
        StyleElement(Document * document,CString name, ElementKey elementId);
        
        virtual std::map<String,String>& style(CString name);
        virtual CString status();
        virtual void setStatus(CString status);
        virtual void addStatus(CString status);
        virtual void removeStatus(CString status);
        virtual Boolean hasStatus(CString status);
        virtual void changedStatus();
        virtual void changedKey(CString key);
        virtual void changedKeys(std::set<String>& keys);
        virtual CString get(CString key);
        virtual CString get(ElementKey key);
        virtual void set(ElementKey key,CString value);
        virtual void set(CString key,CString value);
        virtual void keys(std::set<String>& keys);
        
        Ker_CLASS(StyleElement,Element,"StyleElement")
        
        static void Openlib();
        
        
    protected:
        virtual void onDidAddChildren(Element * element);
        virtual void change(kk::CString key,CString value);
        std::map<String,std::map<String,String>> _styles;
    };
    
}


#endif /* StyleElement_h */
