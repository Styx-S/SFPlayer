//
//  ffmpeg_demuxer.cpp
//  SFPlayer
//
//  Created by StyxS on 2020/12/19.
//

#include "ffmpeg_demuxer.h"

namespace sfplayer {
    void FFmpegDemuxer::TransportParameter(std::shared_ptr<Parameter>p) {
        if (p->type != ParameterType::play) {
            return;
        }
        std::shared_ptr<PlayParameter> playPar = std::static_pointer_cast<PlayParameter>(p);
        int ret = avformat_open_input(&fmt_ctx_, playPar->play_url.c_str(), NULL, NULL);
        if (ret < 0) {
            PostEvent(DemuxerInitStreamError, "open input error");
			return;
        }
        ret = avformat_find_stream_info(fmt_ctx_, NULL);
        if (ret < 0) {
            PostEvent(DemuxerInitStreamError, "find stream info error");
			return;
        }
        
        // 根据读取到的信息向后传输参数
        AVStream *audioStream = nullptr, *videoStream = nullptr;
        for (int i = 0; i < fmt_ctx_->nb_streams; i++) {
            AVStream *currentStream = fmt_ctx_->streams[i];
            if (currentStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                audioStream = currentStream;
                audio_stream_index_ = i;
            }
            else if (currentStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStream = currentStream;
                video_stream_index_ = i;
            }
        }
        std::shared_ptr<DecoderParameter> decoderPar = std::make_shared<DecoderParameter>();
        decoderPar->audio_codecpar = audioStream->codecpar;
        decoderPar->audio_stream_timebase = audioStream->time_base;
        decoderPar->video_codecpar = videoStream->codecpar;
        decoderPar->video_stream_timebase = videoStream->time_base;
        decoder_->TransportParameter(decoderPar);
    }

    bool FFmpegDemuxer::Start() {
        worker_ = std::make_shared<std::thread>([this](){
            while (({
                CONDITION_WAIT(state_cond_, state_mutex_, [this](){
                    return !pause_;
                });
                running_;
            })) {
                std::shared_ptr<MediaPacket> packet = std::make_shared<MediaPacket>();
                av_read_frame(fmt_ctx_, packet->packet_);
                
                int actualIndex = packet->packet_->stream_index;
                if (actualIndex == audio_stream_index_) {
                    packet->type = MediaType::audio;
                    decoder_->PushPacket(packet);
                }
                else if (actualIndex == video_stream_index_) {
                    packet->type = MediaType::video;
                    decoder_->PushPacket(packet);
                }
            }
        });
        
        return true;
    }

    bool FFmpegDemuxer::Stop() {
        {
            SYNCHONIZED(state_mutex_);
            running_ = false;
            state_cond_.notify_all();
        }
        
        worker_->join();
        worker_ = nullptr;
        
        return true;
    }

    bool FFmpegDemuxer::Seek(int64_t milliseconds) {
        double seconds = milliseconds * 1.0 / 1000;
        av_seek_frame(fmt_ctx_,
                      video_stream_index_,
                      seconds / av_q2d(fmt_ctx_->streams[video_stream_index_]->time_base),
                      AVSEEK_FLAG_BACKWARD);
        av_seek_frame(fmt_ctx_,
                      audio_stream_index_,
                      seconds / av_q2d(fmt_ctx_->streams[audio_stream_index_]->time_base),
                      AVSEEK_FLAG_BACKWARD);
    }

    void FFmpegDemuxer::SetOutput(std::shared_ptr<IDecoderInterface> decoder) {
        decoder_ = decoder;
    }

}
