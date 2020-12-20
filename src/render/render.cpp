#include "render.h"

#include <functional>

namespace sfplayer {
	const float width = 1600;
	const float height = 900;
	Render::Render() {
	}


	Render::~Render() {
        SDL_DestroyTexture(texture_);
        SDL_DestroyRenderer(render_);
        SDL_DestroyWindow(window_);
	}

    void Render::TransportParameter(std::shared_ptr<Parameter> p) {
        if (p->type != ParameterType::render) {
            return;
        }
        std::shared_ptr<RenderParameter> renderPar = std::static_pointer_cast<RenderParameter>(p);
        
        SDL_AudioSpec spec;
        spec.freq = renderPar->smaple_rate;
        spec.format = AUDIO_S16SYS;
        spec.channels = renderPar->channel;
        spec.silence = 0;
        spec.samples = renderPar->sample_buffer;
        spec.callback = ReadAudioFrameCallback;
        spec.userdata = this;

        if (SDL_OpenAudio(&spec, NULL) < 0) {
            printf("can't open audio.\n");
            return;
        }
        SDL_PauseAudio(0);
    }

	bool Render::Start() {
		window_ = SDL_CreateWindow("SFPlayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
		render_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC);
        
        return true;
	}

	bool Render::Stop() {
        return true;
	}

	void Render::pushAudioFrame(std::shared_ptr<MediaFrame> frame) {
		std::lock_guard<std::mutex> lock(audio_queue_mutex_);
		audio_queue_.push(frame);
	}

	void Render::pushVideoFrame(std::shared_ptr<MediaFrame> frame) {
		if (!texture_) {
            //SDL_RenderSetLogicalSize(render_, frame->frame_->width, frame->frame_->height);
			texture_ = SDL_CreateTexture(render_, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_TARGET, frame->frame_->width, frame->frame_->height);
		}
		printf("render frame, last:%ld", video_queue_.size());
		SDL_SetRenderDrawColor(render_, 0x11, 0x11, 0x11, 0xff);
		SDL_RenderClear(render_);
		SDL_UpdateYUVTexture(texture_,
                             NULL,
                             frame->frame_->data[0],
                             frame->frame_->linesize[0],
                             frame->frame_->data[1],
                             frame->frame_->linesize[1],
                             frame->frame_->data[2],
                             frame->frame_->linesize[2]);
		SDL_RenderCopy(render_, texture_, NULL, NULL);
		SDL_RenderPresent(render_);
		SDL_Delay(15);
	}

	void Render::ReadAudioFrameCallback(void *udata, Uint8 *stream, int len) {
		Render *render = (Render *)udata;
		SDL_memset(stream, 0, len);
		std::lock_guard<std::mutex> lock(render->audio_queue_mutex_);
		if (render->audio_queue_.size() > 0) {
			std::shared_ptr<MediaFrame> frame = render->audio_queue_.front();
			render->audio_queue_.pop();
			printf("render audio, last: %ld\n", render->audio_queue_.size());
			SDL_MixAudio(stream, frame->audio_data, frame->audio_data_size, SDL_MIX_MAXVOLUME);
		}
	}
}
