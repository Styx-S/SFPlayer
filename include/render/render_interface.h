#pragma once

#include "player_element_interface.h"
extern "C" {
#include <libavformat/avformat.h>
}

namespace sfplayer {
	class IRenderInterface : public IPlayerElementInterface {
	public:
		//IPlayerElementInterface
		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual void Pause() {}
		virtual void Resume() {}

		virtual void pushVideoFrame(uint8_t *dst_data[4], int dst_linesize[4], float width, float height) = 0;
		virtual void pushAudioFrame(AVFrame *frame) = 0;

		virtual int GetCachedVideoSize() { return -1; }
		virtual int GetCachedAudioSize() { return -1; }

	};
}