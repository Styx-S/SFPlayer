#pragma once

#include "player_element_interface.h"
#include "media_struct.h"

namespace sfplayer {
	class IRenderInterface : public IPlayerElementInterface {
	public:
		//IPlayerElementInterface
		virtual bool Start() = 0;
		virtual bool Stop() = 0;
        virtual bool Pause() { return false; }
        virtual bool Resume() { return false; }
        
        virtual void TransportParameter(std::shared_ptr<Parameter> p) {}

        virtual void pushVideoFrame(std::shared_ptr<MediaFrame> frame) = 0;
		virtual void pushAudioFrame(std::shared_ptr<MediaFrame> frame) = 0;

		virtual int GetCachedVideoSize() { return -1; }
		virtual int GetCachedAudioSize() { return -1; }

	};
}
