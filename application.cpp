//
//  application.cpp
//  MetalTest
//
//  Created by Dot32  on 15/6/2026.
//

#define STB_IMAGE_IMPLEMENTATION

#include "application.h"
#include "glfw_adaptor.h"
#include "shaders/types.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <simd/simd.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

using NS::StringEncoding::UTF8StringEncoding;
using std::string;
using std::vector;
using std::ifstream;
using std::ios;

// Starting window size
const int WIN_W = 800;
const int WIN_H = 600;

Application::Application() {
    if (!glfwInit()) {
        std::cerr << "Could not init GLFW" << std::endl;
        return;
    }
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);

    glfw_window = glfwCreateWindow(WIN_W, WIN_H, "MetalTest", nullptr, nullptr);
    glfwSetWindowUserPointer(glfw_window, this);
    glfwSetFramebufferSizeCallback(glfw_window, Application::resize);

    device = MTL::CreateSystemDefaultDevice();
    layer = CA::MetalLayer::layer()->retain();
    layer->setDevice(device);
    layer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

    command_queue = device->newCommandQueue()->retain();
//    buildQuad();
    loadTerrain("height128.raw", 128);
    pipeline = buildShader(
        "vertex_main",
        "frag_main"
    );
    
    model = glm::mat4(1.0f);
    camera = Camera(glm::vec3(0.0f, 100.0f, -2.0f));
    view = camera.get_view_matrix();
    last_frame = 0.0;

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
    
    MTL::RenderPassDepthAttachmentDescriptor* depth_attachment;
    depth_attachment = rpd->depthAttachment();
    depth_attachment->setClearDepth(1.0);
    
    resize(glfw_window, WIN_W, WIN_H);
    
    MTL::DepthStencilDescriptor* depth_stencil_desc;
    depth_stencil_desc = MTL::DepthStencilDescriptor::alloc()->init();
    depth_stencil_desc->setDepthWriteEnabled(true);
    depth_stencil_desc->setDepthCompareFunction(
        MTL::CompareFunction::CompareFunctionLess
    );
    depth_stencil_state = device->newDepthStencilState(depth_stencil_desc);
    
    int img_w, img_h, img_chann;
    unsigned char *data = stbi_load(
        "rocky_terrain_03_diff_4k.jpg",
        &img_w,
        &img_h,
        &img_chann,
        STBI_rgb_alpha
    );
    
    MTL::TextureDescriptor* texture_desc;
    texture_desc = MTL::TextureDescriptor::texture2DDescriptor(
        MTL::PixelFormat::PixelFormatRGBA8Unorm,
        img_w,
        img_h,
        false
    );
    texture_desc->setUsage(MTL::TextureUsageRenderTarget);
    texture_desc->setStorageMode(MTL::StorageModeManaged);
    texture_desc->setUsage(MTL::TextureUsageShaderRead);
    texture = device->newTexture(texture_desc);
    MTL::Region copy_region = MTL::Region::Make2D(0, 0, img_w, img_h);
    texture->replaceRegion(copy_region, 0, data, img_w * 4);
    stbi_image_free(data);
    
    MTL::SamplerDescriptor* sampler_desc;
    sampler_desc = MTL::SamplerDescriptor::alloc()->init();
    sampler_desc->setMinFilter(MTL::SamplerMinMagFilterNearest);
    sampler_desc->setMagFilter(MTL::SamplerMinMagFilterLinear);
    sampler_desc->setMipFilter(MTL::SamplerMipFilterNearest);
    sampler_desc->setSAddressMode(MTL::SamplerAddressModeMirrorRepeat);
    sampler_desc->setTAddressMode(MTL::SamplerAddressModeMirrorRepeat);
    sampler = device->newSamplerState(sampler_desc);
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

