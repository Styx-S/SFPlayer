#include "render.h"

#include <functional>

namespace sfplayer {
	const float width = 1600;
	const float height = 900;
	Render::Render() {
	}


	Render::~Render() {
	}

	void Render::Start() {
		// ª≠√Ê‰÷»æ
		window_ = SDL_CreateWindow("SFPlayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
		render_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC);
		// “Ù∆µ‰÷»æ
		SDL_AudioSpec spec;
		spec.freq = 48000;
		spec.format = AUDIO_S16SYS;
		spec.channels = 1;
		spec.silence = 0;
		spec.samples = 1024;
		spec.callback = ReadAudioFrameCallback;
		spec.userdata = this;

		if (SDL_OpenAudio(&spec, NULL) < 0) {
			printf("can't open audio.\n");
			return;
		} 
		SDL_PauseAudio(0);
	}

	void Render::Stop() {
		SDL_DestroyWindow(window_);
	}

	void Render::pushAudioFrame(uint8_t **pcm_data) {
		std::lock_guard<std::mutex> lock(audio_queue_mutex_);
		audio_queue_.push(pcm_data);
	}

	void Render::pushVideoFrame(uint8_t *dst_data[4], int dst_linesize[4], float width, float height) {
		if (!texture_) {
			texture_ = SDL_CreateTexture(render_, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_TARGET, width, height);
		}
		printf("render frame");
		SDL_SetRenderDrawColor(render_, 0x11, 0x11, 0x11, 0xff);
		SDL_RenderClear(render_);
		SDL_UpdateYUVTexture(texture_, NULL, dst_data[0], dst_linesize[0], dst_data[1], dst_linesize[1], dst_data[2], dst_linesize[2]);
		SDL_RenderCopy(render_, texture_, NULL, NULL);
		SDL_RenderPresent(render_);
		SDL_Delay(15);
		av_freep(dst_data);
	}

	void Render::ReadAudioFrameCallback(void *udata, Uint8 *stream, int len) {
		Render *render = (Render *)udata;
		SDL_memset(stream, 0, len);
		std::lock_guard<std::mutex> lock(render->audio_queue_mutex_);
		if (render->audio_queue_.size() > 0) {
			uint8_t **data = render->audio_queue_.front();
			render->audio_queue_.pop();
			printf("render audio");
			SDL_MixAudio(stream, data[0], 2112, SDL_MIX_MAXVOLUME);
		}
	}
}
