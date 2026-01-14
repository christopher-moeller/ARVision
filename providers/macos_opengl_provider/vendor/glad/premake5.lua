project "glad"
    kind "StaticLib"
    language "C"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    files {
        "src/glad.c"
    }

    includedirs {
        "include"
    }

    externalincludedirs { "include" } -- This is needed for XCode

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
