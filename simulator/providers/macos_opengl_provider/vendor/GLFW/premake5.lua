project "GLFW"
    kind "StaticLib"
    language "C"
    staticruntime "off"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    files {
        "src/src/init.c",
        "src/src/input.c",
        "src/src/monitor.c",
        "src/src/window.c",
        "src/src/context.c",
        "src/src/platform.c",
        "src/src/vulkan.c",
        "src/src/null_init.c",
        "src/src/null_monitor.c",
        "src/src/null_window.c",
        "src/src/null_joystick.c",
        "src/src/cocoa_init.m",
        "src/src/cocoa_monitor.m",
        "src/src/cocoa_window.m",
        "src/src/cocoa_joystick.m",
        "src/src/cocoa_time.c",
        "src/src/nsgl_context.m",
        "src/src/posix_thread.c",
        "src/src/posix_module.c",
        "src/src/egl_context.c",
        "src/src/osmesa_context.c"
    }

    includedirs {
        "src/include"
    }

    defines {
        "_GLFW_COCOA"
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
