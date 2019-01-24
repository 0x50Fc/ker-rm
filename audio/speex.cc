//
//  speex.c
//  Ker
//
//  Created by zhanghailong on 2019/1/24.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "speex.h"
#include "speex/speex_header.h"

namespace kk {
    
    namespace audio {
        
        static kk::Uint SpeexCodecBitSizes[] = {10,15,20,20,28,28,38,38,46,46};

        SpeexCodec::SpeexCodec()
        :_encodeState(nullptr),_decodeState(nullptr),_preprocessState(nullptr),_bits({0}),_frameSize(0),_frameBytes(0),_samplingRate(0),_quality(0),_packageBytes(0) {
            
            _quality = 8;
            _packageBytes = SpeexCodecBitSizes[_quality - 1];
            
            _encodeState = speex_encoder_init(& speex_wb_mode);
            _decodeState = speex_decoder_init(& speex_wb_mode);
            
            speex_bits_init(& _bits);
            
            int b = 1;
            
            speex_encoder_ctl(_encodeState,SPEEX_GET_FRAME_SIZE,&_frameSize);
            speex_encoder_ctl(_encodeState,SPEEX_SET_QUALITY,&_quality);
            speex_encoder_ctl(_encodeState,SPEEX_GET_SAMPLING_RATE,&_samplingRate);
            speex_encoder_ctl(_encodeState,SPEEX_SET_DTX,&b);
            speex_encoder_ctl(_encodeState,SPEEX_SET_VAD,&b);
            
            _preprocessState = speex_preprocess_state_init(_frameSize,_samplingRate);
            
            speex_preprocess_ctl(_preprocessState, SPEEX_PREPROCESS_SET_DENOISE, &b);
            
            _frameBytes = _frameSize * sizeof(spx_int16_t);
            
        }
        
        SpeexCodec::~SpeexCodec() {
            
            speex_bits_destroy(& _bits);
            
            if(_encodeState){
                speex_encoder_destroy(_encodeState);
            }
            
            if(_decodeState){
                speex_decoder_destroy(_decodeState);
            }
            
            if(_preprocessState){
                speex_preprocess_state_destroy(_preprocessState);
            }
            
        }
        
        kk::Uint SpeexCodec::frameSize() {
            return _frameSize;
        }
        
        kk::Uint SpeexCodec::frameBytes() {
            return _frameBytes;
        }
        
        kk::Uint SpeexCodec::samplingRate() {
            return _samplingRate;
        }
        
        void SpeexCodec::setSamplingRate(kk::Uint v) {
            speex_decoder_ctl(_decodeState, SPEEX_SET_SAMPLING_RATE, & v);
            _samplingRate = v;
        }
        
        void SpeexCodec::setQuality(kk::Uint v) {
            _quality = v;
            if(_quality < 1){
                _quality = 1;
            }
            if(_quality > 10){
                _quality = 10;
            }
            _packageBytes = SpeexCodecBitSizes[_quality - 1];
            speex_encoder_ctl(_encodeState,SPEEX_SET_QUALITY,&_samplingRate);
        }
        
        kk::Uint SpeexCodec::quality() {
            return _quality;
        }
        
        kk::Uint SpeexCodec::packageBytes() {
            return _packageBytes;
        }
        
        ssize_t SpeexCodec::encode(const void *inBytes , void * outBytes) {
            
            spx_int16_t * enc = (spx_int16_t *) inBytes;
            
            if(speex_preprocess_run(_preprocessState, enc)) {
                speex_bits_reset(&_bits);
                speex_encode_int(_encodeState, enc, &_bits);
                return (ssize_t) speex_bits_write(&_bits, (char *) outBytes, (int) _frameBytes);
            }
            
            return 0;
        }
        
        ssize_t SpeexCodec::decode(const void *inBytes , size_t length, void * outBytes) {
            
            int rs = 0;
            
            speex_bits_reset(&_bits);
            speex_bits_read_from(&_bits, (char *) inBytes, (int) length);
            rs = speex_decode_int(_decodeState, &_bits, (spx_int16_t *)outBytes);
            
            return rs ==0 ? _frameBytes : 0;
            
        }
        
        SpeexFileOutputStream::SpeexFileOutputStream(OutputStream * output,SpeexCodec * codec):_codec(codec),_output(output) {
            
            if(_codec == nullptr) {
                _codec = new SpeexCodec();
            }
            
            SpeexHeader header;
            
            speex_init_header(&header, _codec->samplingRate(), 1, &speex_wb_mode);
            
            header.vbr = 0;
            header.bitrate = 16;
            header.frame_size = _codec->frameSize();
            header.frames_per_packet = _codec->packageBytes();
            header.reserved1 = _codec->quality();
            
            int bytes = 0;
            void * data = speex_header_to_packet(&header, & bytes);
            
            if(_output->write(data, bytes) != bytes) {
                _output = nullptr;
            }
            
            speex_header_free(data);
            
        }
        
        SpeexCodec * SpeexFileOutputStream::codec() {
            return _codec;
        }
        
        void SpeexFileOutputStream::close() {
            if(_output != nullptr) {
                _output->close();
                _output = nullptr;
            }
        }
        
        ssize_t SpeexFileOutputStream::write(const void * data, size_t length)  {
            if(_output == nullptr) {
                return -1;
            }
            return _output->write(data, length);
        }
        
        SpeexFileInputStream::SpeexFileInputStream(InputStream * input):_codec(nullptr),_input(input) {
            
            char data[sizeof(SpeexHeader)];
            
            if(sizeof(data) != _input->read(data, sizeof(data))){
                _input = nullptr;
            }
            
            SpeexHeader * header = speex_packet_to_header(data, sizeof(data));
            
            if(!header){
                _input = nullptr;
            }
            
            _codec = new SpeexCodec();
            
            _codec->setSamplingRate(header->rate);
            
            if(header->reserved1){
                _codec->setQuality(header->reserved1);
            }
        
            speex_header_free(header);
            
        }
        
        void SpeexFileInputStream::close() {
            if(_input != nullptr) {
                _input->close();
                _input = nullptr;
            }
        }
        
        ssize_t SpeexFileInputStream::read(void * data, size_t length) {
            if(_input == nullptr) {
                return -1;
            }
            return _input->read(data, length);
        }
        
        SpeexCodec * SpeexFileInputStream::codec() {
            return _codec;
        }
        
        void SpeexCodec::Openlib() {
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushClass<SpeexCodec>(ctx, [](duk_context * ctx)->void{
                   
                    kk::PutProperty(ctx, -1, "quality", &SpeexCodec::quality, &SpeexCodec::setQuality);
                    
                });
                
                kk::PushClass<SpeexFileInputStream,InputStream *>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutProperty(ctx, -1, "codec", &SpeexFileInputStream::codec);
                    
                });
                
                kk::PushClass<SpeexFileOutputStream,OutputStream *,SpeexCodec *>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutProperty(ctx, -1, "codec", &SpeexFileOutputStream::codec);
                    
                });
                
            });
        }
        
    }
    
}

