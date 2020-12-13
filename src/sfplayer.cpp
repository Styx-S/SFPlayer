#include "sfplayer-prefix.h"
#include "sfplayer.h"

namespace sfplayer {
	SFPlayer::SFPlayer() {
		ffmpeg_impl_ = std::make_shared<FFMpegImpl>();
	}

	SFPlayer::~SFPlayer() {
		
	}

	bool SFPlayer::Play(std::string url) {
		return ffmpeg_impl_->Play(url);
	}

	void SFPlayer::Start() {
		render_->Start();
		ffmpeg_impl_->Start();
	}
	void SFPlayer::Stop() {

	}
	void SFPlayer::Pause() {

	}
	void SFPlayer::Resume() {

	}
	void SFPlayer::Seek(float seconds) {

	}

	void SFPlayer::SetRender(std::shared_ptr<IRenderInterface> render) {
		render_ = render;
		ffmpeg_impl_->SetRenderOutput(render);
	}
}