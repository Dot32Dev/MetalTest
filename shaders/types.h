//
//  types.h
//  MetalTest
//
//  Created by Dot32  on 22/6/2026.
//

#ifndef types_h
#define types_h

#ifdef __METAL_VERSION__
    #include <metal_stdlib>
    using metal::float3;
#else
    #include <simd/simd.h>
    using simd::float3;
#endif

struct Vertex {
    float3 pos;
};

#endif /* types_h */
