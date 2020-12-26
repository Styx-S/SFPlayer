//
//  render.cpp
//  SFPlayer
//
//  Created by StyxS on 2020/12/26.
//

#include "render.h"

namespace sfplayer {

    IVideoRenderInterface::IVideoRenderInterface(size_t buffer_size)
    : frame_buffer_(buffer_size) {
    
    }

    // 音画同步逻辑
    std::shared_ptr<MediaFrame> IVideoRenderInterface::PickSyncFrame() {
        auto render = master_.lock();
        if (!render) {
            // 不使用音画同步
            last_frame_ = frame_buffer_.Read();
            return last_frame_;
        }
        
        int64_t targetPts = render->GetLastAudioPts();
        std::shared_ptr<MediaFrame> picked;
        
        std::shared_ptr<MediaFrame> nextFrame = ({
            std::unique_lock<std::mutex> lock(frame_buffer_.mutex_);
            frame_buffer_.UnsafeFront();
        });
        if (!nextFrame) {
            picked = last_frame_;
        }
        else if (nextFrame->pts > targetPts) {
            picked = last_frame_;
        } else {
            // 先简单取当前
            std::unique_lock<std::mutex> lock(frame_buffer_.mutex_);
            frame_buffer_.UnsafePop();
            frame_buffer_.cond_.notify_all();
            picked = nextFrame;
        }
        
        if (picked) {
            printf("[sfplayer][sync]pick video pts: %lld\n", picked->pts);
            last_frame_ = picked;
        }
        return picked;
    }

}
