#pragma once
#include "PlatformProvider.h"

namespace arv
{
    class MacosOpenGlPlatformProvider : public PlatformProvider
    {
    public:
        MacosOpenGlPlatformProvider();
        ~MacosOpenGlPlatformProvider() override;

        void Init(PlatformApplicationContext* context) override;
    };
}
