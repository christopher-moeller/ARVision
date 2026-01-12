#pragma once

#include "ARVCoreConfiguration.h"

namespace arv
{
    class ARVApplication
    {
    public:
        explicit ARVApplication(ARVCoreConfiguration* coreConfiguration);
        void Initialize();
    private:
        ARVCoreConfiguration* m_coreConfiguration;
    };
}
