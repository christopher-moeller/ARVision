#include "MacosOpenGlRenderingAPI.h"
#include <iostream>

namespace arv
{
    MacosOpenGlRenderingAPI::MacosOpenGlRenderingAPI()
    {
    }

    MacosOpenGlRenderingAPI::~MacosOpenGlRenderingAPI()
    {
    }

    void MacosOpenGlRenderingAPI::Init()
    {
        std::cout << "MacosOpenGlRenderingAPI::Init()" << std::endl;
    }

    void MacosOpenGlRenderingAPI::Draw()
    {
        std::cout << "MacosOpenGlRenderingAPI::Draw()" << std::endl;
    }
}
