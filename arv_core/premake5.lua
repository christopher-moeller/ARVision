project "arv_core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{cfg.buildcfg}" 
    objdir "obj/%{cfg.buildcfg}"

    files { "src/**.cpp", "src/**.h" }

    includedirs {
        "src",
        "../arv_core_interfaces/src"
    }

    sysincludedirs {
        GLM_INCLUDE_DIR,
        STB_INCLUDE_DIR,
        TINYOBJLOADER_INCLUDE_DIR,
        path.getabsolute("vendor/nlohmann"),
        path.getabsolute("vendor/tinyexr")
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
