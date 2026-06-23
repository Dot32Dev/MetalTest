//
//  imgui_adaptor.mm
//  MetalTest
//
//  Created by Dot32  on 23/6/2026.
//

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include "imgui_adaptor.h"
#include <imgui_impl_metal.h>

void imgui_init_metal(MTL::Device* device) {
    ImGui_ImplMetal_Init((__bridge id<MTLDevice>)device);
}

void imgui_shutdown_metal() {
    ImGui_ImplMetal_Shutdown();
}

void imgui_new_frame_metal(MTL::RenderPassDescriptor* rpd) {
    ImGui_ImplMetal_NewFrame((__bridge MTLRenderPassDescriptor*)rpd);
}

void imgui_render_metal(
    MTL::CommandBuffer* cmd,
    MTL::RenderCommandEncoder* enc
) {
    ImGui_ImplMetal_RenderDrawData(
        ImGui::GetDrawData(),
        (__bridge id<MTLCommandBuffer>)cmd,
        (__bridge id<MTLRenderCommandEncoder>)enc
    );
}
