#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

namespace sfplayer {
	enum MediaType : int {
		audio = 0,
		video,
	};


	class MediaFrame {
	public:
        MediaType type;
        MediaFrame(MediaType t) {
            type = t;
            if (type == video) {
                frame_ = av_frame_alloc();
            }
        }
        ~MediaFrame() {
            if (frame_) {
                if (type == video) {
                    // allocated by av_image_alloc
                    av_freep(&frame_->data[0]);
                }
                av_frame_free(&frame_);
            }
            if (audio_data) {
                av_freep(&audio_data);
                audio_data_size = 0;
            }
        }
        
        int64_t pts;
        
        // audio only
        uint8_t *audio_data = nullptr;  // 音频数据
        int audio_data_size = 0;        // 音频数据大小
        int channels = 0;               // 通道数
        int sample_rate = 0;            // 采样率
        int nb_samples = 0;             // 采样数
        
        // video only
        AVFrame *frame_ = nullptr;
	};

    class MediaPacket {
    public:
        MediaPacket() {
            packet_ = av_packet_alloc();
        }
        MediaPacket(AVPacket* packet) {
            packet_ = packet;
        }
        
        ~MediaPacket() {
            if (packet_) {
                av_packet_free(&packet_);
            }
        }
        
        MediaType type;
        AVPacket *packet_;
    };
}
