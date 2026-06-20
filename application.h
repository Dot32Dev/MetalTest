//
//  application.h
//  MetalTest
//
//  Created by Dot32  on 15/6/2026.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <MetalKit/MetalKit.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Application {
    public:
        Application();
        ~Application();
        void run();
    private:
        GLFWwindow* glfw_window;
        MTL::Device* device;
        CA::MetalLayer* layer;
        NS::Window* window;
        CA::MetalDrawable* drawable;
        MTL::RenderPassDescriptor* rpd;
        MTL::RenderPassColorAttachmentDescriptor* colour_attachment;
        MTL::CommandQueue* command_queue;
        MTL::RenderPipelineState* pipeline;
        MTL::Buffer* vertex_buffer;
        MTL::Buffer* index_buffer;
        int index_count;
        glm::mat4 model;
        void buildTriangle();
        void buildQuad();
        MTL::RenderPipelineState* buildShader(
            std::string filename,
            std::string vert_name,
            std::string frag_name
        );
        NS::Menu* createMenuBar();
};

#endif
