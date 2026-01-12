project "arv_core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{cfg.buildcfg}" 
    objdir "obj/%{cfg.buildcfg}"

    files { "src/**.cpp", "src/**.h" }

    includedirs { "../arv_core_interfaces/src" }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
