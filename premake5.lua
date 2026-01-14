workspace "arv_dev_workspace"
    configurations { "Debug", "Release" }
    location "build"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "arv_core_interfaces"
include "arv_core"
include "providers/macos_opengl_provider"
include "sandbox"
