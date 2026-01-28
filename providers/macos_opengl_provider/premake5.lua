include "vendor/GLFW"
include "vendor/glad"

project "arv_macos_opengl_provider"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    files { "src/**.cpp", "src/**.h" }

    includedirs {
        "../../arv_core_interfaces/src",
        "../../arv_core/src"
    }

    sysincludedirs {
        "vendor/GLFW/src/include",
        "vendor/glad/include",
        GLM_INCLUDE_DIR,
        STB_INCLUDE_DIR,
        path.getabsolute("../../arv_core/vendor/tinyexr"),
        path.getabsolute("../../arv_core/vendor/nlohmann")
    }

    dependson { "GLFW", "glad" }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
