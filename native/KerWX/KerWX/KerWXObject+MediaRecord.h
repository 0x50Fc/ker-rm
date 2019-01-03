//
//  KerWXObject+MediaRecord.h
//  KerWX
//
//  Created by zuowu on 2018/12/29.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>

@interface RecorderManager : KerWXObject
-(void) start;
@end



@interface KerWXObject (MediaRecord)

-(void) startRecord:(KerJSObject *) object;
-(void) stopRecord:(KerJSObject *) object;

-(KerWXObject *) getRecorderManager;

@end


