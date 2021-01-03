//
//  apple_audio_render.mm
//  SFPlayer
//
//  Created by StyxS on 2020/12/28.
//

#include "apple_audio_render.h"

#import <MacTypes.h>

constexpr int kFramesPerPacket = 1;
constexpr int kBitsPerChannel = 16;

SFPAppleAudioRender::SFPAppleAudioRender()
: IAudioRenderInterface(10) {
    
}

bool SFPAppleAudioRender::Start() {
    return true;
}

bool SFPAppleAudioRender::Stop() {
    return true;
}

bool SFPAppleAudioRender::Seek(int64_t milliseconds) {
    audio_buffer_.Clear();
    return true;
}

#pragma mark - AudioQueue
void SFPAppleAudioRender::TransportParameter(std::shared_ptr<Parameter> p) {
    if (p->type != ParameterType::render) {
        return;
    }
    std::shared_ptr<RenderParameter> renderPar = std::static_pointer_cast<RenderParameter>(p);
    memset(&stream_description_, 0, sizeof(stream_description_));
    stream_description_.mSampleRate = renderPar->smaple_rate;
    stream_description_.mFormatID = kAudioFormatLinearPCM;
    stream_description_.mFormatFlags = kAudioFormatFlagIsSignedInteger;
    stream_description_.mFramesPerPacket = kFramesPerPacket;
    stream_description_.mChannelsPerFrame = renderPar->channel;
    stream_description_.mBitsPerChannel = kBitsPerChannel;
    stream_description_.mBytesPerFrame = (kBitsPerChannel / 8) * renderPar->channel;
    stream_description_.mBytesPerPacket = kFramesPerPacket * stream_description_.mBytesPerFrame;

    audio_buffer_size_ = stream_description_.mBytesPerPacket * renderPar->sample_buffer;
    
    OSStatus status = AudioQueueNewOutput(&stream_description_, AudioQueueCallback, this, nil, NULL, 0, &audio_queue_);
    if (status) {
        printf("AppleAudioRender start failed\n");
    }
    AudioQueueSetParameter(audio_queue_, kAudioQueueParam_Volume, 1.0);
    
    // 空数据塞入buffer
    for (int i = 0; i < kSFPAppleAudioRenderBufferSize; i++) {
        status =  AudioQueueAllocateBuffer(audio_queue_, audio_buffer_size_, &audio_queue_buffer_[i]);
        AudioQueueCallback(this, audio_queue_, audio_queue_buffer_[i]);
    }
    AudioQueueStart(audio_queue_, NULL);
    return true;
}

void SFPAppleAudioRender::AudioQueueCallback(void *user_data, AudioQueueRef audio_queue, AudioQueueBufferRef buffer) {
    SFPAppleAudioRender *render = (SFPAppleAudioRender *)user_data;
    auto frame = render->audio_buffer_.Read();
    
    void *data = NULL;
    int len = 0;
    if (frame) {
        data = frame->audio_data;
        len = frame->audio_data_size;
        if (auto master = render->master_.lock()) {
            master->UpdateLastAudioPts(frame->pts);
        }
    }
    
    memset(buffer->mAudioData, 0x00, render->audio_buffer_size_);
    if (data && len > 0) {
        printf("[sfplayer]post audio frame size: %d", len);
        memcpy(buffer->mAudioData, data, len);
        buffer->mAudioDataByteSize = len;
    } else {
        buffer->mAudioDataByteSize = render->audio_buffer_size_;
    }
    AudioQueueEnqueueBuffer(audio_queue, buffer, 0, NULL);
}
