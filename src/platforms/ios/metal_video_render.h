//
//  metal_video_render.h
//  SFPlayer
//
//  Created by StyxS on 2020/12/22.
//


#ifndef metal_video_render_h
#define metal_video_render_h

#import <MetalKit/MetalKit.h>

#include "render.h"

using namespace sfplayer;

@class SFPMTKViewDelegateProxy;
class SFPMetalVideoRender : public IVideoRenderInterface {
public:
    SFPMetalVideoRender();
    SFPMetalVideoRender(MTKView *mtkView);
    ~SFPMetalVideoRender();
    //IPlayerElementInterface
    bool Start() override;
    bool Stop() override;
    bool Pause() override { return false; }
    bool Resume() override { return false; }
    
    void TransportParameter(std::shared_ptr<Parameter> p) override {}
    void PushVideoFrame(std::shared_ptr<MediaFrame> frame) override;
    int GetCachedVideoSize() override { return -1; }
    
    MTKView *GetRenderView();
    void _DrawNow();
private:
    __strong MTKView *render_view_;
    __strong id<MTLDevice> render_device_;
    __strong id<MTLCommandQueue> render_command_queue_;
    __strong id<MTLRenderPipelineState> render_pipeline_state_;
    __strong id<MTLBuffer> render_vertex_buffer_;
    __strong id<MTLBuffer> convert_matrix_buffer_;
    __strong SFPMTKViewDelegateProxy *render_view_delegate_;
    
    void InitMetal();
    bool SetFragmentTexture(id<MTLRenderCommandEncoder> encoder);
    
    std::shared_ptr<MediaFrame> last_frame_;
};


#endif /* metal_video_render_h */
