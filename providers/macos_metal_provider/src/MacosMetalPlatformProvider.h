#pragma once
#include "PlatformProvider.h"

namespace arv
{
    class MacosMetalPlatformProvider : public PlatformProvider
    {
    public:
        MacosMetalPlatformProvider();
        ~MacosMetalPlatformProvider() override;

        void Init(PlatformApplicationContext* context) override;
    };
}
