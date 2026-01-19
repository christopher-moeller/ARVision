#include "MacosOpenGlGlfwCanvas.h"
#include "ARVBase.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace arv
{
    MacosOpenGlGlfwCanvas::MacosOpenGlGlfwCanvas()
    {
    }

    MacosOpenGlGlfwCanvas::~MacosOpenGlGlfwCanvas()
    {
        Destroy();
    }

    void MacosOpenGlGlfwCanvas::Init(PlatformApplicationContext* context)
    {
        if (!glfwInit())
        {
            ARV_LOG_ERROR("Failed to initialize GLFW");
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        m_window = glfwCreateWindow(800, 600, "ARVision (OpenGL)", nullptr, nullptr);
        if (!m_window)
        {
            ARV_LOG_ERROR("Failed to create GLFW window");
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            ARV_LOG_ERROR("Failed to initialize GLAD");
            return;
        }

    
    
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
        


        ARV_LOG_INFO("MacosOpenGlGlfwCanvas::Init() - Window created successfully");
        ARV_LOG_INFO("OpenGL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    }

    void MacosOpenGlGlfwCanvas::PollEvents()
    {
        glfwPollEvents();
    }

    void MacosOpenGlGlfwCanvas::SwapBuffers()
    {
        if (m_window)
        {
            glfwSwapBuffers(m_window);
        }
    }

    void MacosOpenGlGlfwCanvas::Destroy()
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
        glfwTerminate();
    }

    bool MacosOpenGlGlfwCanvas::ShouldClose()
    {
        return m_window && glfwWindowShouldClose(m_window);
    }
}
