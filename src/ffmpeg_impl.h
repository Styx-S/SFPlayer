#pragma once
#include "player_element_interface.h"
#include "render.h"
#include <thread>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

namespace sfplayer {
	class FFMpegImpl : IPlayerElementInterface {
	public:
		FFMpegImpl();
		~FFMpegImpl();
		

		bool Play(std::string url);
		// IPlayerElementInterface
		void Start();
		void Stop();
		void Pause() {}
		void Resume() {}
		void SetRenderOutput(std::shared_ptr<IRenderInterface> render);

	private:
		std::shared_ptr<IRenderInterface> render_;
		std::shared_ptr<std::thread> worker_;

		AVFormatContext *format_context_ = NULL;
		AVCodecContext *audio_codec_context_ = NULL;
		AVCodecContext *video_codec_context_ = NULL;
		SwrContext *audio_swr_context_ = NULL;
		SwsContext *video_sws_context_ = NULL;
		int audio_stream_index_;
		int video_stream_index_;
	};
}

