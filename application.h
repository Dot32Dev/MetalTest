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

class ViewDelegate : public MTK::ViewDelegate {
    public:
        ViewDelegate(MTL::Device* device);
        virtual ~ViewDelegate() override;
        virtual void drawInMTKView(MTK::View* view) override;
    private:
        MTL::Device* device;
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
};

class AppDelegate : public NS::ApplicationDelegate {
    public:
        ~AppDelegate();
        NS::Menu* createMenuBar();  
        virtual void applicationWillFinishLaunching(
            NS::Notification* notification
        ) override;
        
        virtual void applicationDidFinishLaunching(
            NS::Notification* notification
        ) override;
        
        virtual bool applicationShouldTerminateAfterLastWindowClosed(
            NS::Application* sender
        ) override;
    private:
        NS::Window* window;
        MTK::View* view;
        MTL::Device* device;
        ViewDelegate* view_delegate = nullptr;
};

#endif
