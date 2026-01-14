project "arv_core_interfaces"
    kind "StaticLib" 
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    files {
        "src/**.h"
    }

    sysincludedirs {
        GLM_INCLUDE_DIR
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
