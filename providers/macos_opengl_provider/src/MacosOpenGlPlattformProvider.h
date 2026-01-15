#pragma once
#include "PlattformProvider.h"

namespace arv
{
    class MacosOpenGlPlattformProvider : public PlattformProvider
    {
    public:
        MacosOpenGlPlattformProvider();
        ~MacosOpenGlPlattformProvider() override;

        void Init() override;
    };
}
