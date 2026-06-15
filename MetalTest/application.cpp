//
//  application.cpp
//  MetalTest
//
//  Created by Dot32  on 15/6/2026.
//

#include "application.h"

using NS::StringEncoding::UTF8StringEncoding;

AppDelegate::~AppDelegate() {
    view->release();
    window->release();
    device->release();
    delete viewDelegate;
}

NS::Menu* AppDelegate::createMenuBar() {
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
        [](void*, SEL, const NS::Object*) {
            auto app = NS::Application::sharedApplication();
            app->windows()->object<NS::Window>(0)->close();
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

void AppDelegate::applicationWillFinishLaunching(
    NS::Notification* notification
) {
    NS::Menu* menu = createMenuBar();
    NS::Application* app = reinterpret_cast<NS::Application*>(
        notification->object()
    );
    app->setMainMenu(menu);
    app->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
}

void AppDelegate::applicationDidFinishLaunching(
    NS::Notification* notification
) {
    CGRect frame = (CGRect){{100.0, 100.0}, {800.0, 600.0}};

    window = NS::Window::alloc()->init(
        frame,
        NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
        NS::BackingStoreBuffered,
        false
    );

    device = MTL::CreateSystemDefaultDevice();

    view = MTK::View::alloc()->init(frame, device);
    view->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    view->setClearColor(MTL::ClearColor::Make(1.0, 0.0, 0.0, 1.0));

    viewDelegate = new MTKViewDelegate(device);
    view->setDelegate(viewDelegate);

    window->setContentView(view);
    window->setTitle(
        NS::String::string("MetalTest", NS::StringEncoding::UTF8StringEncoding)
    );

    window->makeKeyAndOrderFront(nullptr);

    NS::Application* app = reinterpret_cast< NS::Application* >(
        notification->object()
    );
    app->activateIgnoringOtherApps(true);
}

bool AppDelegate::applicationShouldTerminateAfterLastWindowClosed(
    NS::Application* sender
) {
    return true;
}

MTKViewDelegate::MTKViewDelegate(MTL::Device* device): 
    MTK::ViewDelegate(), 
    device(device->retain())
{
    commandQueue = device->newCommandQueue();
}

MTKViewDelegate::~MTKViewDelegate() {
    commandQueue->release();
    device->release();
}

void MTKViewDelegate::drawInMTKView( MTK::View* view ) {
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer* cmd = commandQueue->commandBuffer();
    MTL::RenderPassDescriptor* rpd = view->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* enc = cmd->renderCommandEncoder( rpd );
    enc->endEncoding();
    cmd->presentDrawable(view->currentDrawable());
    cmd->commit();

    pool->release();
}
