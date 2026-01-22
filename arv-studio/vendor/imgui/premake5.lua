-- ImGui include directory for other projects
IMGUI_INCLUDE_DIR = path.getabsolute("src")
IMGUI_BACKENDS_DIR = path.getabsolute("src/backends")

project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    files {
        -- Core ImGui files
        "src/imgui.cpp",
        "src/imgui_demo.cpp",
        "src/imgui_draw.cpp",
        "src/imgui_tables.cpp",
        "src/imgui_widgets.cpp",
        -- GLFW backend (platform)
        "src/backends/imgui_impl_glfw.cpp",
        -- OpenGL3 backend (renderer)
        "src/backends/imgui_impl_opengl3.cpp",
        -- Metal backend (renderer) - .mm is auto-detected as Obj-C++
        "src/backends/imgui_impl_metal.mm"
    }

    includedirs {
        "src",
        "src/backends"
    }

    sysincludedirs {
        -- GLFW headers from the metal provider
        "../../../providers/macos_metal_provider/vendor/GLFW/src/include"
    }

    -- Note: ImGui will use its own built-in OpenGL3 loader (imgui_impl_opengl3_loader.h)

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
