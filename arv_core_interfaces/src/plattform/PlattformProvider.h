#pragma once

namespace arv
{
    class PlattformProvider
    {
    public:
        PlattformProvider() = default;
        virtual void Init() = 0;
    };
}
