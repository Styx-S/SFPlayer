//
//  metal_video_render.mm
//  SFPlayer
//
//  Created by StyxS on 2020/12/22.
//

#include "metal_video_render.h"

SFPMetalVideoRender::SFPMetalVideoRender() {
    render_view_ = nil;
    InitMetal();
}

SFPMetalVideoRender::SFPMetalVideoRender(MTKView *mtkView) {
    render_view_ = mtkView;
    InitMetal();
}

SFPMetalVideoRender::~SFPMetalVideoRender() {
    
}

bool SFPMetalVideoRender::Start() {
    return true;
}

bool SFPMetalVideoRender::Stop() {
    return true;
}

void SFPMetalVideoRender::PushVideoFrame(std::shared_ptr<MediaFrame> frame) {
    last_frame_ = frame;
}

MTKView* SFPMetalVideoRender::GetRenderView() {
    return render_view_;
}

#pragma mark - metal
@interface SFPMTKViewDelegateProxy : NSObject<MTKViewDelegate> {
    SFPMetalVideoRender *_impl;
}

- initWithImpl:(SFPMetalVideoRender *)impl;
- (void)drawInMTKView:(MTKView *)view;
- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size;

@end

@implementation SFPMTKViewDelegateProxy

- (id)initWithImpl:(SFPMetalVideoRender *)impl {
    if (self = [super init]) {
        _impl = impl;
    }
    return self;
}

- (void)drawInMTKView:(MTKView *)view {
    _impl->Draw();
}

- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
    // do nothing;
}

@end

void SFPMetalVideoRender::InitMetal() {
    render_view_delegate_ = [[SFPMTKViewDelegateProxy alloc] initWithImpl:this];
    if (render_view_) {
        render_device_ = render_view_.device;
    } else {
        render_device_ = MTLCreateSystemDefaultDevice();
        render_view_ = [[MTKView alloc] initWithFrame:CGRectZero device:render_device_];
    }
    render_view_.delegate = render_view_delegate_;
    render_command_queue_ = [render_device_ newCommandQueue];
    // pipeline
    id<MTLLibrary> library = [render_device_ newDefaultLibrary];
    id<MTLFunction> vertexFunction = [library newFunctionWithName:@"vertexShader"];
    id<MTLFunction> fragmentFunction = [library newFunctionWithName:@"fragmentShader"];
    MTLRenderPipelineDescriptor *pipelineDescriptor = [MTLRenderPipelineDescriptor new];
    pipelineDescriptor.vertexFunction = vertexFunction;
    pipelineDescriptor.fragmentFunction = fragmentFunction;
    //pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
    pipelineDescriptor.colorAttachments[0].pixelFormat = render_view_.colorPixelFormat;
    
    NSError *error = nil;
    render_pipeline_state_ = [render_device_ newRenderPipelineStateWithDescriptor:pipelineDescriptor error:&error];
    if (!render_pipeline_state_) {
        NSLog(@"create pipeline error: %@", error);
    }
    // vertexBuffer
    static const struct {
        vector_float4 position;                 // 顶点坐标
        vector_float2 textureCoordinate;        // 纹理坐标
    } vertices[] = {
        { .position = { -1.0, -1.0,  0.0,  1.0 }, .textureCoordinate = { 0.f, 0.f } },
        { .position = {  1.0, -1.0,  0.0,  1.0 }, .textureCoordinate = { 1.f, 0.f } },
        { .position = {  1.0,  1.0,  0.0,  1.0 }, .textureCoordinate = { 1.f, 1.f } },
        
        { .position = { -1.0, -1.0,  0.0,  1.0 }, .textureCoordinate = { 0.f, 0.f } },
        { .position = {  1.0,  1.0,  0.0,  1.0 }, .textureCoordinate = { 1.f, 1.f } },
        { .position = { -1.0,  1.0,  0.0,  1.0 }, .textureCoordinate = { 0.f, 1.f } }
    };
    render_vertex_buffer_ = [render_device_ newBufferWithBytes:vertices length:sizeof(vertices) options:MTLResourceOptionCPUCacheModeDefault];
}

void SFPMetalVideoRender::RenderFrame(std::shared_ptr<MediaFrame> frame) {
    id<MTLCommandBuffer> commandBuffer = [render_command_queue_ commandBuffer];
    id<MTLRenderCommandEncoder> renderCommandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:render_view_.currentRenderPassDescriptor];
    [renderCommandEncoder setRenderPipelineState:render_pipeline_state_];
    [renderCommandEncoder setVertexBuffer:render_vertex_buffer_ offset:0 atIndex:0];
    [renderCommandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:6];
    [renderCommandEncoder endEncoding];
    
    [commandBuffer presentDrawable:render_view_.currentDrawable];
    [commandBuffer commit];
}

void SFPMetalVideoRender::Draw() {
    RenderFrame(last_frame_);
}


