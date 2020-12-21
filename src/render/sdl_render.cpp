#include "sdl_render.h"

#include <functional>

namespace sfplayer {
	const float width = 1600;
	const float height = 900;
	SDLRender::SDLRender() {
        SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        audio_render_impl_ = std::make_shared<SDLAudioRender>();
        video_render_impl_ = std::make_shared<SDLVideoRender>();
	}


	SDLRender::~SDLRender() {
        audio_render_impl_ = nullptr;
        video_render_impl_ = nullptr;
        SDL_Quit();
	}

    void SDLRender::WaitLoop() {
        bool keepAlive = true;
        while (keepAlive) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    keepAlive = false;
                }
            }
        }
    }

#pragma mark - SDLAudioRender

    void SDLAudioRender::TransportParameter(std::shared_ptr<Parameter> p) {
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

    bool SDLAudioRender::Start() {
        return true;
    }
	
	bool SDLAudioRender::Stop() {
        return true;
	}

	void SDLAudioRender::PushAudioFrame(std::shared_ptr<MediaFrame> frame) {
		std::lock_guard<std::mutex> lock(audio_queue_mutex_);
		audio_queue_.push(frame);
	}

	

	void SDLAudioRender::ReadAudioFrameCallback(void *udata, Uint8 *stream, int len) {
		SDLAudioRender *render = (SDLAudioRender *)udata;
		SDL_memset(stream, 0, len);
		std::lock_guard<std::mutex> lock(render->audio_queue_mutex_);
		if (render->audio_queue_.size() > 0) {
			std::shared_ptr<MediaFrame> frame = render->audio_queue_.front();
			render->audio_queue_.pop();
			printf("render audio, last: %ld\n", render->audio_queue_.size());
			SDL_MixAudio(stream, frame->audio_data, frame->audio_data_size, SDL_MIX_MAXVOLUME);
		}
	}

#pragma mark - SDLVideoRender
    SDLVideoRender::~SDLVideoRender() {
        SDL_DestroyTexture(texture_);
        SDL_DestroyRenderer(render_);
        SDL_DestroyWindow(window_);
    }

    bool SDLVideoRender::Start() {
        window_ = SDL_CreateWindow("SFPlayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
        render_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC);
    
        return true;
    }

    bool SDLVideoRender::Stop() {
        return true;
    }

    void SDLVideoRender::PushVideoFrame(std::shared_ptr<MediaFrame> frame) {
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
}
