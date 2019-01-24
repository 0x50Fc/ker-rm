//
//  speex.h
//  Ker
//
//  Created by zhanghailong on 2019/1/24.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef audio_speex_h
#define audio_speex_h

#include <audio/audio.h>
#include <speex/speex.h>
#include "speex/speex_preprocess.h"
#include "speex/speex_echo.h"

namespace kk {
    
    namespace audio {
        
        class SpeexCodec : public AudioCodec {
        public:
            SpeexCodec();
            virtual ~SpeexCodec();
            virtual kk::Uint frameSize(); // 帧大小
            virtual kk::Uint frameBytes(); // 帧字节
            virtual kk::Uint samplingRate(); // 码率
            virtual void setSamplingRate(kk::Uint v);
            virtual void setQuality(kk::Uint v); // 1~10
            virtual kk::Uint quality();
            virtual kk::Uint packageBytes();
            virtual ssize_t encode(const void *inBytes , void * outBytes);
            virtual ssize_t decode(const void *inBytes , size_t length, void * outBytes);
            
            Ker_CLASS(SpeexCodec,AudioCodec,"SpeexCodec")
            
            static void Openlib();
            
        protected:
            void * _encodeState;
            void * _decodeState;
            SpeexPreprocessState * _preprocessState;
            SpeexBits _bits;
            kk::Uint _frameSize;
            kk::Uint _frameBytes;
            kk::Uint _samplingRate;
            kk::Uint _quality;
            kk::Uint _packageBytes;
        };
        
        class SpeexFileOutputStream : public OutputStream {
        public:
            
            SpeexFileOutputStream(OutputStream * output,SpeexCodec * codec);
            
            virtual void close();
            virtual ssize_t write(const void * data, size_t length) ;
            
            virtual SpeexCodec * codec();
            
            Ker_CLASS(SpeexFileOutputStream,OutputStream,"SpeexFileOutputStream")
            
        protected:
            kk::Strong<OutputStream> _output;
            kk::Strong<SpeexCodec> _codec;
        };
        
        class SpeexFileInputStream : public InputStream {
        public:
            
            SpeexFileInputStream(InputStream * input);
            
            virtual void close();
            virtual ssize_t read(void * data, size_t length) ;
            virtual SpeexCodec * codec();
            
            Ker_CLASS(SpeexFileInputStream,InputStream,"SpeexFileInputStream")
            
        protected:
            kk::Strong<InputStream> _input;
            kk::Strong<SpeexCodec> _codec;
        };
        
        
    }
    
}


#endif /* speex_h */
