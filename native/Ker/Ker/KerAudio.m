//
//  KerAudio.m
//  Ker
//
//  Created by 张海龙 on 2019/1/24.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#import "KerAudio.h"
#import <AVFoundation/AVFoundation.h>
#import "KerUI.h"

@implementation KerAudioStartRecordRes


@end

@interface KerAudio()<AVAudioRecorderDelegate> {

}

@property(nonatomic,strong) AVAudioRecorder * recorder;
@property(nonatomic,strong) id<KerAudioStartRecordObject> recorderObject;

@end

@implementation KerAudio

+(void) openlib {
    KerAddOpenlibInterface([KerAudio class]);
    KerAddAppOpenlib(^(NSString *basePath, NSString *appkey, KerOpenlibSetLibrary setLibrary) {
        setLibrary(@"ker.audio",[[KerAudio alloc] init]);
    });
    
}

@synthesize recorder = _recorder;
@synthesize recorderObject = _recorderObject;

-(void) dealloc {
    
    [_recorder setDelegate:nil];
    [_recorder stop];
    
}

-(void) _startRecord:(id<KerAudioStartRecordObject>) object {
    
    char m[] = "XXXXXXXXXXXX";
    
    mkstemp(m);
    
    NSString * path = [KerUI resolvePath:[NSString stringWithFormat:@"ker-tmp:///KerAudio_AVAudioRecorder_%s.wav",m]];
    
    NSFileManager * fm = [NSFileManager defaultManager];
    
    [fm createFileAtPath:path contents:nil attributes:nil];
    
    NSError * err = nil;
    
    _recorder = [[AVAudioRecorder alloc] initWithURL:[NSURL fileURLWithPath:path]
                                            settings:@{
                                                       AVSampleRateKey:@(8000),
                                                       AVNumberOfChannelsKey:@(1),
                                                       AVLinearPCMBitDepthKey:@(8),
                                                       AVLinearPCMIsFloatKey:@(YES),
                                                       AVFormatIDKey:@(kAudioFormatLinearPCM)
                                                       } error:& err];
    
    if(err != nil) {
        [fm removeItemAtPath:path error:nil];
        _recorder = nil;
        [object fail:[err localizedDescription]];
        [object complete];
        return;
    }
    
    [_recorder setDelegate:self];
    
    if(![_recorder prepareToRecord]) {
        [fm removeItemAtPath:path error:nil];
        _recorder = nil;
        [object fail:@"[prepareToRecord]"];
        [object complete];
        return;
    }
    
    _recorder.meteringEnabled = YES;
    
    if(![_recorder prepareToRecord]) {
        [fm removeItemAtPath:path error:nil];
        _recorder = nil;
        [object fail:@"[record]"];
        [object complete];
        return;
    }
    
    _recorderObject = object;
    
}

-(void) startRecord:(KerJSObject *) jsObject {
    
    id<KerAudioStartRecordObject> object = [jsObject implementProtocol:@protocol(KerAudioStartRecordObject)];
    
    if(object == nil) {
        return;
    }
    
    _recorderObject = nil;
    [_recorder setDelegate:nil];
    [_recorder stop];
    _recorder = nil;
    
    NSError * err = nil;
    
    AVAudioSession * session = [AVAudioSession sharedInstance];
    
    [session setCategory:AVAudioSessionCategoryRecord error:&err];
    
    if(err != nil) {
        [object fail:[err localizedDescription]];
        [object complete];
        return;
    }
    
    [session setActive:YES error:&err];
    
    if(err != nil) {
        [object fail:[err localizedDescription]];
        [object complete];
        return;
    }
    
    __weak KerAudio * v = self;
    
    [session requestRecordPermission:^(BOOL granted) {
        if(granted) {
            [v _startRecord:object];
        } else {
            [object fail:@"请开启录音权限"];
            [object complete];
        }
    }];
    
    
}

-(void) stopRecord {
    [_recorder stop];
}

- (void)audioRecorderDidFinishRecording:(AVAudioRecorder *)recorder successfully:(BOOL)flag {
    if(_recorder == recorder) {
        KerAudioStartRecordRes * res = [[KerAudioStartRecordRes alloc] init];
        res.tempFilePath = [KerUI resolveURI:_recorder.url.path];
        [_recorderObject success:res];
        [_recorderObject complete];
        [_recorder setDelegate:nil];
        _recorder = nil;
        _recorderObject = nil;
    }
}


- (void)audioRecorderEncodeErrorDidOccur:(AVAudioRecorder *)recorder error:(NSError * ) error {
    if(_recorder == recorder) {
        [_recorderObject fail:[error localizedDescription]];
        [_recorderObject complete];
        [_recorder setDelegate:nil];
        [_recorder deleteRecording];
        _recorder = nil;
        _recorderObject = nil;
        
    }
}

@end
