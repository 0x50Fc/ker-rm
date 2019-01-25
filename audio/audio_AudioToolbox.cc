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
            
            AudioQueueInputCallback(AudioCodec * codec,OutputStream * output):_codec(codec),_output(output),_cancel(false){
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
            
            virtual void cancel() {
                _cancel = true;
            }
            
            virtual kk::Boolean isCanceled() {
                return _cancel;
            }
            
        protected:
            kk::Strong<OutputStream> _output;
            kk::Strong<AudioCodec> _codec;
            void * _data;
            size_t _size;
            kk::Boolean _cancel;
        };
        
        static void AudioQueueInput_AudioQueueInputCallback(
                                                      void *                          inUserData,
                                                      AudioQueueRef                   inAQ,
                                                      AudioQueueBufferRef             inBuffer,
                                                      const AudioTimeStamp *          inStartTime,
                                                      UInt32                          inNumberPacketDescriptions,
                                                      const AudioStreamPacketDescription *inPacketDescs){
            
            AudioQueueInputCallback * cb = (AudioQueueInputCallback *) inUserData;
            
            if(cb->isCanceled()) {
                return;
            }

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
            
            kk::Strong<AudioQueueInput> strong = this;
            
            _queue->async([strong,this,doError,doDone]()->void{
                
                AudioQueueInputCallback cb(_codec,_output);
                
                AudioStreamBasicDescription format = {0};
                
                format.mFormatID = kAudioFormatLinearPCM;
                format.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
                format.mChannelsPerFrame = 1;
                format.mBitsPerChannel = 16;
                format.mFramesPerPacket = 1;
                format.mBytesPerPacket =  2;
                format.mBytesPerFrame = 2;
                format.mSampleRate = _codec->samplingRate();
                
                AudioQueueRef queue = nullptr;
                
                OSStatus s = AudioQueueNewInput(&format, AudioQueueInput_AudioQueueInputCallback, &cb, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode, 0, &queue);
                
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
                    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.017, true);
                }
                
                cb.cancel();
                
                AudioQueueStop(queue, true);
                
                CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.017, true);
                
                AudioQueueDispose(queue, true);
                
                CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.017, true);
                
                doDone();
             
                
            });
            
        }
        
        class AudioQueueOutputCallback {
        public:
            
            AudioQueueOutputCallback(AudioCodec * codec,InputStream * input):_codec(codec),_input(input),_cancel(false),_length(0){
                _size = codec->packageBytes();
                _data = (kk::Ubyte *) malloc(_size);
            }
            
            virtual ~AudioQueueOutputCallback() {
                free(_data);
            }
            
            virtual kk::Boolean read(void * data) {
                
                ssize_t n = _size - _length;
                
                n = _input->read(_data + _length, n);
                
                if(n == -1) {
                    return false;
                }
                
                _length += n;
                
                if(_length != _size) {
                    return false;
                }
                
                _codec->decode(_data, _length, data);
                _length = 0;
                
                return true;
            }
            
            virtual void cancel() {
                _cancel = true;
            }
            
            virtual kk::Boolean isCanceled() {
                return _cancel;
            }
            
            virtual size_t size() {
                return _size;
            }
            
        protected:
            kk::Strong<InputStream> _input;
            kk::Strong<AudioCodec> _codec;
            kk::Ubyte * _data;
            size_t _size;
            size_t _length;
            kk::Boolean _cancel;
        };
        
        static void AudioQueueOutput_AudioQueueOutputCallback(
                                                      void *                  inUserData,
                                                      AudioQueueRef           inAQ,
                                                      AudioQueueBufferRef     inBuffer){
            
            AudioQueueOutputCallback * cb = (AudioQueueOutputCallback *) inUserData;
            
            inBuffer->mAudioDataByteSize = (UInt32) cb->size();
            
            if(cb->read(inBuffer->mAudioData)) {
                AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
            } else {
                cb->cancel();
            }
            
        }
        
        void AudioQueueOutput::run() {
            
            kk::Strong<DispatchQueue> queue = getCurrentDispatchQueue();
            kk::Weak<AudioQueueOutput> v = this;
            
            std::function<void(kk::CString)> doError = [v,queue](kk::CString errmsg)->void{
                AudioQueueOutput * input = v.operator->();
                if(input != nullptr) {
                    kk::String s = errmsg;
                    queue->async([v,s]()->void{
                        AudioQueueOutput * input = v.operator->();
                        if(input != nullptr) {
                            input->doError(s.c_str());
                        }
                    });
                }
            };
            
            std::function<void()> doDone = [v,queue]()->void{
                AudioQueueOutput * input = v.operator->();
                if(input != nullptr) {
                    queue->async([v]()->void{
                        AudioQueueOutput * input = v.operator->();
                        if(input != nullptr) {
                            input->doDone();
                        }
                    });
                }
            };
            
            kk::Strong<AudioQueueOutput> strong = this;
            
            _queue->async([strong,this,doError,doDone]()->void{
                
                AudioQueueOutputCallback cb(_codec,_input);
                
                AudioStreamBasicDescription format = {0};
                
                format.mFormatID = kAudioFormatLinearPCM;
                format.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
                format.mChannelsPerFrame = 1;
                format.mBitsPerChannel = 16;
                format.mFramesPerPacket = 1;
                format.mBytesPerPacket =  2;
                format.mBytesPerFrame = 2;
                format.mSampleRate = _codec->samplingRate();
                
                AudioQueueRef queue = nullptr;
                
                OSStatus s = AudioQueueNewOutput(&format, AudioQueueOutput_AudioQueueOutputCallback, &cb, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode, 0, &queue);
                
                if(s != noErr) {
                    doError("[AudioQueueOutput::doStart] [AudioQueueNewInput]");
                    return;
                }
                
                kk::Uint bufferSize = _codec->frameBytes();
                
                AudioQueueBufferRef buffers[AUDIO_RECORD_BUFFER_SIZE];
                
                for(int i=0;i<AUDIO_RECORD_BUFFER_SIZE;i++){
                    AudioQueueAllocateBuffer(queue,bufferSize,&buffers[i]);
                }
                
                for(int i=0;i<AUDIO_RECORD_BUFFER_SIZE;i++){
                    
                    if(cb.read(buffers[i]->mAudioData)) {
                        AudioQueueEnqueueBuffer(queue, buffers[i], 0, NULL);
                    } else {
                        break;
                    }
                    
                }
                
                AudioQueuePrime(queue, 0, NULL);
                
                AudioQueueSetParameter(queue, kAudioQueueParam_Volume, 1.0);
                
                kk::Boolean isStart = true;
                
                AudioQueueStart(queue, NULL);
                
                while(this->_state != AudioQueueStateError
                      && this->_state != AudioQueueStateStop){
                    if(cb.isCanceled()) {
                        this->_state = AudioQueueStateStop;
                        break;
                    }
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
                    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.017, false);
                }
                
                cb.cancel();
                
                AudioQueueStop(queue, true);
                
                for(int i=0;i<AUDIO_RECORD_BUFFER_SIZE;i++){
                    
                    AudioQueueFreeBuffer(queue,buffers[i]);
                    
                }
                
                
                AudioQueueDispose(queue, true);
                
                doDone();
                
                
            });
            
        }
        
    }
    
}
