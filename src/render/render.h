#pragma once

#include "render_interface.h"
#include <mutex>

namespace sfplayer {
	class Render : public IRenderInterface {
	public:
		Render();
		~Render();

		bool Start() override ;
		bool Stop() override ;
        bool Pause() override { return false; };
        bool Resume() override { return false; };
        virtual void TransportParameter(std::shared_ptr<Parameter> p) override;

		void pushVideoFrame(std::shared_ptr<MediaFrame> frame);
		void pushAudioFrame(std::shared_ptr<MediaFrame> frame);

		int GetCachedVideoSize() { return -1; }
		int GetCachedAudioSize() { return -1; }

	private:
		SDL_Window *window_ = NULL;
		SDL_Renderer *render_ = NULL;
		SDL_Texture *texture_ = NULL;
		std::queue<std::shared_ptr<MediaFrame>> video_queue_;
		std::queue<std::shared_ptr<MediaFrame>> audio_queue_;
		std::mutex audio_queue_mutex_;

		static void ReadAudioFrameCallback(void *udata, Uint8 *stream, int len);
	};
}

