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

int main(int argc, char* argv[]) {
    NS::AutoreleasePool* autoreleasePool = NS::AutoreleasePool::alloc()->init();

    Application* app = new Application();
    app->run();
    delete app;

    autoreleasePool->release();

    return 0;
}
