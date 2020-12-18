#pragma once
namespace sfplayer {
	class IPlayerElementInterface {
		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual void Pause() {}
		virtual void Resume() {}
	protected:
		bool running_ = true;
	};
}
