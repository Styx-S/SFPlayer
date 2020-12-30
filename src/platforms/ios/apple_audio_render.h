//
//  apple_audio_render.h
//  SFPlayer
//
//  Created by StyxS on 2020/12/28.
//

#ifndef apple_audio_render_h
#define apple_audio_render_h

#include "render.h"
#include "ring_buffer.h"

#import <AudioToolbox/AudioQueue.h>

using namespace sfplayer;

constexpr int kSFPAppleAudioRenderBufferSize = 3;

class SFPAppleAudioRender : public IAudioRenderInterface {
public:
    SFPAppleAudioRender();
    //IPlayerElementInterface
    bool Start() override;
    bool Stop() override;
    virtual bool Seek(int64_t  milliseconds) override;
    
    void TransportParameter(std::shared_ptr<Parameter> p) override;
    bool PushAudioFrame(std::shared_ptr<MediaFrame> frame) override;
    
private:
    AudioQueueRef audio_queue_;
    AudioStreamBasicDescription stream_description_;
    AudioQueueBufferRef audio_buffer_[kSFPAppleAudioRenderBufferSize];
    int audio_buffer_size_;
    RingBuffer<MediaFrame> frame_buffer_;
    
    static void AudioQueueCallback(void *user_data,
                                   AudioQueueRef audio_queue,
                                   AudioQueueBufferRef buffer);
};

#endif /* apple_audio_render_h */
