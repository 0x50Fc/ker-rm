//
//  BlockElement.h
//  Ker
//
//  Created by zhanghailong on 2019/1/22.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef kk_BlockElement_h
#define kk_BlockElement_h


#include <document/element.h>

namespace kk {
    
    class BlockElement : public Element {
    public:
        
        BlockElement(Document * document,CString name, ElementKey elementId);

        Ker_CLASS(BlockElement,Element,"BlockElement")
        
        static void Openlib();
        
    };
    
}

#endif /* BlockElement_h */
