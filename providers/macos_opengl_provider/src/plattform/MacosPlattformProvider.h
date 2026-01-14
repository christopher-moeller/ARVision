#pragma once
#include "PlattformProvider.h"

namespace arv
{
    class MacosPlattformProvider : public PlattformProvider
    {
    public:
        MacosPlattformProvider();
        ~MacosPlattformProvider() override;

        void Init() override;
    };
}
