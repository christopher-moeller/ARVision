#pragma once

#include "Camera.h"
#include <memory>
#include "events/EventManager.h"
#include "../utils/Timestep.h"


namespace arv {

    class CameraControllerAppContext {
    public:
        CameraControllerAppContext(EventManager* eventManager, const Timestep& timestep) : m_EventManager(eventManager), m_Timestep(timestep) {}
        
        bool IsKeyPressed(int keyCode);
        
        inline const Timestep& GetTimestep() const { return m_Timestep; }
    private:
        EventManager* m_EventManager;
        Timestep m_Timestep;
    };

    enum class DeviceType
    {
        None = 0,
        DESKTOP_COMPUTER,
        MOBILE_DEVICE
        
    };

    class CameraController {
    public:
        CameraController(Camera* camera, DeviceType deviceType);
        
        void Init();
        void UpdateOnStep(CameraControllerAppContext& context);
        
        virtual ~CameraController() = default;
        
    protected:
        Camera* m_Camera;
        DeviceType m_DeviceType;
        
    protected:
        virtual void InitForDesktopComputer() {};
        virtual void UpdateOnStepForDesktopComputer(CameraControllerAppContext& context) {};
        
        virtual void InitForMobileDevice() {};
        virtual void UpdateOnStepForMobileDevice(CameraControllerAppContext& context) {};
    };

}
