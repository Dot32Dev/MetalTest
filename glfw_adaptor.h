//
//  glfw_adaptor.hpp
//  MetalTest
//
//  Created by Dot32  on 20/6/2026.
//

#ifndef glfw_adaptor_h
#define glfw_adaptor_h

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

// Including metal-cpp headers into the Objective C environment has a fit about
// casting, so instead we forward declare the types 
namespace NS {class Window;}
namespace CA {class MetalLayer;}

/// @brief Get the metal-cpp NS::Window from the GLFW window
/// GLFW only returns the Objective C "ID", and we need the casted C++ wrapper, so I wrap the GLFW
/// function in here.
/// @param window The GLFW window.
/// @param layer The CA::MetalLayer to place on the window.
/// @return The NS::Window to do native window activities with.
NS::Window* get_ns_window(GLFWwindow* window, CA::MetalLayer* layer);

#endif /* glfw_adaptor_h */
