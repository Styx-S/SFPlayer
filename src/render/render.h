#pragma once

#include "player_element_interface.h"

namespace sfplayer {
    
    class Render;
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
        
        void SetMaster(std::weak_ptr<Render> master) { master_ = master; }
        
    protected:
        std::weak_ptr<Render> master_;
    };

    class IVideoRenderInterface : public IPlayerElementInterface {
    public:
        IVideoRenderInterface(size_t buffer_size);
        //IPlayerElementInterface
        virtual bool Start() = 0;
        virtual bool Stop() = 0;
        virtual bool Pause() { return false; }
        virtual bool Resume() { return false; }
        
        virtual void TransportParameter(std::shared_ptr<Parameter> p) {}
        virtual void PushVideoFrame(std::shared_ptr<MediaFrame> frame) { frame_buffer_.WaitAndWrite(frame); }
        virtual std::shared_ptr<MediaFrame> PickSyncFrame();
        virtual int GetCachedVideoSize() { return -1; }
        
        void SetMaster(std::weak_ptr<Render> master) { master_ = master; }
        
    protected:
        std::weak_ptr<Render> master_;
        RingBuffer<MediaFrame> frame_buffer_;
        
        std::shared_ptr<MediaFrame> last_frame_;
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
        
        static std::shared_ptr<Render> NextInstance() {
            return std::make_shared<Render>();
        }
        
        void SetAudioRenderImpl(std::shared_ptr<IAudioRenderInterface> audio_impl) {
            audio_render_impl_ = audio_impl;
        }
        void SetVideoRenderImpl(std::shared_ptr<IVideoRenderInterface> video_impl) {
            video_render_impl_ = video_impl;
        }
        
        void UpdateLastAudioPts(int64_t pts) {
            std::lock_guard<std::mutex> lock(audio_pts_mutex);
            last_audio_pts = pts;
        }
        int64_t GetLastAudioPts() {
            std::lock_guard<std::mutex> lock(audio_pts_mutex);
            return last_audio_pts;
        }
        
        void UpdateLastVideoPts(int64_t pts) {
            std::lock_guard<std::mutex> lock(video_pts_mutex);
            last_video_pts = pts;
        }
        int64_t GetLastVideoPts() {
            std::lock_guard<std::mutex> lock(video_pts_mutex);
            return last_video_pts;
        }
        
        
    protected:
        std::shared_ptr<IAudioRenderInterface> audio_render_impl_ = nullptr;
        std::shared_ptr<IVideoRenderInterface> video_render_impl_ = nullptr;
        
        int64_t last_audio_pts;
        int64_t last_video_pts;
        
        std::mutex audio_pts_mutex;
        std::mutex video_pts_mutex;
	};
}
