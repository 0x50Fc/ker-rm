//
//  KerURLProtocol.h
//  Ker
//
//  Created by hailong11 on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>

@interface KerURLProtocol : NSURLProtocol

+(void) openlibs;

+(void) installWKWebViewConfiguration:(WKWebViewConfiguration *) confirguration;

@end
