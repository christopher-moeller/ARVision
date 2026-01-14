project "sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    systemversion "10.15"

    files { "src/**.cpp", "src/**.h" }

    includedirs { "../arv_core_interfaces/src" }
    includedirs { "../providers/macos_opengl_provider/src" }
    includedirs { "../arv_core/src" }

    libdirs { "../arv_core/bin/Debug",
              "../arv_core/bin/Release",
              "../providers/macos_opengl_provider/bin/Debug",
              "../providers/macos_opengl_provider/bin/Release" }

    links { "arv_core", "arv_macos_opengl_provider" }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
