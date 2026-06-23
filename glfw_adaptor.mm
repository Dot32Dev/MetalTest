//
//  glfw_adaptor.mm
//  MetalTest
//
//  Created by Dot32  on 20/6/2026.
//

#import <Foundation/Foundation.h>
#include "glfw_adaptor.h"

NS::Window* get_ns_window(GLFWwindow* window, CA::MetalLayer* layer) {
    CALayer* obj_layer = (__bridge CALayer*)layer;
    NSWindow* obj_window = glfwGetCocoaWindow(window);
    
    obj_window.contentView.layer = obj_layer;
    obj_window.contentView.wantsLayer = YES;
    
    return (__bridge NS::Window*)obj_window;
}
