//
//  ffmpeg_decoder.h
//  SFPlayer
//
//  Created by StyxS on 2020/12/20.
//

#ifndef ffmpeg_decoder_h
#define ffmpeg_decoder_h

#include "decoder_interface.h"
#include "ring_buffer.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

namespace sfplayer {
    class FFmpegDeocder : public IDecoderInterface {
    public:
        FFmpegDeocder();
        // IPlayerElementInterface
        bool Start() override;
        bool Stop() override;
        bool Seek(int64_t  milliseconds) override;
        // 设置当前模块所需的参数
        void TransportParameter(std::shared_ptr<Parameter> p) override;
        
        void PushPacket(std::shared_ptr<MediaPacket> packet) override;
    private:
        AVCodecContext *audio_codec_context_ = NULL;
        AVCodecContext *video_codec_context_ = NULL;
        SwrContext *audio_swr_context_ = NULL;
        SwsContext *video_sws_context_ = NULL;
        AVRational audio_stream_timebase_;
        AVRational video_stream_timebase_;
        
        // seek时音频和视频不能对齐可能导致死锁（音画同步等待audio），这里手动过滤一下
        int64_t seek_pts_ = 0;
        
        std::shared_ptr<std::thread> audio_worker_;
        std::shared_ptr<std::thread> video_worker_;
        RingBuffer<MediaPacket> audio_packet_buffer;
        RingBuffer<MediaPacket> video_packet_buffer;
        
        bool first_audio_frame_ = true;
        
        void DecodeAudio();
        void DecodeVideo();
    };
}

#endif /* ffmpeg_decoder_h */
