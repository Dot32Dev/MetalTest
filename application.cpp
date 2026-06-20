//
//  application.cpp
//  MetalTest
//
//  Created by Dot32  on 15/6/2026.
//

#include "application.h"
#include "glfw_adaptor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <simd/simd.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using NS::StringEncoding::UTF8StringEncoding;

// The layout of each vertex
struct Vertex {
    simd::float3 pos;
};

Application::Application() {
    if (!glfwInit()) {
        std::cerr << "Could not init GLFW" << std::endl;
        return;
    }
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);

    glfw_window = glfwCreateWindow(800, 600, "MetalTest", nullptr, nullptr);

    device = MTL::CreateSystemDefaultDevice();
    layer = CA::MetalLayer::layer()->retain();
    layer->setDevice(device);
    layer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

    command_queue = device->newCommandQueue()->retain();
    buildQuad();
    pipeline = buildShader(
        "shaders/triangle.metal",
        "vertex_main",
        "frag_main"
    );
    model = glm::mat4(1.0f);

    window = ((NS::Window*)get_ns_window(glfw_window, layer))->retain();
    
    NS::Menu* menu = createMenuBar();
    NS::Application* app = NS::Application::sharedApplication();
    app->setMainMenu(menu);
    menu->release();

    rpd = MTL::RenderPassDescriptor::alloc()->init();
    colour_attachment = rpd->colorAttachments()->object(0);
    colour_attachment->setLoadAction(MTL::LoadAction::LoadActionClear);
    colour_attachment->setClearColor(MTL::ClearColor(0.0f, 0.0f, 0.0f, 1.0));
    colour_attachment->setStoreAction(MTL::StoreAction::StoreActionStore);
}

Application::~Application() {
    rpd->release();
    command_queue->release();
    window->release();
    layer->release();
    device->release();
    vertex_buffer->release();
    index_buffer->release();
    pipeline->release();
    
    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}

void Application::run() {
    while (!glfwWindowShouldClose(glfw_window)) {
        glfwPollEvents();
        
        NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
        
        drawable = layer->nextDrawable();
        colour_attachment->setTexture(drawable->texture());
        
        MTL::CommandBuffer* cmd = command_queue->commandBuffer();
        MTL::RenderCommandEncoder* enc = cmd->renderCommandEncoder(rpd);
        
        model = glm::rotate(model, 0.01f, glm::vec3(0.0f, 0.0f, 1.0f));
        
        enc->setRenderPipelineState(pipeline);
        enc->setVertexBuffer(vertex_buffer, 0, 0);
        enc->setVertexBytes(glm::value_ptr(model), sizeof(simd::float4x4), 1);
        enc->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle,
           NS::UInteger(6),
           MTL::IndexTypeUInt16,
           index_buffer,
           NS::UInteger(0),
           NS::UInteger(1)
       );
        
        enc->endEncoding();
        cmd->presentDrawable(drawable);
        cmd->commit();
        
        pool->release();
    }
}

