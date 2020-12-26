//
//  metal_video_render.mm
//  SFPlayer
//
//  Created by StyxS on 2020/12/22.
//

#include "metal_video_render.h"

SFPMetalVideoRender::SFPMetalVideoRender()
: SFPMetalVideoRender(nil) {
    
}

SFPMetalVideoRender::SFPMetalVideoRender(MTKView *mtkView)
: IVideoRenderInterface(10) {
    render_view_ = mtkView;
    InitMetal();
}

SFPMetalVideoRender::~SFPMetalVideoRender() {
    
}

bool SFPMetalVideoRender::Start() {
    return true;
}

bool SFPMetalVideoRender::Stop() {
    render_view_.delegate = nil;
    return true;
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
    _impl->_DrawNow();
}

- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
    // do nothing;
}

@end

void SFPMetalVideoRender::InitMetal() {
    if (render_view_) {
        render_device_ = render_view_.device;
    } else {
        render_device_ = MTLCreateSystemDefaultDevice();
        render_view_ = [[MTKView alloc] initWithFrame:CGRectZero device:render_device_];
    }
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
        { .position = { -1.0,  1.0,  0.0,  1.0 }, .textureCoordinate = { 0.f, 0.f } },
        { .position = { -1.0, -1.0,  0.0,  1.0 }, .textureCoordinate = { 0.f, 1.f } },
        { .position = {  1.0,  1.0,  0.0,  1.0 }, .textureCoordinate = { 1.f, 0.f } },
        { .position = {  1.0, -1.0,  0.0,  1.0 }, .textureCoordinate = { 1.f, 1.f } }
    };
    render_vertex_buffer_ = [render_device_ newBufferWithBytes:vertices length:sizeof(vertices) options:MTLResourceOptionCPUCacheModeDefault];
    
    static const struct {
        matrix_float3x3 matrix;
        vector_float3 offset;
    } converter  = {
        .matrix = {
            (simd_float3){1.0,    1.0,    1.0},
            (simd_float3){0.0,    -0.343, 1.765},
            (simd_float3){1.4,    -0.711, 0.0},
        },
        .offset = { 0, -0.5, -0.5 }
    };
    convert_matrix_buffer_ = [render_device_ newBufferWithBytes:&converter length:sizeof(converter) options:MTLResourceOptionCPUCacheModeDefault];
    
    render_view_delegate_ = [[SFPMTKViewDelegateProxy alloc] initWithImpl:this];
    render_view_.delegate = render_view_delegate_;
}

bool SFPMetalVideoRender::SetFragmentTexture(id<MTLRenderCommandEncoder> encoder, std::shared_ptr<MediaFrame> frame) {
    
    if (!(
          frame
          && frame->frame_
          && frame->frame_->width != 0
          && frame->frame_->height != 0
          )) {
        return false;
    }
    int width = frame->frame_->width;
    int height = frame->frame_->height;
    
    MTLTextureDescriptor *yPanelDescriptor = [[MTLTextureDescriptor alloc] init];
    yPanelDescriptor.pixelFormat = MTLPixelFormatR8Unorm;
    yPanelDescriptor.width = width;
    yPanelDescriptor.height = height;
    MTLTextureDescriptor *uvPanelDescriptor = [[MTLTextureDescriptor alloc] init];
    uvPanelDescriptor.pixelFormat = MTLPixelFormatR8Unorm;
    uvPanelDescriptor.width = width / 2;
    uvPanelDescriptor.height = height / 2;
    
    id<MTLTexture> yPanelTexture = [render_device_ newTextureWithDescriptor:yPanelDescriptor];
    id<MTLTexture> uPanelTexture = [render_device_ newTextureWithDescriptor:uvPanelDescriptor];
    id<MTLTexture> vPanelTexture = [render_device_ newTextureWithDescriptor:uvPanelDescriptor];
    MTLRegion yRegion = { {0, 0, 0}, { yPanelDescriptor.width, yPanelDescriptor.height, 1} };
    MTLRegion uvReigon = { {0, 0, 0}, { uvPanelDescriptor.width, uvPanelDescriptor.height, 1} };
    [yPanelTexture replaceRegion:yRegion
                     mipmapLevel:0
                       withBytes:frame->frame_->data[0]
                     bytesPerRow:frame->frame_->linesize[0]];
    [uPanelTexture replaceRegion:uvReigon
                     mipmapLevel:0
                       withBytes:frame->frame_->data[1]
                     bytesPerRow:frame->frame_->linesize[1]];
    [vPanelTexture replaceRegion:uvReigon
                     mipmapLevel:0
                       withBytes:frame->frame_->data[2]
                     bytesPerRow:frame->frame_->linesize[2]];
    
    [encoder setFragmentTexture:yPanelTexture atIndex:0];
    [encoder setFragmentTexture:uPanelTexture atIndex:1];
    [encoder setFragmentTexture:vPanelTexture atIndex:2];
    return true;
}

void SFPMetalVideoRender::_DrawNow() {
    std::shared_ptr<MediaFrame> pickFrame = PickSyncFrame();
    id<MTLCommandBuffer> commandBuffer = [render_command_queue_ commandBuffer];
    id<MTLRenderCommandEncoder> renderCommandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:render_view_.currentRenderPassDescriptor];
    
    if (!SetFragmentTexture(renderCommandEncoder, pickFrame)) {
        [renderCommandEncoder endEncoding];
        return;
    }
    [renderCommandEncoder setRenderPipelineState:render_pipeline_state_];
    [renderCommandEncoder setVertexBuffer:render_vertex_buffer_ offset:0 atIndex:0];
    [renderCommandEncoder setFragmentBuffer:convert_matrix_buffer_ offset:0 atIndex:0];
    [renderCommandEncoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];
    [renderCommandEncoder endEncoding];
    
    [commandBuffer presentDrawable:render_view_.currentDrawable];
    [commandBuffer commit];
}

