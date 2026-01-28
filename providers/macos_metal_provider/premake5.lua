include "vendor/GLFW"

project "arv_macos_metal_provider"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    files { "src/**.mm", "src/**.h" }

    includedirs {
        "../../arv_core_interfaces/src",
        "../../arv_core/src"
    }

    sysincludedirs {
        "vendor/GLFW/src/include",
        GLM_INCLUDE_DIR,
        STB_INCLUDE_DIR,
        path.getabsolute("../../arv_core/vendor/tinyexr"),
        path.getabsolute("../../arv_core/vendor/nlohmann")
    }

    links {
        "GLFW",
        "Cocoa.framework",
        "IOKit.framework",
        "CoreVideo.framework",
        "Metal.framework",
        "QuartzCore.framework"
    }

    dependson { "GLFW" }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
