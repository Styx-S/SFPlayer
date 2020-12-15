#include "sfplayer-prefix.h"

namespace sfplayer {
	enum MediaFrameType {
		AudioFrame,
		VideoFrame,
	};


	class MediaFrame {
	public:
		MediaFrameType type;
	};
}