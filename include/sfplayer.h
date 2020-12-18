#pragma once

#include "render_interface.h"
#include "ffmpeg_impl.h"


namespace sfplayer {
	class SFPlayer {
	public:
		SFPlayer();
		~SFPlayer();

		bool Play(std::string url);
		void Start();
		void Stop();
		void Pause();
		void Resume();
		void Seek(float seconds);

		void SetRender(std::shared_ptr<IRenderInterface> render);
	private:
		std::shared_ptr<FFMpegImpl> ffmpeg_impl_;
		std::shared_ptr<IRenderInterface> render_;
	};
}

