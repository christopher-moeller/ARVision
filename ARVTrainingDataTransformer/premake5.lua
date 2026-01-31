-- ARVTrainingDataTransformer premake5 configuration
workspace "ARVTrainingDataTransformer"
    configurations { "Debug", "Release" }
    architecture "x86_64"
    location "build"

project "ARVTrainingDataTransformer"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
    objdir "%{wks.location}/obj/%{cfg.buildcfg}"

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
        "src",
        "vendor",
        "vendor/glm"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Speed"

    filter "system:macosx"
        xcodebuildsettings {
            ["MACOSX_DEPLOYMENT_TARGET"] = "12.0"
        }