void Application::loadTerrain(string file_name, int size) {
    ifstream infile(file_name, ios::binary);
    if (!infile) return;

    vector<unsigned char> terrain_data = vector<unsigned char>();

    // Get length of file
    infile.seekg(0, ios::end);
    int length = (int)infile.tellg();

    // Read data as block
    infile.seekg(0, ios::beg);
    terrain_data.resize(length);
    infile.read(reinterpret_cast<char*>(terrain_data.data()), length);
    infile.close();
    
    vector<Vertex> vert_vector = vector<Vertex>();
    vector<ushort> index_vector = vector<ushort>();
    
    for (int z = 0; z < size; z++) {
        for (int x = 0; x < size; x++) {
            int y = terrain_data[z * size + x];
            vert_vector.push_back({
                {(float)(x - size / 2), (float)(y)*0.5f, (float)(z - size / 2)},
                {((float)x / size) * 8.0f, ((float)z / size) * 8.0f}
            });
        }
    }

    for (int z = 0; z < size-1; z++) {
        for (int x = 0; x < size-1; x++) {
            // Triangle one
            index_vector.push_back(z * size + x); // Top left of tile
            index_vector.push_back(z * size + x + 1); // Top right of tile
            index_vector.push_back((z + 1) * size + x); // Bottom left of tile

            // Triangle two
            index_vector.push_back(z * size + x + 1); // Top right of tile
            index_vector.push_back((z + 1) * size + x + 1); // Bottom right
            index_vector.push_back((z + 1) * size + x); // Bottom left of tile
        }
    }
    
    index_count = (int)index_vector.size();
    
    vertex_buffer = device->newBuffer(
        vert_vector.size() * sizeof(Vertex),
        MTL::ResourceStorageModeManaged
    );
    memcpy(
        vertex_buffer->contents(),
        vert_vector.data(),
        vert_vector.size() * sizeof(Vertex)
    );
    vertex_buffer->didModifyRange(
        NS::Range(0, vert_vector.size() * sizeof(Vertex))
    );
      
    index_buffer = device->newBuffer(
        index_count * sizeof(ushort),
        MTL::ResourceStorageModeManaged
    );
    memcpy(index_buffer->contents(),
        index_vector.data(),
        index_count * sizeof(ushort)
    );
    index_buffer->didModifyRange(NS::Range(0, index_count * sizeof(ushort)));
}

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
    vertex_buffer->didModifyRange(NS::Range(0, 3 * sizeof(Vertex)));
    
    index_buffer = device->newBuffer(
        index_count * sizeof(ushort),
        MTL::ResourceStorageModeManaged
    );
    memcpy(index_buffer->contents(), indices, index_count * sizeof(ushort));
    index_buffer->didModifyRange(NS::Range(0, index_count * sizeof(ushort)));
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
    vertex_buffer->didModifyRange(NS::Range(0, 4  * sizeof(Vertex)));
    
    index_buffer = device->newBuffer(
        index_count * sizeof(ushort),
        MTL::ResourceStorageModeManaged
    );
    memcpy(index_buffer->contents(), indices, index_count * sizeof(ushort));
    index_buffer->didModifyRange(NS::Range(0, index_count * sizeof(ushort)));
}

