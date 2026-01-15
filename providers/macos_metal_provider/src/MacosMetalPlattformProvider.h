#pragma once
#include "PlattformProvider.h"

namespace arv
{
    class MacosMetalPlattformProvider : public PlattformProvider
    {
    public:
        MacosMetalPlattformProvider();
        ~MacosMetalPlattformProvider() override;

        void Init() override;
    };
}