//AppDelegate::~AppDelegate() {
//    view->release();
//    window->release();
//    device->release();
//    delete view_delegate;
//}
//
NS::Menu* Application::createMenuBar() {
    NS::Menu* mainMenu = NS::Menu::alloc()->init();
    
    NS::MenuItem* appMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu* appMenu = NS::Menu::alloc()->init(
        NS::String::string("Appname", UTF8StringEncoding)
    );
    
    NS::String* appName = NS::RunningApplication::currentApplication()
        ->localizedName();
    
    NS::String* quitItemName = NS::String::string(
        "Quit ",
        UTF8StringEncoding
    )->stringByAppendingString(appName);
    
    SEL quitCallback = NS::MenuItem::registerActionCallback(
        "appQuit",
        [](void*, SEL, const NS::Object* sender) {
            ///TODO: Make application a singleton and close properly with
            /// glfwSetWindowShouldClose(window, GLFW_TRUE);
            auto app = NS::Application::sharedApplication();
            app->terminate(sender);
        }
    );
    
    NS::MenuItem* appQuitItem = appMenu->addItem(
        quitItemName,
        quitCallback,
        NS::String::string("q", UTF8StringEncoding)
    );
    appQuitItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);
    appMenuItem->setSubmenu(appMenu);
    
    NS::MenuItem* windowMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu* windowMenu = NS::Menu::alloc()->init(
        NS::String::string("Window", UTF8StringEncoding)
    );
    
    SEL closeWindowCallback = NS::MenuItem::registerActionCallback (
        "windowClose",
        [](void*, SEL, const NS::Object* sender) {
            ///TODO: Make application a singleton and close properly with
            /// glfwSetWindowShouldClose(window, GLFW_TRUE);
            auto app = NS::Application::sharedApplication();
            app->terminate(sender);
        }
    );
    
    NS::MenuItem* closeWindowItem = windowMenu->addItem(
        NS::String::string("Close Window", UTF8StringEncoding),
        closeWindowCallback,
        NS::String::string("w", UTF8StringEncoding)
    );
    closeWindowItem->setKeyEquivalentModifierMask(
        NS::EventModifierFlagCommand
    );
    
    windowMenuItem->setSubmenu(windowMenu);
    
    mainMenu->addItem(appMenuItem);
    mainMenu->addItem(windowMenuItem);
    
    appMenuItem->release();
    windowMenuItem->release();
    appMenu->release();
    windowMenu->release();
    
    return mainMenu->autorelease();
}
//
//void AppDelegate::applicationWillFinishLaunching(
//    NS::Notification* notification
//) {
//    NS::Menu* menu = createMenuBar();
//    NS::Application* app = reinterpret_cast<NS::Application*>(
//        notification->object()
//    );
//    app->setMainMenu(menu);
//    app->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
//}
//
//void AppDelegate::applicationDidFinishLaunching(
//    NS::Notification* notification
//) {
//    CGRect frame = (CGRect){{100.0, 100.0}, {800.0, 600.0}};
//
//    window = NS::Window::alloc()->init(
//        frame,
//        NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
//        NS::BackingStoreBuffered,
//        false
//    );
//
//    device = MTL::CreateSystemDefaultDevice();
//
//    view = MTK::View::alloc()->init(frame, device);
//    view->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
//    view->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0));
//
//    view_delegate = new ViewDelegate(device);
//    view->setDelegate(view_delegate);
//
//    window->setContentView(view);
//    window->setTitle(
//        NS::String::string("MetalTest", NS::StringEncoding::UTF8StringEncoding)
//    );
//
//    window->makeKeyAndOrderFront(nullptr);
//
//    NS::Application* app = reinterpret_cast< NS::Application* >(
//        notification->object()
//    );
//    app->activateIgnoringOtherApps(true);
//}
//
//bool AppDelegate::applicationShouldTerminateAfterLastWindowClosed(
//    NS::Application* sender
//) {
//    return true;
//}
//
//ViewDelegate::ViewDelegate(MTL::Device* device): 
//    MTK::ViewDelegate(), 
//    device(device->retain()),
//    model(glm::mat4(1.0f))
//{
//    command_queue = device->newCommandQueue();
//    buildQuad();
//    pipeline = buildShader(
//        "shaders/triangle.metal",
//        "vertex_main",
//        "frag_main"
//    );
//}
//
void Application::buildTriangle() {
    Vertex vertices[] = {
        {{-0.75, -0.75, 0.0}},
        {{0.75, -0.75, 0.0}},
        {{0.0, 0.75, 0.0}},
    };
    ushort indices[] = {0, 1, 2};
    index_count = 3;
    
    vertex_buffer = device->newBuffer(
        3 * sizeof(Vertex),
        MTL::ResourceStorageModeManaged
    );
    
    memcpy(vertex_buffer->contents(), vertices, 3 * sizeof(Vertex));
    vertex_buffer->didModifyRange(NS::Range(0, 3));
    
    index_buffer = device->newBuffer(
        index_count * sizeof(ushort),
        MTL::ResourceStorageModeManaged
    );
    memcpy(index_buffer->contents(), indices, index_count * sizeof(Vertex));
    index_buffer->didModifyRange(NS::Range(0, index_count));
}

