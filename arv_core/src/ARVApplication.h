#pragma once

#include "ARVCoreProvider.h"

namespace arv
{
    class ARVApplication
    {
    public:
        explicit ARVApplication(ARVCoreProvider& coreProvider);
        void Initialize();
    private:
        ARVCoreProvider& m_coreProvider;
    };
}
