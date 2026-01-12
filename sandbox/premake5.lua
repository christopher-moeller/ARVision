workspace "sandbox"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    location "build"

project "sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    systemversion "10.15"

    files { "src/**.cpp", "src/**.h" }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
