//
//  main.cpp
//  MetalTest
//
//  Created by Dot32  on 15/6/2026.
//

//#define NS_PRIVATE_IMPLEMENTATION
//#define MTL_PRIVATE_IMPLEMENTATION
//#define MTK_PRIVATE_IMPLEMENTATION
//#define CA_PRIVATE_IMPLEMENTATION
//
//#include "application.h"
//#include "glfw_adaptor.h"
//#include <iostream>
//#include <Metal/Metal.hpp>
//#include <AppKit/AppKit.hpp>
////#include <QuartzCore/QuartzCore.hpp>
////#include <MetalKit/MetalKit.hpp>
////#include <Foundation/Foundation.hpp>
////
////int main(int argc, const char * argv[]) {
////    NS::AutoreleasePool* autoreleasePool = NS::AutoreleasePool::alloc()->init();
////    
////    AppDelegate delegate;
////    
////    NS::Application* app = NS::Application::sharedApplication();
////    app->setDelegate(&delegate);
////    app->run();
////    
////    autoreleasePool->release();
////    return 0;
////}
//
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>
//
//int main() {
//    if (!glfwInit()) return -1;
//    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//
//    GLFWwindow* glfw_window = glfwCreateWindow(
//        800,
//        600,
//        "MetalTest",
//        nullptr,
//        nullptr
//    );
//
//    MTL::Device* device = MTL::CreateSystemDefaultDevice();
//    CA::MetalLayer* layer = CA::MetalLayer::layer()->retain();
//    layer->setDevice(device);
//    layer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
//    
//    CA::MetalDrawable* drawable;
//    MTL::CommandQueue* command_queue = device->newCommandQueue()->retain();
//    
//    NS::Window* window = ((NS::Window*)get_ns_window(
//        glfw_window,
//        layer
//    ))->retain();
//    
//    MTL::RenderPassDescriptor* rpd = MTL::RenderPassDescriptor::alloc()->init();
//    MTL::RenderPassColorAttachmentDescriptor* colour_attachment;
//    colour_attachment = rpd->colorAttachments()->object(0);
//    colour_attachment->setLoadAction(MTL::LoadAction::LoadActionClear);
//    colour_attachment->setClearColor(MTL::ClearColor(1.0f, 0.1f, 0.05f, 1.0));
//    colour_attachment->setStoreAction(MTL::StoreAction::StoreActionStore);
//
//    while (!glfwWindowShouldClose(glfw_window)) {
//        glfwPollEvents();
//        
//        NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
//        
//        drawable = layer->nextDrawable();
//        colour_attachment->setTexture(drawable->texture());
//        
//        MTL::CommandBuffer* cmd = command_queue->commandBuffer();
//        MTL::RenderCommandEncoder* enc = cmd->renderCommandEncoder(rpd);
//        
//        enc->endEncoding();
//        cmd->presentDrawable(drawable);
//        cmd->commit();
//        
//        pool->release();
//    }
//
//    glfwDestroyWindow(glfw_window);
//    glfwTerminate();
//    
//    command_queue->release();
//    window->release();
//    layer->release();
//    device->release();
//}

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "application.h"

int main(int argc, char* argv[]) {
    NS::AutoreleasePool* autoreleasePool = NS::AutoreleasePool::alloc()->init();

    Application* app = new Application();
    app->run();
    delete app;

    autoreleasePool->release();

    return 0;
}
