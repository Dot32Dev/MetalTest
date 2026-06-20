//
//  glfw_adaptor.hpp
//  MetalTest
//
//  Created by Dot32  on 20/6/2026.
//

#ifndef glfw_adaptor_h
#define glfw_adaptor_h

//#include <Metal/Metal.hpp>
//#include <AppKit/AppKit.hpp>
////#include <MetalKit/MetalKit.hpp>
//#include <QuartzCore/QuartzCore.hpp>
//#include <AppKit/AppKit.hpp>
//#include <QuartzCore/CAMetalLayer.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

void* get_ns_window(GLFWwindow* window, void* layer);

#endif /* glfw_adaptor_h */
