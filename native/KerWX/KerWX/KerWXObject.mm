//
//  KerWXObject.mm
//  KerWX
//
//  Created by hailong11 on 2018/11/15.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject.h"
#import <Ker/Ker.h>
#import "KerWXObject+Location.h"
#import "KerWXObject+Http.h"

#include <core/jit.h>
#include <ui/page.h>

@implementation KerWXObject

+(void) openlib {

    kk::addOpenlib([](duk_context * ctx)->void{
        
        kk::objc::PushInterface(ctx, [KerWXObject class]);
        kk::objc::PushProtocol(ctx, @protocol(KerWXRequestTask));
        
    });
    
    kk::ui::addPageOpenlib([](duk_context * ctx,kk::ui::Page * page)->void{
        
        
        @autoreleasepool {
            
            KerWXObject * object = [[KerWXObject alloc] init];
            
            kk::Any native((__bridge kk::Native *)object);
            
            page->addLibrary("wx", native);
            
            
        }
        
    });
    
    kk::ui::addAppOpenlib([](duk_context * ctx,kk::ui::App * app)->void{
        
        
        @autoreleasepool {
            
            KerWXObject * object = [[KerWXObject alloc] init];
            
            kk::Strong<kk::NativeObject> native = new kk::NativeObject((__bridge kk::Native *)object);
            
            kk::PushObject(ctx, native.get());
            
            duk_put_global_string(ctx, "wx");
            
        }
        
    });
    
}

-(void) dealloc {
    kk::Log("[KerWXObject] [dealloc]");
}

@end
