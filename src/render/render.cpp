#include "sfplayer-prefix.h"
#include "render.h"

namespace sfplayer {
	const float width = 1600;
	const float height = 900;
	Render::Render()
	/*: window_(nullptr)*/ {
	}


	Render::~Render() {
	}

	void Render::Start() {
		window_ = SDL_CreateWindow("SFPlayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
		render_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC);
	}

	void Render::Stop() {
		SDL_DestroyWindow(window_);
	}

	void Render::pushAudioFrame(AVFrame *frame) {

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
}
