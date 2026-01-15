#include "MacosMetalGlfwCanvas.h"

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Cocoa/Cocoa.h>

#include <iostream>

namespace arv
{
    MacosMetalGlfwCanvas::MacosMetalGlfwCanvas()
    {
    }

    MacosMetalGlfwCanvas::~MacosMetalGlfwCanvas()
    {
        Destroy();
    }

    void MacosMetalGlfwCanvas::Init(PlattformApplicationContext* context)
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }

        // Tell GLFW not to create an OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_window = glfwCreateWindow(800, 600, "ARVision (Metal)", nullptr, nullptr);
        if (!m_window)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }

        // Get the native Cocoa window
        NSWindow* nsWindow = glfwGetCocoaWindow(m_window);
        if (!nsWindow)
        {
            std::cerr << "Failed to get Cocoa window" << std::endl;
            return;
        }

        // Create the Metal device
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device)
        {
            std::cerr << "Failed to create Metal device" << std::endl;
            return;
        }

        // Create Metal layer and configure it
        m_metalLayer = [CAMetalLayer layer];
        m_metalLayer.device = device;
        m_metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        m_metalLayer.framebufferOnly = YES;

        // Get the content view and set up the Metal layer
        NSView* contentView = [nsWindow contentView];
        [contentView setWantsLayer:YES];
        [contentView setLayer:m_metalLayer];

        // Set the drawable size
        CGSize drawableSize = contentView.bounds.size;
        drawableSize.width *= contentView.window.backingScaleFactor;
        drawableSize.height *= contentView.window.backingScaleFactor;
        m_metalLayer.drawableSize = drawableSize;

        std::cout << "MacosMetalGlfwCanvas::Init() - Window created successfully" << std::endl;
        std::cout << "Metal Device: " << [[device name] UTF8String] << std::endl;
    }

    void MacosMetalGlfwCanvas::PollEvents()
    {
        glfwPollEvents();
    }

    void MacosMetalGlfwCanvas::SwapBuffers()
    {
        // Metal handles presentation through the command buffer
        // SwapBuffers is a no-op for Metal, presentation happens in the rendering API
    }

    void MacosMetalGlfwCanvas::Destroy()
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
        m_metalLayer = nullptr;
        glfwTerminate();
    }

    bool MacosMetalGlfwCanvas::ShouldClose()
    {
        return m_window && glfwWindowShouldClose(m_window);
    }
}
