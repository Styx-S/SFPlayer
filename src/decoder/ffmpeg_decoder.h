//
//  ffmpeg_decoder.h
//  SFPlayer
//
//  Created by StyxS on 2020/12/20.
//

#ifndef ffmpeg_decoder_h
#define ffmpeg_decoder_h

#include "decoder_interface.h"

namespace sfplayer {
    class FFmpegDeocder : public IDecoderInterface {
    public:
        // IPlayerElementInterface
        bool Start() override;
        bool Stop() override;
        bool Pause() override { return false; }
        bool Resume() override { return false; }
        // 设置当前模块所需的参数
        void TransportParameter(std::shared_ptr<Parameter> p) override;
        
        void PushPacket(std::shared_ptr<MediaPacket> packet) override;
    private:
        AVCodecContext *audio_codec_context_ = NULL;
        AVCodecContext *video_codec_context_ = NULL;
        SwrContext *audio_swr_context_ = NULL;
        SwsContext *video_sws_context_ = NULL;
        
        std::shared_ptr<std::thread> audio_worker_;
        std::shared_ptr<std::thread> video_worker_;
        std::queue<std::shared_ptr<MediaPacket>> audio_packet_queue_;
        std::queue<std::shared_ptr<MediaPacket>> video_packet_queue_;
        
        bool first_audio_frame_ = true;
        
        void DecodeAudio();
        void DecodeVideo();
    };
}

#endif /* ffmpeg_decoder_h */
