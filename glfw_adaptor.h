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

/// @brief Get the metal-cpp NS::Window from the GLFW window
/// Works with void pointers because including metal-cpp in the objective c file
/// presented casting errors.
/// @param window The GLFW window.
/// @param layer The CA::MetalLayer to place on the window.
/// @return The NS::Window to do native window activities with.
void* get_ns_window(GLFWwindow* window, void* layer);

#endif /* glfw_adaptor_h */
