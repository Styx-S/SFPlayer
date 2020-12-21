#pragma once

#include "render.h"

namespace sfplayer {
    
    class SDLAudioRender : public IAudioRenderInterface {
    public:
        //IPlayerElementInterface
        bool Start() override;
        bool Stop() override;
        bool Pause() override { return false; }
        bool Resume() override { return false; }
        
        void TransportParameter(std::shared_ptr<Parameter> p) override;
        void PushAudioFrame(std::shared_ptr<MediaFrame> frame) override;
        int GetCachedAudioSize() override { return -1; }
    private:
        std::queue<std::shared_ptr<MediaFrame>> audio_queue_;
        std::mutex audio_queue_mutex_;

        static void ReadAudioFrameCallback(void *udata, Uint8 *stream, int len);
    };

    class SDLVideoRender : public IVideoRenderInterface {
    public:
        ~SDLVideoRender();
        //IPlayerElementInterface
        bool Start() override;
        bool Stop() override;
        bool Pause() override { return false; }
        bool Resume() override { return false; }
        
        void TransportParameter(std::shared_ptr<Parameter> p) override {}
        void PushVideoFrame(std::shared_ptr<MediaFrame> frame) override;
        int GetCachedVideoSize() override { return -1; }
    private:
        SDL_Window *window_ = NULL;
        SDL_Renderer *render_ = NULL;
        SDL_Texture *texture_ = NULL;
        
        std::queue<std::shared_ptr<MediaFrame>> video_queue_;
    };


	class SDLRender : public Render {
	public:
		SDLRender();
		~SDLRender();
        
        
        /// 等待结束
        void WaitLoop();
	};
}

