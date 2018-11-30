//
//  KerWXObject+Analytics.h
//  KerWX
//
//  Created by hailong11 on 2018/11/29.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWXObject.h>


@interface KerWXObject (Analytics)

-(void) reportAnalytics:(NSString *) name object:(KerJSObject *) object;

@end

