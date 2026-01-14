include "vendor/GLFW"

project "arv_macos_opengl_provider"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    files { "src/**.cpp", "src/**.h" }

    includedirs {
        "../../arv_core_interfaces/src"
    }

    sysincludedirs {
        "vendor/GLFW/src/include",
        GLM_INCLUDE_DIR
    }

    dependson { "GLFW" }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
