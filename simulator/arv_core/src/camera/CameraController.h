#pragma once

#include "Camera.h"
#include <memory>
#include "events/EventManager.h"
#include "../utils/Timestep.h"

namespace arv {

    class CameraControllerAppContext {
    public:
        CameraControllerAppContext(EventManager* eventManager, const Timestep& timestep)
            : m_EventManager(eventManager), m_Timestep(timestep) {}

        bool IsKeyPressed(int keyCode);

        const Timestep& GetTimestep() const { return m_Timestep; }

    private:
        EventManager* m_EventManager;
        Timestep m_Timestep;
    };

    // Forward declaration
    template<typename CameraType>
    class CameraController;

    // Input strategy interface - handles device-specific input
    template<typename CameraType>
    class InputStrategy {
    public:
        virtual ~InputStrategy() = default;
        virtual void Init(CameraController<CameraType>& controller) = 0;
        virtual void Update(CameraController<CameraType>& controller, CameraControllerAppContext& context) = 0;
    };

    // Camera controller template - eliminates duplicate pointers
    template<typename CameraType>
    class CameraController {
    public:
        CameraController(CameraType* camera, std::unique_ptr<InputStrategy<CameraType>> inputStrategy)
            : m_Camera(camera)
            , m_InputStrategy(std::move(inputStrategy))
        {}

        virtual ~CameraController() = default;

        void Init() {
            if (m_InputStrategy) {
                m_InputStrategy->Init(*this);
            }
        }

        void UpdateOnStep(CameraControllerAppContext& context) {
            if (m_InputStrategy) {
                m_InputStrategy->Update(*this, context);
            }
        }

        CameraType* GetCamera() { return m_Camera; }
        const CameraType* GetCamera() const { return m_Camera; }

    protected:
        CameraType* m_Camera;
        std::unique_ptr<InputStrategy<CameraType>> m_InputStrategy;
    };

}