void Application::buildQuad() {
    Vertex vertices[] = {
        {{-0.75, -0.75, 0.0}},
        {{0.75, -0.75, 0.0}},
        {{0.75, 0.75, 0.0}},
        {{-0.75, 0.75, 0.0}},
    };
    ushort indices[] = {0, 1, 2, 2, 3, 0};
    index_count = 6;
    
    vertex_buffer = device->newBuffer(
        4 * sizeof(Vertex),
        MTL::ResourceStorageModeManaged
    );
    memcpy(vertex_buffer->contents(), vertices, 4 * sizeof(Vertex));
    vertex_buffer->didModifyRange(NS::Range(0, 4));
    
    index_buffer = device->newBuffer(
        index_count * sizeof(ushort),
        MTL::ResourceStorageModeManaged
    );
    memcpy(index_buffer->contents(), indices, index_count * sizeof(Vertex));
    index_buffer->didModifyRange(NS::Range(0, index_count));
}

MTL::RenderPipelineState* Application::buildShader(
   std::string filename,
   std::string vert_name,
   std::string frag_name
) {
    std::ifstream file;
    file.open(filename);
    std::stringstream reader;
    reader << file.rdbuf();
    std::string raw_string = reader.str();
    
    NS::String* source = NS::String::string(
        raw_string.c_str(), 
        UTF8StringEncoding
    );

    NS::Error* error = nullptr;
    MTL::CompileOptions* options = nullptr;
    MTL::Library* library = device->newLibrary(source, options, &error);
    if (!library) {
        std::cerr << error->localizedDescription()->utf8String() << std::endl;
    }

    MTL::Function* vert_fn = library->newFunction(
        NS::String::string(vert_name.c_str(), UTF8StringEncoding)
    );
    MTL::Function* frag_fn = library->newFunction(
        NS::String::string(frag_name.c_str(), UTF8StringEncoding)
    );
    
    MTL::RenderPipelineDescriptor* descriptor;
    descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    descriptor->setVertexFunction(vert_fn);
    descriptor->setFragmentFunction(frag_fn);
    descriptor->colorAttachments()->object(0)->setPixelFormat(
        MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB
    );
    
    MTL::RenderPipelineState* pipeline = device->newRenderPipelineState(
        descriptor, 
        &error
    );
    if (!pipeline) {
        std::cerr << error->localizedDescription()->utf8String() << std::endl;
    }
    
    descriptor->release();
    vert_fn->release();
    frag_fn->release();
    library->release();
    
    return pipeline;
}
//
//ViewDelegate::~ViewDelegate() {
//    vertex_buffer->release();
//    index_buffer->release();
//    pipeline->release();
//    command_queue->release();
//    device->release();
//}
//
//void ViewDelegate::drawInMTKView(MTK::View* view) {
//    model = glm::rotate(model, 0.01f, glm::vec3(0.0f, 0.0f, 1.0f));
//    
//    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
//
//    MTL::CommandBuffer* cmd = command_queue->commandBuffer();
//    MTL::RenderPassDescriptor* rpd = view->currentRenderPassDescriptor();
//    MTL::RenderCommandEncoder* enc = cmd->renderCommandEncoder( rpd );
//    
//    enc->setRenderPipelineState(pipeline);
//    enc->setVertexBuffer(vertex_buffer, 0, 0);
//    enc->setVertexBytes(glm::value_ptr(model), sizeof(simd::float4x4), 1);
//    enc->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle,
//       NS::UInteger(6),
//       MTL::IndexTypeUInt16,
//       index_buffer,
//       NS::UInteger(0),
//       NS::UInteger(1)
//   );
//    
//    enc->endEncoding();
//    cmd->presentDrawable(view->currentDrawable());
//    cmd->commit();
//
//    pool->release();
//}
