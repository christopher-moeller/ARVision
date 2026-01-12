#include <iostream>
#include "ARVApplication.h"

int main()
{
    arv::ARVCoreProvider coreProvider;
    arv::ARVApplication app(coreProvider);
    app.Initialize();
    return 0;
}
