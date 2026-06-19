//
//  triangle.metal
//  MetalTest
//
//  Created by Dot32  on 19/6/2026.
//

#include <metal_stdlib>
using namespace metal;

constant float4 positions[] {
    float4(-0.75, -0.75, 0.0, 1.0),
    float4(0.75, -0.75, 0.0, 1.0),
    float4(0.0, 0.75, 0.0, 1.0)
};

constant float3 colours[] {
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
};

struct VertexPayload {
    float4 position [[position]];
    float3 colour;
};

VertexPayload vertex vertex_main(uint vertex_id [[vertex_id]]) {
    VertexPayload payload;
    
    payload.position = positions[vertex_id];
    payload.colour = colours[vertex_id];
    
    return payload;
}

float4 fragment frag_main(VertexPayload in [[stage_in]]) {
    return float4(in.colour, 1.0);
}
