//
//  ffmpeg_demuxer.h
//  SFPlayer
//
//  Created by StyxS on 2020/12/19.
//

#ifndef ffmpeg_demuxer_h
#define ffmpeg_demuxer_h

#include "player_element_interface.h"
#include "decoder_interface.h"

namespace sfplayer {
    class FFmpegDemuxer : public IPlayerElementInterface {
    public:
        // IPlayerElementInterface
        bool Start() override;
        bool Stop() override;
        bool Pause() override { return false; };
        bool Resume() override { return false; };
        void TransportParameter(std::shared_ptr<Parameter>p) override;
        
        void SetOutput(std::shared_ptr<IDecoderInterface> decoder);
    private:
        AVFormatContext *fmt_ctx_ = nullptr;
        std::shared_ptr<IDecoderInterface> decoder_;
        
        std::shared_ptr<std::thread> worker_;
        int video_stream_index_;
        int audio_stream_index_;
    };
}

#endif /* ffmpeg_demuxer_hpp */
