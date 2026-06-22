//
//  triangle.metal
//  MetalTest
//
//  Created by Dot32  on 19/6/2026.
//

#include <metal_stdlib>
#include "types.h"

using namespace metal;

struct VertexOutput {
    float4 pos [[position]];
    float2 tex_coord;
};

VertexOutput vertex vertex_main(
    uint vertex_id [[vertex_id]],
    device const Vertex* vertices [[buffer(0)]],
    constant float4x4& model [[buffer(1)]],
    constant float4x4& view [[buffer(2)]],
    constant float4x4& projection [[buffer(3)]]
) {
    VertexOutput payload;
    
    payload.pos = projection * view * model * float4(
        vertices[vertex_id].pos,
        1.0
    );
    
//    payload.colour = float3(1.0, 0.2, 0.0);
//    payload.colour = float3(vertices[vertex_id].tex_coord, 0.0);
    payload.tex_coord = vertices[vertex_id].tex_coord;
    
    return payload;
}

float4 fragment frag_main(
    VertexOutput in [[stage_in]],
    texture2d<float> texture [[texture(0)]],
    sampler sampler [[sampler(0)]]
) {
    return texture.sample(sampler, in.tex_coord);
}
