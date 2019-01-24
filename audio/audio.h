//
//  audio.h
//  Ker
//
//  Created by zhanghailong on 2019/1/24.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef audio_audio_h
#define audio_audio_h

#include <core/kk.h>
#include <core/event.h>
#include <core/stream.h>

namespace kk {
    
    namespace audio {
        
        class AudioCodec : public Object {
        public:
            virtual kk::Uint frameSize() = 0; // 帧大小
            virtual kk::Uint frameBytes() = 0; // 帧字节
            virtual kk::Uint samplingRate() = 0; // 码率
            virtual kk::Uint packageBytes() = 0; // 打包字节
            virtual ssize_t encode(const void *inBytes , void * outBytes) = 0;
            virtual ssize_t decode(const void *inBytes , size_t length, void * outBytes) = 0;
        };
        
        enum {
            AudioQueueStateNone,
            AudioQueueStateStart,
            AudioQueueStateStop,
            AudioQueueStatePause,
            AudioQueueStateError
        };
        
        typedef kk::Uint AudioQueueState;
        
        class AudioQueue : public EventEmitter {
        public:
            AudioQueue(AudioCodec * codec);
            virtual ~AudioQueue();
            virtual AudioQueueState state();
            virtual AudioCodec * codec();
            virtual void start();
            virtual void stop();
            virtual void resume();
            virtual void pause();
            
            Ker_CLASS(AudioQueue, EventEmitter, "AudioQueue");
            
        protected:
            virtual void run();
            virtual void doError(kk::CString errmsg);
            virtual void doDone();
            AudioQueueState _state;
            kk::Strong<DispatchQueue> _queue;
            kk::Strong<AudioCodec> _codec;
        };
 
        typedef kk::Uint AudioSessionCategory;

        class AudioQueueInput : public AudioQueue {
        public:
            AudioQueueInput(AudioCodec * codec,OutputStream * output);
            
            virtual OutputStream * output();
            
            
            Ker_CLASS(AudioQueueInput, AudioQueue, "AudioQueueInput");
            
        protected:
            virtual void run();
            kk::Strong<OutputStream> _output;
        };
        
        enum {
            AudioSessionCategoryAmbient,
            AudioSessionCategorySoloAmbient,
            AudioSessionCategoryPlayback,
            AudioSessionCategoryRecord,
            AudioSessionCategoryPlayAndRecord,
        };
        
        
        class Audio : public Object {
        public:
            
            Ker_CLASS(Audio, Object, "Audio");
            
            static void startSession(AudioSessionCategory category,std::function<void(kk::CString)> && func);
            
            static void startSession(AudioSessionCategory category,JSObject * func);
            static void Openlib();
        };
        
        
        
        
    }
    
}

#endif /* audio_h */
