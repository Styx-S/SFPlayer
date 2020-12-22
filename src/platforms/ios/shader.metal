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

vertex VertexOutput vertexShader(uint vid [[ vertex_id]],
                                 device VertexInput* input [[buffer(0)]]) {
    VertexOutput out;
    out.position = input[vid].position;
    out.textureCoordinate = input[vid].textureCoordinate;
    return out;
}

fragment float4 fragmentShader(VertexOutput out [[stage_in]]) {
    return float4(1.0, 0, 0, 1.0);
}
