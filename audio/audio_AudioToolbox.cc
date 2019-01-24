//
//  audio_AudioToolbox.cc
//  Ker
//
//  Created by zhanghailong on 2019/1/24.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "audio.h"
#include <AudioToolbox/AudioToolbox.h>

namespace kk {
    
    namespace audio {
        
        #define AUDIO_RECORD_BUFFER_SIZE        4
        
        class AudioQueueInputCallback {
        public:
            
            AudioQueueInputCallback(AudioCodec * codec,OutputStream * output):_codec(codec),_output(output){
                _size = codec->packageBytes();
                _data = malloc(codec->packageBytes());
            }
            
            virtual ~AudioQueueInputCallback() {
                free(_data);
            }
            
            virtual void write(const void * data) {
                memset(_data, 0, _size);
                _codec->encode(data, _data);
                _output->write(_data, _size);
            }
            
        protected:
            kk::Strong<OutputStream> _output;
            kk::Strong<AudioCodec> _codec;
            void * _data;
            size_t _size;
        };
        
        static void AudioQueueInput_AudioQueueInputCallback(
                                                      void *                          inUserData,
                                                      AudioQueueRef                   inAQ,
                                                      AudioQueueBufferRef             inBuffer,
                                                      const AudioTimeStamp *          inStartTime,
                                                      UInt32                          inNumberPacketDescriptions,
                                                      const AudioStreamPacketDescription *inPacketDescs){
            
            AudioQueueInputCallback * cb = (AudioQueueInputCallback *) inUserData;
            cb->write(inBuffer->mAudioData);
            inBuffer->mAudioDataByteSize = 0;
            AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
        }
        

        void AudioQueueInput::run() {
            
            kk::Strong<DispatchQueue> queue = getCurrentDispatchQueue();
            kk::Weak<AudioQueueInput> v = this;
            
            std::function<void(kk::CString)> doError = [v,queue](kk::CString errmsg)->void{
                AudioQueueInput * input = v.operator->();
                if(input != nullptr) {
                    kk::String s = errmsg;
                    queue->async([v,s]()->void{
                        AudioQueueInput * input = v.operator->();
                        if(input != nullptr) {
                            input->doError(s.c_str());
                        }
                    });
                }
            };
            
            std::function<void()> doDone = [v,queue]()->void{
                AudioQueueInput * input = v.operator->();
                if(input != nullptr) {
                    queue->async([v]()->void{
                        AudioQueueInput * input = v.operator->();
                        if(input != nullptr) {
                            input->doDone();
                        }
                    });
                }
            };
            
            _queue->async([v,this,doError,doDone]()->void{
                
                if(v.operator->() == this) {
                    
                    AudioStreamBasicDescription format = {0};
                    
                    format.mFormatID = kAudioFormatLinearPCM;
                    format.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
                    format.mChannelsPerFrame = 1;
                    format.mBitsPerChannel = 16;
                    format.mFramesPerPacket = 1;
                    format.mBytesPerPacket =  2;
                    format.mBytesPerFrame = 2;
                    format.mSampleRate = _codec->samplingRate();
                    
                    AudioQueueInputCallback cb(_codec,_output);
                    
                    AudioQueueRef queue = nullptr;
                    
                    OSStatus s = AudioQueueNewInput(&format, AudioQueueInput_AudioQueueInputCallback, &cb, CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &queue);
                    
                    if(s != noErr) {
                        doError("[AudioQueueInput::doStart] [AudioQueueNewInput]");
                        return;
                    }
                    
                    kk::Uint bufferSize = _codec->frameBytes();
                    
                    AudioQueueBufferRef buffers[AUDIO_RECORD_BUFFER_SIZE];
                    
                    for(int i=0;i<AUDIO_RECORD_BUFFER_SIZE;i++){
                        AudioQueueAllocateBuffer(queue,bufferSize,&buffers[i]);
                        AudioQueueEnqueueBuffer(queue, buffers[i], 0, NULL);
                    }
                    
                    AudioQueueFlush(queue);
                    
                    AudioQueueSetParameter(queue, kAudioQueueParam_Volume, 1.0);
                    
                    kk::Boolean isStart = true;
                    
                    AudioQueueStart(queue, NULL);
                    
                    while(this->_state != AudioQueueStateError
                          && this->_state != AudioQueueStateStop){
                        if(this->_state == AudioQueueStateStart) {
                            if(!isStart) {
                                isStart = true;
                                AudioQueueStart(queue, NULL);
                            }
                        } else if(this->_state == AudioQueueStatePause) {
                            if(isStart) {
                                isStart = false;
                                AudioQueuePause(queue);
                            }
                        }
                        CFRunLoopRunInMode(kCFRunLoopCommonModes, 0.017, false);
                    }
                    
                    AudioQueueStop(queue, true);
                    
                    for(int i=0;i<AUDIO_RECORD_BUFFER_SIZE;i++){
                        AudioQueueFreeBuffer(queue,buffers[i]);
                    }
                    
                    AudioQueueDispose(queue, true);
                    
                    doDone();
                    
                }
                
            });
            
        }
        
        
        
    }
    
}
