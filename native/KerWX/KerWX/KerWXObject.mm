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

-(instancetype) initWithBasePath:(NSString *) basePath dataPath:(NSString *) dataPath {
    if((self = [super init])) {
        _basePath = basePath;
        _dataPath = dataPath;
    }
    return self;
}

+(void) openlib {

    KerAddOpenlibInterface([KerWXObject class]);
    KerAddOpenlibProtocol(@protocol(KerWXRequestTask));
    
    KerAddOpenlibFunction func = ^(NSString * basePath,NSString * appkey,KerOpenlibSetLibrary setLibrary) {
        
        NSString * dir = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents/ker/"];
        
        [[NSFileManager defaultManager] createDirectoryAtPath:dir withIntermediateDirectories:YES attributes:nil error:nil];
        
        KerWXObject * object = [[KerWXObject alloc] initWithBasePath:basePath  dataPath:[dir stringByAppendingFormat:@"%@/",appkey]];
        
        setLibrary(@"wx",object);
        
    };
    
    KerAddPageOpenlib(func);
    
    KerAddAppOpenlib(func);
    
}

-(void) dealloc {
    kk::Log("[KerWXObject] [dealloc]");
}

@end
