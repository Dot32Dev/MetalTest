//
//  application.cpp
//  MetalTest
//
//  Created by Dot32  on 15/6/2026.
//

#include "application.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <simd/simd.h>

using NS::StringEncoding::UTF8StringEncoding;

struct Vertex {
    simd::float3 pos;
};

AppDelegate::~AppDelegate() {
    view->release();
    window->release();
    device->release();
    delete view_delegate;
}

NS::Menu* AppDelegate::createMenuBar() {
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
        [](void*, SEL, const NS::Object*) {
            auto app = NS::Application::sharedApplication();
            app->windows()->object<NS::Window>(0)->close();
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

void AppDelegate::applicationWillFinishLaunching(
    NS::Notification* notification
) {
    NS::Menu* menu = createMenuBar();
    NS::Application* app = reinterpret_cast<NS::Application*>(
        notification->object()
    );
    app->setMainMenu(menu);
    app->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
}

void AppDelegate::applicationDidFinishLaunching(
    NS::Notification* notification
) {
    CGRect frame = (CGRect){{100.0, 100.0}, {800.0, 600.0}};

    window = NS::Window::alloc()->init(
        frame,
        NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
        NS::BackingStoreBuffered,
        false
    );

    device = MTL::CreateSystemDefaultDevice();

    view = MTK::View::alloc()->init(frame, device);
    view->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    view->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0));

    view_delegate = new ViewDelegate(device);
    view->setDelegate(view_delegate);

    window->setContentView(view);
    window->setTitle(
        NS::String::string("MetalTest", NS::StringEncoding::UTF8StringEncoding)
    );

    window->makeKeyAndOrderFront(nullptr);

    NS::Application* app = reinterpret_cast< NS::Application* >(
        notification->object()
    );
    app->activateIgnoringOtherApps(true);
}

bool AppDelegate::applicationShouldTerminateAfterLastWindowClosed(
    NS::Application* sender
) {
    return true;
}

ViewDelegate::ViewDelegate(MTL::Device* device): 
    MTK::ViewDelegate(), 
    device(device->retain())
{
    command_queue = device->newCommandQueue();
    buildQuad();
    pipeline = buildShader(
        "shaders/triangle.metal",
        "vertex_main",
        "frag_main"
    );
}

void ViewDelegate::buildTriangle() {
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

void ViewDelegate::buildQuad() {
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

MTL::RenderPipelineState* ViewDelegate::buildShader(
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

ViewDelegate::~ViewDelegate() {
    vertex_buffer->release();
    index_buffer->release();
    pipeline->release();
    command_queue->release();
    device->release();
}

void ViewDelegate::drawInMTKView(MTK::View* view) {
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer* cmd = command_queue->commandBuffer();
    MTL::RenderPassDescriptor* rpd = view->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* enc = cmd->renderCommandEncoder( rpd );
    
    enc->setRenderPipelineState(pipeline);
    enc->setVertexBuffer(vertex_buffer, 0, 0);
    enc->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle,
       NS::UInteger(6),
       MTL::IndexTypeUInt16,
       index_buffer,
       NS::UInteger(0),
       NS::UInteger(1)
   );
    
    enc->endEncoding();
    cmd->presentDrawable(view->currentDrawable());
    cmd->commit();

    pool->release();
}
