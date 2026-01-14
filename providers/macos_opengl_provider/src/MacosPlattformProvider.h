#pragma once
#include "plattform/PlattformProvider.h"

namespace arv
{
    class MacosPlattformProvider : public PlattformProvider
    {
    public:
        MacosPlattformProvider();
        void Init() override;
    };
}
