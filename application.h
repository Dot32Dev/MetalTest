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
#define GLFW_INCLUDE_NONE

#include <glm/glm.hpp>
#include <MetalKit/MetalKit.hpp>
#include <GLFW/glfw3.h>
#include "camera.h"

class Application {
    public:
        /// @brief Constructor for Application
        Application();
        
        /// @brief Destructor for Application
        ~Application();

        /// @brief Run the application
        void run();
    private:
        Camera camera;
        GLFWwindow* glfw_window;
        MTL::Device* device;
        CA::MetalLayer* layer;
        NS::Window* window;
        CA::MetalDrawable* drawable;
        MTL::RenderPassDescriptor* rpd;
        MTL::RenderPassColorAttachmentDescriptor* colour_attachment;
        MTL::DepthStencilState* depth_stencil_state;
        MTL::CommandQueue* command_queue;
        MTL::RenderPipelineState* pipeline;
        MTL::Buffer* vertex_buffer;
        MTL::Buffer* index_buffer;
        MTL::Texture* texture;
        MTL::SamplerState* sampler;
        int index_count;
        double last_frame;
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        void loadTerrain(std::string file_name, int size);
        void buildTriangle();
        void buildQuad();
        MTL::RenderPipelineState* buildShader(
            std::string vert_name,
            std::string frag_name
        );
        NS::Menu* createMenuBar();
        static void resize(GLFWwindow* glfw_window, int width, int height);
};

#endif