MTL::RenderPipelineState* Application::buildShader(
   std::string vert_name,
   std::string frag_name
) {
    MTL::Library* library = device->newDefaultLibrary();
    if (!library) {
        std::cerr << "Shader load error" << std::endl;
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
    descriptor->setDepthAttachmentPixelFormat(
        MTL::PixelFormat::PixelFormatDepth32Float
    );
    
    NS::Error* error = nullptr;
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

void Application::resize(GLFWwindow* glfw_window, int width, int height) {
    void* user_pointer = glfwGetWindowUserPointer(glfw_window);
    Application* app = reinterpret_cast<Application*>(user_pointer);
    
    app->layer->setDrawableSize(
        CGSizeMake(width, height) // (Not retina)
    );
    
    app->projection = glm::perspective(
        glm::radians(90.0f),
        (float)width/height,
        0.2f,
        1000.0f
    );
    
    MTL::TextureDescriptor* depth_texture_desc;
    depth_texture_desc = MTL::TextureDescriptor::texture2DDescriptor(
        MTL::PixelFormat::PixelFormatDepth32Float,
        width,
        height,
        false
    );

    depth_texture_desc->setUsage(MTL::TextureUsageRenderTarget);
    depth_texture_desc->setStorageMode(MTL::StorageModePrivate);
    
    MTL::RenderPassDepthAttachmentDescriptor* depth_attachment;
    depth_attachment = app->rpd->depthAttachment();
    depth_attachment->setClearDepth(1.0);
    depth_attachment->setTexture(app->device->newTexture(depth_texture_desc));
}

void Application::run() {
    while (!glfwWindowShouldClose(glfw_window)) {
        glfwPollEvents();
        
        // Delta time
        double current_frame = glfwGetTime();
        double delta_time = current_frame - last_frame;
        last_frame = current_frame;
        
//        model = glm::rotate(
//            model,
//            (float)(0.5 * delta_time),
//            glm::vec3(0.0f, 0.0f, -1.0f)
//        );
        
        float movement_speed = 15.0 * delta_time;
        float rot_speed = 1.5 * delta_time;

        if (glfwGetKey(glfw_window, GLFW_KEY_X) == GLFW_PRESS) {
            movement_speed *= 2.0;
        }
        
        vec3 pos_input = vec3(0.0f, 0.0f, 0.0f);
        if (glfwGetKey(glfw_window, GLFW_KEY_W) == GLFW_PRESS)
            pos_input += vec3(0.0f, 0.0f, movement_speed);
        if (glfwGetKey(glfw_window, GLFW_KEY_S) == GLFW_PRESS)
            pos_input += vec3(0.0f, 0.0f, -movement_speed);
        if (glfwGetKey(glfw_window, GLFW_KEY_A) == GLFW_PRESS)
            pos_input += vec3(-movement_speed, 0.0f, 0.0f);
        if (glfwGetKey(glfw_window, GLFW_KEY_D) == GLFW_PRESS)
            pos_input += vec3(movement_speed, 0.0f, 0.0f);

        vec2 dir_input = vec2(0.0f, 0.0f);
        if (glfwGetKey(glfw_window, GLFW_KEY_UP) == GLFW_PRESS)
            dir_input += vec2(0.0f, rot_speed);
        if (glfwGetKey(glfw_window, GLFW_KEY_DOWN) == GLFW_PRESS)
            dir_input += vec2(0.0f, -rot_speed);
        if (glfwGetKey(glfw_window, GLFW_KEY_LEFT) == GLFW_PRESS)
            dir_input += vec2(rot_speed, 0.0f);
        if (glfwGetKey(glfw_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            dir_input += vec2(-rot_speed, 0.0f);

        // Send input to camera
        camera.pos_input(camera.get_target(), pos_input);
        camera.dir_input(camera.get_target(), dir_input);
        
        view = camera.get_view_matrix();
        
        // Render
        NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
        
        drawable = layer->nextDrawable();
        colour_attachment->setTexture(drawable->texture());
        
        MTL::CommandBuffer* cmd = command_queue->commandBuffer();
        MTL::RenderCommandEncoder* enc = cmd->renderCommandEncoder(rpd);
        
        enc->setRenderPipelineState(pipeline);
        enc->setDepthStencilState(depth_stencil_state);
//        enc->setTriangleFillMode(MTL::TriangleFillModeLines);
        
        enc->setVertexBuffer(vertex_buffer, 0, 0);
        enc->setVertexBytes(glm::value_ptr(model), sizeof(simd::float4x4), 1);
        enc->setVertexBytes(glm::value_ptr(view), sizeof(simd::float4x4), 2);
        enc->setFragmentTexture(texture, 0);
        enc->setFragmentSamplerState(sampler, 0);
        enc->setVertexBytes(
            glm::value_ptr(projection),
            sizeof(simd::float4x4),
            3
        );
        
        enc->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle,
           NS::UInteger(index_count),
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
