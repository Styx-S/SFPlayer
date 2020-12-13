#pragma once

#include "render_interface.h"

namespace sfplayer {
	class Render : public IRenderInterface {
	public:
		Render();
		~Render();

		void Start();
		void Stop();
		void Pause() {};
		void Resume() {};

		void pushVideoFrame(uint8_t *dst_data[4], int dst_linesize[4], float width, float height);
		void pushAudioFrame(AVFrame *frmae);

		int GetCachedVideoSize() { return -1; }
		int GetCachedAudioSize() { return -1; }

	private:
		SDL_Window *window_ = NULL;
		SDL_Renderer *render_ = NULL;
		SDL_Texture *texture_ = NULL;
		std::queue<int> *video_queue_;
		std::queue<int> *audio_queue_;
	};
}

