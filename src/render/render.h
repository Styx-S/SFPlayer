#pragma once

#include "player_element_interface.h"
#include "media_struct.h"

namespace sfplayer {
    
    class IAudioRenderInterface : public IPlayerElementInterface {
    public:
        //IPlayerElementInterface
        virtual bool Start() = 0;
        virtual bool Stop() = 0;
        virtual bool Pause() { return false; }
        virtual bool Resume() { return false; }
        
        virtual void TransportParameter(std::shared_ptr<Parameter> p) {}
        virtual bool PushAudioFrame(std::shared_ptr<MediaFrame> frame) = 0;
        virtual int GetCachedAudioSize() { return -1; }
    };

    class IVideoRenderInterface : public IPlayerElementInterface {
    public:
        //IPlayerElementInterface
        virtual bool Start() = 0;
        virtual bool Stop() = 0;
        virtual bool Pause() { return false; }
        virtual bool Resume() { return false; }
        
        virtual void TransportParameter(std::shared_ptr<Parameter> p) {}
        virtual void PushVideoFrame(std::shared_ptr<MediaFrame> frame) = 0;
        virtual int GetCachedVideoSize() { return -1; }
    };

	class Render : public IPlayerElementInterface {
	public:
		//IPlayerElementInterface
        virtual bool Start() override {
            return audio_render_impl_->Start()
                && video_render_impl_->Start();
        }
        virtual bool Stop() override {
            return audio_render_impl_->Stop()
                && video_render_impl_->Stop();
        }
        virtual bool Pause() override {
            return audio_render_impl_->Pause()
                && video_render_impl_->Pause();
        }
        virtual bool Resume() override {
            return audio_render_impl_->Resume()
                && video_render_impl_->Resume();
        }
        
        virtual void TransportParameter(std::shared_ptr<Parameter> p) override {
            audio_render_impl_->TransportParameter(p);
            video_render_impl_->TransportParameter(p);
        }

        virtual bool PushAudioFrame(std::shared_ptr<MediaFrame> frame) {
            return audio_render_impl_->PushAudioFrame(frame);
        }
        virtual void PushVideoFrame(std::shared_ptr<MediaFrame> frame) {
            video_render_impl_->PushVideoFrame(frame);
        }

		virtual int GetCachedAudioSize() {
            return audio_render_impl_ ? audio_render_impl_->GetCachedAudioSize() : -1;
        }
		virtual int GetCachedVedioSize() {
            return video_render_impl_ ? video_render_impl_->GetCachedVideoSize() : -1;
        }
        
        void SetAudioRenderImpl(std::shared_ptr<IAudioRenderInterface> audio_impl) {
            audio_render_impl_ = audio_impl;
        }
        void SetVideoRenderImpl(std::shared_ptr<IVideoRenderInterface> video_impl) {
            video_render_impl_ = video_impl;
        }
        
    protected:
        std::shared_ptr<IAudioRenderInterface> audio_render_impl_ = nullptr;
        std::shared_ptr<IVideoRenderInterface> video_render_impl_ = nullptr;
	};
}
