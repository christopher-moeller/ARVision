-- Include ImGui vendor library
include "vendor/imgui"

project "arv-studio"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    systemversion "10.15"

    files { "src/**.cpp", "src/**.h", "src/**.mm" }

    includedirs {
        "../arv_core_interfaces/src",
        "../providers/macos_metal_provider/src",
        "../providers/macos_opengl_provider/src",
        "../arv_core/src"
    }

    sysincludedirs {
        GLM_INCLUDE_DIR,
        IMGUI_INCLUDE_DIR,
        IMGUI_BACKENDS_DIR,
        -- GLFW headers for ImGui layer
        "../providers/macos_metal_provider/vendor/GLFW/src/include",
        -- nlohmann JSON for JsonSceneParser
        "../arv_core/vendor/nlohmann"
    }

    libdirs {
        "../arv_core/bin/Debug",
        "../arv_core/bin/Release",

        "../providers/macos_metal_provider/bin/Debug",
        "../providers/macos_metal_provider/bin/Release",
        "../providers/macos_metal_provider/vendor/GLFW/bin/Debug",
        "../providers/macos_metal_provider/vendor/GLFW/bin/Release",

        "../providers/macos_opengl_provider/bin/Debug",
        "../providers/macos_opengl_provider/bin/Release",
        "../providers/macos_opengl_provider/vendor/GLFW/bin/Debug",
        "../providers/macos_opengl_provider/vendor/GLFW/bin/Release",
        "../providers/macos_opengl_provider/vendor/glad/bin/Debug",
        "../providers/macos_opengl_provider/vendor/glad/bin/Release",

        -- ImGui library
        "vendor/imgui/bin/Debug",
        "vendor/imgui/bin/Release"
    }

    links {
        "arv_core",
        "arv_macos_opengl_provider",
        "arv_macos_metal_provider",
        "GLFW",
        "glad",
        "ImGui",
        "Cocoa.framework",
        "OpenGL.framework",
        "IOKit.framework",
        "CoreVideo.framework",
        "CoreFoundation.framework",
        "QuartzCore.framework",
        "Metal.framework",
        "MetalKit.framework"
    }

    dependson { "ImGui" }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
