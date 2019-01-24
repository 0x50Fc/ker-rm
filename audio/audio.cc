//
//  audio.c
//  Ker
//
//  Created by zhanghailong on 2019/1/24.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "audio.h"

namespace kk {

    namespace audio {
        
        AudioQueue::AudioQueue(AudioCodec * codec):_codec(codec),_state(AudioQueueStateNone) {
            
        }
        
        AudioQueue::~AudioQueue() {
            if(_queue != nullptr) {
                if(_state != AudioQueueStateError && _state != AudioQueueStateStop) {
                    _state = AudioQueueStateStop;
                }
                _queue->sync([]()->void{});
            }
        }
        
        void AudioQueue::start() {
            if(_state == AudioQueueStateNone) {
                _queue = createDispatchQueue("AudioQueue");
                _state = AudioQueueStateStart;
                run();
            }
        }
        
        void AudioQueue::stop() {
            
            if(_state != AudioQueueStateStop) {
                _state = AudioQueueStateStop;
            }
        }
        
        void AudioQueue::resume() {
            
            if(_state == AudioQueueStatePause) {
                _state = AudioQueueStateStart;
            }
            
        }
        
        void AudioQueue::pause() {
            
            if(_state == AudioQueueStateStart) {
                _state = AudioQueueStatePause;
            }
            
        }
        
        AudioQueueState AudioQueue::state() {
            return _state;
        }
        
        AudioCodec * AudioQueue::codec() {
            return _codec;
        }
        
        void AudioQueue::doError(kk::CString errmsg) {
            _state = AudioQueueStateError;
            kk::Strong<Event> e = new Event();
            e->setData(new TObject<kk::String, kk::String>({{"errmsg",errmsg}}));
            emit("error", e);
        }
        
        void AudioQueue::doDone() {
            kk::Strong<Event> e = new Event();
            emit("done", e);
        }
        
        void AudioQueue::run() {
            
        }
        
        AudioQueueInput::AudioQueueInput(AudioCodec * codec,OutputStream * output):AudioQueue(codec),_output(output){
            
        }
        
        OutputStream * AudioQueueInput::output() {
            return _output;
        }
        
        void Audio::startSession(AudioSessionCategory category,JSObject * func) {
            if(func != nullptr) {
                kk::Strong<JSObject> v = func;
                startSession(category, [v](kk::CString errmsg)->void{
                    if(errmsg != nullptr) {
                        v->invoke<void,kk::CString>(nullptr, errmsg);
                    } else {
                        v->invoke<void>(nullptr);
                    }
                });
            }
        }
        
        void Audio::Openlib() {
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<Audio>(ctx, [](duk_context * ctx)->void{
                    
                    duk_push_int(ctx, AudioSessionCategoryAmbient);
                    duk_put_prop_string(ctx, -3, "Ambient");
                    
                    duk_push_int(ctx, AudioSessionCategorySoloAmbient);
                    duk_put_prop_string(ctx, -3, "SoloAmbient");
                    
                    duk_push_int(ctx, AudioSessionCategoryPlayback);
                    duk_put_prop_string(ctx, -3, "Playback");
                    
                    duk_push_int(ctx, AudioSessionCategoryRecord);
                    duk_put_prop_string(ctx, -3, "Record");
                    
                    duk_push_int(ctx, AudioSessionCategoryPlayAndRecord);
                    duk_put_prop_string(ctx, -3, "PlayAndRecord");
                    
                    kk::PushFunction<void,AudioSessionCategory,JSObject *>(ctx, Audio::startSession);
                    duk_put_prop_string(ctx, -3, "startSession");
                    
                });
                
                kk::PushInterface<AudioQueue>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutMethod(ctx,-1,"start",&AudioQueue::start);
                    kk::PutMethod(ctx,-1,"stop",&AudioQueue::stop);
                    kk::PutMethod(ctx,-1,"pause",&AudioQueue::pause);
                    kk::PutMethod(ctx,-1,"resume",&AudioQueue::resume);
                    kk::PutProperty(ctx, -1, "state", &AudioQueue::state);
                    kk::PutProperty(ctx, -1, "codec", &AudioQueue::codec);
                    
                });
                
                kk::PushClass<AudioQueueInput,AudioCodec*,OutputStream *>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutMethod(ctx,-1,"output",&AudioQueueInput::output);
                    
                });
            });
            
        }
        
    }
    
}

