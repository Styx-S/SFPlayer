#pragma once

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
            frame_ = av_frame_alloc();
        }
        ~MediaFrame() {
            if (frame_) {
                if (type == video) {
                    // allocated by av_image_alloc
                    av_freep(&frame_->data[0]);
                }
                av_frame_free(&frame_);
            }
//            if (audio_data) {
//                delete [] audio_data;
//                audio_data = nullptr;
//            }
        }
        
        // audio only
        uint8_t *audio_data = nullptr;
        int audio_data_size = 0;
        
        // video & audio
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
