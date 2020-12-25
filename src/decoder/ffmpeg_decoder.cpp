//
//  ffmpeg_decoder.cpp
//  SFPlayer
//
//  Created by StyxS on 2020/12/20.
//

#include "ffmpeg_decoder.h"

namespace sfplayer {

    FFmpegDeocder::FFmpegDeocder()
    : audio_packet_buffer(100)
    , video_packet_buffer(100) {
        
    }

    void FFmpegDeocder::TransportParameter(std::shared_ptr<Parameter> p) {
        if (p->type != ParameterType::decoder) {
            return;
        }
        std::shared_ptr<DecoderParameter> decoderPar = std::static_pointer_cast<DecoderParameter>(p);
        
        // Audio codec context
        AVCodec *codec = avcodec_find_decoder(decoderPar->audio_codecpar->codec_id);
        if (!codec) {
            printf("find audio decoder error\n");
			return;
        }
        audio_codec_context_ = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(audio_codec_context_, decoderPar->audio_codecpar);
        avcodec_open2(audio_codec_context_, codec, NULL);
        
        // Video codec context
        codec = avcodec_find_decoder(decoderPar->video_codecpar->codec_id);
        if (!codec) {
            printf("find video decoder error\n");
			return;
        }
        video_codec_context_ = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(video_codec_context_, decoderPar->video_codecpar);
        video_codec_context_->framerate = decoderPar->video_framerate;
        avcodec_open2(video_codec_context_, codec, NULL);
        
        // Audio resample context
        audio_swr_context_ = swr_alloc_set_opts(NULL,
            audio_codec_context_->channel_layout,
            AV_SAMPLE_FMT_S16P,
            audio_codec_context_->sample_rate,
            audio_codec_context_->channel_layout,
            audio_codec_context_->sample_fmt,
            audio_codec_context_->sample_rate,
            0,
            NULL);
        
        // Video rescale context YUV420P
        video_sws_context_ = sws_getCachedContext(video_sws_context_,
            video_codec_context_->width,
            video_codec_context_->height,
            video_codec_context_->pix_fmt,
            video_codec_context_->width,
            video_codec_context_->height,
            AV_PIX_FMT_YUV420P,
            SWS_BILINEAR,
            NULL, NULL, NULL);
    }

    bool FFmpegDeocder::Start() {
        audio_worker_ = std::make_shared<std::thread>(&FFmpegDeocder::DecodeAudio, this);
        video_worker_ = std::make_shared<std::thread>(&FFmpegDeocder::DecodeVideo, this);
        
        return true;
    }

    bool FFmpegDeocder::Stop() {
        running_ = false;
        audio_worker_->join();
        audio_worker_ = nullptr;
        video_worker_->join();
        video_worker_ = nullptr;
		return true;
    }

    void FFmpegDeocder::PushPacket(std::shared_ptr<MediaPacket> packet) {
        if (!(packet && packet->packet_)) {
            return;
        }
        
        if (packet->type == MediaType::audio) {
            audio_packet_buffer.WaitAndWrite(packet);
        }
        else if (packet->type == MediaType::video) {
            video_packet_buffer.WaitAndWrite(packet);
        }
    }

    void FFmpegDeocder::DecodeAudio() {
        while (running_) {
            std::shared_ptr<MediaPacket> packet = audio_packet_buffer.WaitAndRead();
            
            avcodec_send_packet(audio_codec_context_, packet->packet_);
            AVFrame *srcFrame = av_frame_alloc();
            int ret = avcodec_receive_frame(audio_codec_context_, srcFrame);
            if (ret < 0) {
                av_frame_free(&srcFrame);
                continue;
            }
            
            int output_size = av_samples_get_buffer_size(NULL, audio_codec_context_->channels, 1024, AV_SAMPLE_FMT_S16, 1);
            // TODO: 下面一段代码导致没法播放声音，但是我想知道为什么
            //                size_t len = output_size * 2 * 2;
            //                uint8_t *out_buffer = new uint8_t[len];
            //                memset(out_buffer, 0x0, len);
            //                swr_convert(audio_swr_context_, &out_buffer, output_size, (const uint8_t **)srcFrame->data, srcFrame->nb_samples);
            std::shared_ptr<MediaFrame> frame = std::make_shared<MediaFrame>(MediaType::audio);
            frame->frame_->channel_layout = srcFrame->channel_layout;
            frame->frame_->sample_rate = audio_codec_context_->sample_rate;
            frame->frame_->format = AV_SAMPLE_FMT_S16;
            swr_convert_frame(audio_swr_context_, frame->frame_, srcFrame);
            av_frame_free(&srcFrame);
            
            frame->audio_data = frame->frame_->data[0];
            frame->audio_data_size = av_samples_get_buffer_size(frame->frame_->linesize, frame->frame_->channels, frame->frame_->nb_samples, (AVSampleFormat)frame->frame_->format, 0);
            
            if (first_audio_frame_) {
                first_audio_frame_ = false;
                std::shared_ptr<RenderParameter> renderPar = std::make_shared<RenderParameter>();
                renderPar->smaple_rate = frame->frame_->sample_rate;
                renderPar->channel = frame->frame_->channels;
                renderPar->sample_buffer = frame->frame_->nb_samples;
                render_->TransportParameter(renderPar);
            }
            render_->PushAudioFrame(frame);
        }
    }

    void FFmpegDeocder::DecodeVideo() {
        while (running_) {
            std::shared_ptr<MediaPacket> packet = video_packet_buffer.WaitAndRead();
            avcodec_send_packet(video_codec_context_, packet->packet_);
            
            AVFrame *srcFrame = av_frame_alloc();
            int ret = avcodec_receive_frame(video_codec_context_, srcFrame);
            
            std::shared_ptr<MediaFrame> frame = std::make_shared<MediaFrame>(MediaType::video);
            frame->frame_->width = srcFrame->width;
            frame->frame_->height = srcFrame->height;
            frame->frame_->format = AV_PIX_FMT_YUV420P;
            av_freep(frame->frame_->data);
            av_image_alloc(frame->frame_->data, frame->frame_->linesize, srcFrame->width, srcFrame->height, AV_PIX_FMT_YUV420P, 1);
            sws_scale(video_sws_context_, srcFrame->data, srcFrame->linesize, 0, video_codec_context_->height, frame->frame_->data, frame->frame_->linesize);
            av_frame_free(&srcFrame);
            
            render_->PushVideoFrame(frame);
        }
    }

}
