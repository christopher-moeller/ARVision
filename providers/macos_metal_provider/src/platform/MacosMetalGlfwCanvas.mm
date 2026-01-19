#include "MacosMetalGlfwCanvas.h"

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Cocoa/Cocoa.h>

#include "ARVBase.h"

namespace arv
{
    MacosMetalGlfwCanvas::MacosMetalGlfwCanvas()
    {
    }

    MacosMetalGlfwCanvas::~MacosMetalGlfwCanvas()
    {
        Destroy();
    }

    void MacosMetalGlfwCanvas::Init(PlatformApplicationContext* context)
    {
        if (!glfwInit())
        {
            ARV_LOG_ERROR("Failed to initialize GLFW");
            return;
        }

        // Tell GLFW not to create an OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_window = glfwCreateWindow(800, 600, "ARVision (Metal)", nullptr, nullptr);
        if (!m_window)
        {
            ARV_LOG_ERROR("Failed to create GLFW window");
            glfwTerminate();
            return;
        }

        // Get the native Cocoa window
        NSWindow* nsWindow = glfwGetCocoaWindow(m_window);
        if (!nsWindow)
        {
            ARV_LOG_ERROR("Failed to get Cocoa window");
            return;
        }

        // Create the Metal device
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device)
        {
            ARV_LOG_ERROR("Failed to create Metal device");
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

        glfwSetWindowUserPointer(m_window, context->GetEventManager());
        GLFWwindow* glfwwindow = m_window;

        context->GetEventManager()->SetKeyPressedPollCallback([glfwwindow](int keycode) {
            auto state = glfwGetKey(glfwwindow, static_cast<int32_t>(keycode));
            return state == GLFW_PRESS;
        });


        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
            
            EventManager* eventManager = (EventManager*) glfwGetWindowUserPointer(window);
            
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            
            switch (action) {
                case GLFW_PRESS:
                {
                    //eventManager->Test();
                    MouseButtonPressedEvent event(xpos, ypos, button);
                    eventManager->PushEvent(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    
                    break;
                }
            }
            
        });
        
        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
            EventManager* eventManager = (EventManager*) glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            eventManager->PushEvent(event);
        });
        
        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            EventManager* eventManager = (EventManager*) glfwGetWindowUserPointer(window);
            
            
            switch (action) {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(key, 0);
                    eventManager->PushEvent(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(key);
                    eventManager->PushEvent(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(key, 1);
                    eventManager->PushEvent(event);
                    break;
                }
                    
            }
        });
        
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
            
        });
        
        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
            EventManager* eventManager = (EventManager*) glfwGetWindowUserPointer(window);
            
            ApplicationResizeEvent event(width, height);
            eventManager->PushEvent(event);
        });

        ARV_LOG_INFO("MacosMetalGlfwCanvas::Init() - Window created successfully");
        ARV_LOG_INFO("Metal Device: {}", [[device name] UTF8String]);
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
