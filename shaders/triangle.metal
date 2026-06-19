//
//  triangle.metal
//  MetalTest
//
//  Created by Dot32  on 19/6/2026.
//

#include <metal_stdlib>
using namespace metal;

struct VertexInput {
    float3 position;
};

struct VertexOutput {
    float4 position [[position]];
    float3 colour;
};

VertexOutput vertex vertex_main(
    uint vertex_id [[vertex_id]],
    device const VertexInput* vertices [[buffer(0)]],
    constant float4x4& transform [[buffer(1)]]
) {
    VertexOutput payload;
    
    payload.position = transform * float4(vertices[vertex_id].position, 1.0);
    payload.colour = float3(1.0, 0.2, 0.0);
    
    return payload;
}

float4 fragment frag_main(VertexOutput in [[stage_in]]) {
    return float4(in.colour, 1.0);
}
