#pragma once

#include "ARVApplication.h"

#define PROVIDER_OPENGL 1
#define PROVIDER_METAL 2

#define ARV_LOG_INFO(...) arv::ARVApplication::Get()->GetLogger()->Info(__VA_ARGS__)
#define ARV_LOG_ERROR(...) arv::ARVApplication::Get()->GetLogger()->Error(__VA_ARGS__)
