//
//  main.cpp
//  MetalTest
//
//  Created by Dot32  on 15/6/2026.
//

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "application.h"
#include <iostream>
#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
//
//int main(int argc, const char * argv[]) {
//    NS::AutoreleasePool* autoreleasePool = NS::AutoreleasePool::alloc()->init();
//    
//    AppDelegate delegate;
//    
//    NS::Application* app = NS::Application::sharedApplication();
//    app->setDelegate(&delegate);
//    app->run();
//    
//    autoreleasePool->release();
//    return 0;
//}

#include <GLFW/glfw3.h>

int main() {
    if (!glfwInit())
        return -1;

    GLFWwindow* window =
        glfwCreateWindow(800, 600, "Hello", nullptr, nullptr);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
