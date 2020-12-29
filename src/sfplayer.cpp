#include "sfplayer.h"

#include "ffmpeg_decoder.h"
#include "parameter.h"

namespace sfplayer {
	SFPlayer::SFPlayer() {
        demuxer_ = std::make_shared<FFmpegDemuxer>();
        decoder_ = std::make_shared<FFmpegDeocder>();
        
        demuxer_->SetOutput(decoder_);
	}

	SFPlayer::~SFPlayer() {
        demuxer_ = nullptr;
        decoder_ = nullptr;
        render_ = nullptr;
	}

	bool SFPlayer::Play(std::string url) {
        play_parameter_ = std::make_shared<PlayParameter>();
        play_parameter_->play_url = url;
        demuxer_->TransportParameter(play_parameter_);
        return true;
	}

	void SFPlayer::Start() {
        demuxer_->Start();
        decoder_->Start();
		render_->Start();
	}
	void SFPlayer::Stop() {
        demuxer_->Stop();
        decoder_->Stop();
		render_->Stop();
	}
	void SFPlayer::Pause() {
        demuxer_->Pause();
        decoder_->Pause();
        render_->Pause();
	}
	void SFPlayer::Resume() {
        demuxer_->Resume();
        decoder_->Resume();
        render_->Resume();
	}
	void SFPlayer::Seek(float seconds) {

	}

	void SFPlayer::SetRender(std::shared_ptr<Render> render) {
		render_ = render;
        decoder_->SetRender(render);
	}
}
