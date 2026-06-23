//
//  imgui_adaptor.h
//  MetalTest
//
//  Created by Dot32  on 23/6/2026.
//

#ifndef imgui_adaptor_h
#define imgui_adaptor_h

// Forward declaring rather than including
namespace MTL {
    class Device;
    class RenderPassDescriptor;
    class CommandBuffer;
    class RenderCommandEncoder;
}

void imgui_init_metal(MTL::Device* device);
void imgui_shutdown_metal();
void imgui_new_frame_metal(MTL::RenderPassDescriptor* rpd);
void imgui_render_metal(
    MTL::CommandBuffer* cmd, MTL::RenderCommandEncoder* enc
);


#endif /* imgui_adaptor_h */
