#pragma once

#include <sstream>

namespace arv {

    enum class EventType
    {
        None = 0,
        WindowClose,
        ApplicationResizeEvent,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        ScreenTouchedEvent,
        KeyPressedEvent,
        KeyReleasedEvent
        
    };

    class Event {

    public:
        virtual ~Event() = default;

        virtual EventType GetType() = 0;
        virtual const char* GetName() const = 0;
        virtual std::string ToString() const { return GetName(); }

        bool handled = false;  // Layers can mark events as consumed
    };

    class WindowCloseEvent : public Event {
        
    public:
        inline EventType GetType() override { return EventType::WindowClose; };
        inline const char * GetName() const override { return "WindowClose"; };
        
    };


    class MouseEvent : public Event {
        
    public:
        inline double GetXPos() const { return m_XPos; };
        inline double GetYPos() const { return m_YPos; };
        
        std::string ToString() const override {
            std::stringstream ss;
            ss << GetName() << " (" << GetXPos() << "|" << GetYPos() << ")";
            return ss.str();
        }
    protected:
        MouseEvent(double x, double y) : m_XPos(x), m_YPos(y) {}
    private:
        double m_XPos;
        double m_YPos;
        
    };

    class MouseButtonPressedEvent : public MouseEvent {
    public:
        MouseButtonPressedEvent(double x, double y, int button) : MouseEvent(x, y), m_Button(button) {}
        inline int GetButton() { return m_Button; };
        
        inline EventType GetType() override { return EventType::MouseButtonPressed; };
        inline const char * GetName() const override { return "MouseButtonPressed"; };
    private:
        int m_Button;
    };

    class ScreenTouchedEvent : public Event {
    public:
        
        ScreenTouchedEvent(double x, double y) : m_XPos(x), m_YPos(y) {}
        
        inline double GetXPos() const { return m_XPos; };
        inline double GetYPos() const { return m_YPos; };
        
        std::string ToString() const override {
            std::stringstream ss;
            ss << GetName() << " (" << GetXPos() << "|" << GetYPos() << ")";
            return ss.str();
        }
        
        inline EventType GetType() override { return EventType::ScreenTouchedEvent; };
        inline const char * GetName() const override { return "ScreenTouchedEvent"; };

    private:
        double m_XPos;
        double m_YPos;
    };

    class KeyEvent : public Event {
    public:
        inline int GetKeyCode() const { return m_Keycode; }
        
    protected:
        KeyEvent(int keycode) : m_Keycode(keycode) {}
        
        int m_Keycode;
    };

    class KeyPressedEvent : public KeyEvent {
        
    public:
        KeyPressedEvent(int keyCode, bool isRepeat = false) : KeyEvent(keyCode), m_IsRepeat(isRepeat) {}
        
        bool IsRepeat() const { return m_IsRepeat; }
        
        inline EventType GetType() override { return EventType::KeyPressedEvent; }
        const char* GetName() const override { return "KeyPressedEvent"; }
        
        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_Keycode << " (repeat = " << m_IsRepeat << ")";
            return ss.str();
        }
        
    private:
        bool m_IsRepeat;
        
    };

    class KeyReleasedEvent : public KeyEvent {
        
    public:
        KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}
        
        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << m_Keycode;
            return ss.str();
        }
        
        EventType GetType() override { return EventType::KeyReleasedEvent; }
        const char* GetName() const override { return "KeyReleased"; }
        
    };

    class ApplicationResizeEvent : public Event {
    public:
        ApplicationResizeEvent(int width, int height) : m_Width(width), m_Height(height) {}
        
        inline int GetWidth() { return m_Width; }
        inline int GetHeight() { return m_Height; }
        
        std::string ToString() const override {
            std::stringstream ss;
            ss << "ApplicationResizeEvent: " << m_Width << ", " << m_Height;
            return ss.str();
        }
        
        EventType GetType() override { return EventType::ApplicationResizeEvent; }
        const char* GetName() const override { return "ApplicationResizeEvent"; }
        
    private:
        int m_Width, m_Height;
    };


    inline std::ostream& operator<<(std::ostream& os, const Event& e) {
        return os << e.ToString();
    }
}