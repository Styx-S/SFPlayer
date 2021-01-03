#include "sfplayer.h"

#include "ffmpeg_decoder.h"
#include "parameter.h"

namespace sfplayer {
	SFPlayer::SFPlayer() {
        status_machine_ = std::make_shared<PlayerStatusMachine>();
        
        demuxer_ = std::make_shared<FFmpegDemuxer>();
        decoder_ = std::make_shared<FFmpegDeocder>();
        
        demuxer_->SetEventBus(status_machine_);
        demuxer_->SetOutput(decoder_);
        decoder_->SetEventBus(status_machine_);
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
        if (GetStatus() != Ready) {
            printf("not ready");
            return;
        }
        demuxer_->Start();
        decoder_->Start();
		render_->Start();
        status_machine_->PostEvent(std::make_shared<PlayerEvent>(ControlStart));
	}
	void SFPlayer::Stop() {
        if (!(GetStatus() == Playing || GetStatus() == Seeking)) {
            printf("not playing");
            return;
        }
        demuxer_->Stop();
        decoder_->Stop();
		render_->Stop();
        status_machine_->PostEvent(std::make_shared<PlayerEvent>(ControlStop));
	}
	void SFPlayer::Pause() {
        if (!(GetStatus() == Playing || GetStatus() == Seeking)) {
            printf("not playing");
            return;
        }
        status_machine_->PostEvent(std::make_shared<PlayerEvent>(ControlPause));
        demuxer_->Pause();
        decoder_->Pause();
        render_->Pause();
	}
	void SFPlayer::Resume() {
        if (!(GetStatus() == Pausing || GetStatus() == Seeking)) {
            printf("not pausing");
            return;
        }
        status_machine_->PostEvent(std::make_shared<PlayerEvent>(ControlResume));
        demuxer_->Resume();
        decoder_->Resume();
        render_->Resume();
	}
	void SFPlayer::Seek(float seconds) {
        if (GetStatus() != Playing) {
            printf("not playing");
            return;
        }
        status_machine_->PostEvent(std::make_shared<PlayerEvent>(ControlSeek));
        Pause();
        int64_t ms = (int64_t)(seconds * 1000);
        demuxer_->Seek(ms);
        decoder_->Seek(ms);
        render_->Seek(ms);
        Resume();
	}

	void SFPlayer::SetRender(std::shared_ptr<Render> render) {
		render_ = render;
        render_->SetEventBus(status_machine_);
        
        decoder_->SetRender(render);
	}

    PlayerStatus SFPlayer::GetStatus() {
        if (status_machine_) {
            return status_machine_->GetCurrentStatus();
        } else {
            return PlayerStatus::UnknowError;
        }
    }
}
