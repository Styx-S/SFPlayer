#pragma once

#include "ffmpeg_demuxer.h"
#include "decoder_interface.h"
#include "render.h"
#include "player_status_machine.h"

class PlayParameter;
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

		void SetRender(std::shared_ptr<Render> render);
	private:
        std::shared_ptr<PlayerStatusMachine> status_machine_;
        PlayerStatus GetStatus();
        
		std::shared_ptr<FFmpegDemuxer> demuxer_;
        std::shared_ptr<IDecoderInterface> decoder_;
		std::shared_ptr<Render> render_;
        
        std::shared_ptr<PlayParameter> play_parameter_;
	};
}

