#pragma once

#include "render_interface.h"
#include <mutex>

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
		void pushAudioFrame(uint8_t **pcm_data);

		int GetCachedVideoSize() { return -1; }
		int GetCachedAudioSize() { return -1; }

	private:
		SDL_Window *window_ = NULL;
		SDL_Renderer *render_ = NULL;
		SDL_Texture *texture_ = NULL;
		std::queue<int> video_queue_;
		std::queue<uint8_t **> audio_queue_;
		std::mutex audio_queue_mutex_;

		static void ReadAudioFrameCallback(void *udata, Uint8 *stream, int len);
	};
}

