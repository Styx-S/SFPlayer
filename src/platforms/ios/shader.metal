//
//  shader.metal
//  SFPlayer
//
//  Created by StyxS on 2020/12/22.
//

#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;

struct VertexInput {
    vector_float4 position;
    vector_float2 textureCoordinate;
};

struct VertexOutput {
    vector_float4 position [[position]];
    vector_float2 textureCoordinate;
};

struct YUV2RGBConverter {
    matrix_float3x3 matrix;
    vector_float3 offset;
};

vertex VertexOutput
vertexShader(const device VertexInput* input [[buffer(0)]],
             uint vid [[ vertex_id]]) {
    VertexOutput out;
    out.position = input[vid].position;
    out.textureCoordinate = input[vid].textureCoordinate;
    return out;
}

fragment float4
fragmentShader(VertexOutput out [[stage_in]],
               texture2d<float> yTexture [[texture(0)]],
               texture2d<float> uTexture [[texture(1)]],
               texture2d<float> vTexture [[texture(2)]],
               device YUV2RGBConverter *converter [[buffer(0)]]) {
    constexpr sampler linerSampler(mag_filter::linear, min_filter::linear);
    
    vector_float3 yuv = vector_float3(yTexture.sample(linerSampler, out.textureCoordinate).r,
                                      uTexture.sample(linerSampler, out.textureCoordinate).r,
                                      vTexture.sample(linerSampler, out.textureCoordinate).r);
    vector_float3 rgb = converter->matrix * (yuv + converter->offset);
    
    return float4(rgb, 1.0);
}
