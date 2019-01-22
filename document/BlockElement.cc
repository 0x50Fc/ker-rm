//
//  BlockElement.c
//  Ker
//
//  Created by zhanghailong on 2019/1/22.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "BlockElement.h"


namespace kk {
    
    BlockElement::BlockElement(Document * document,CString name, ElementKey elementId):Element(document,name,elementId){
        
    }
    
    static Element * BlockElementCreate(Document * document,kk::CString name,ElementKey elementId) {
        return new BlockElement(document,name,elementId);
    }
    
    void BlockElement::Openlib() {
        
        Document::library("block",BlockElementCreate);
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushInterface<BlockElement>(ctx, [](duk_context * ctx)->void{
                
            });
            
        });
        
        
    }
}

