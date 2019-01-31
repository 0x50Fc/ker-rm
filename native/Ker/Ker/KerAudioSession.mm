//
//  KerAudioSession.m
//  Ker
//
//  Created by 张海龙 on 2019/1/24.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include <audio/audio.h>
#import <AVFoundation/AVFoundation.h>

namespace kk {

    namespace audio {
        
        void Audio::startSession(AudioSessionCategory category,std::function<void(kk::CString)> && func) {
            
            @autoreleasepool{
                
                AVAudioSession * session = [AVAudioSession sharedInstance];
                
                NSString * v = AVAudioSessionCategoryAmbient;
                
                switch(category) {
                    case AudioSessionCategoryAmbient:
                        v = AVAudioSessionCategoryAmbient;
                        break;
                    case AudioSessionCategorySoloAmbient:
                        v = AVAudioSessionCategorySoloAmbient;
                        break;
                    case AudioSessionCategoryPlayback:
                        v = AVAudioSessionCategoryPlayback;
                        break;
                    case AudioSessionCategoryRecord:
                        v = AVAudioSessionCategoryRecord;
                        break;
                    case AudioSessionCategoryPlayAndRecord:
                        v = AVAudioSessionCategoryPlayAndRecord;
                        break;
                }
                
                NSError * err = nil;
                
                [session setCategory:v error:& err];
                
                if(err != nil) {
                    func("[AudioSession] [setCategory]");
                    return;
                }
                
                [session setActive:YES error:&err];
                
                if(err != nil) {
                    func("[AudioSession] [setActive]");
                    return;
                }
                
                if(category == AudioSessionCategoryRecord || category == AudioSessionCategoryPlayAndRecord) {
                    if(session.recordPermission == AVAudioSessionRecordPermissionGranted) {
                        func(nullptr);
                    } else {
                        std::function<void(kk::CString)> * fn = new std::function<void(kk::CString)>(func);
                        [session requestRecordPermission:^(BOOL granted) {
                            
                            if(granted) {
                                (*fn)(nullptr);
                            } else {
                                (*fn)("请开启录音权限");
                            }
                            
                            delete fn;
                            
                        }];
                    }
                } else {
                    func(nullptr);
                }
                
                
            }
            
        }
    }
    
}

