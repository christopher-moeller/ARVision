workspace "arv_dev_workspace"
    configurations { "Debug", "Release" }
    location "build"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Vendor libraries (header-only)
include "vendor/GLM"

include "arv_core_interfaces"
include "arv_core"
include "providers/macos_opengl_provider"
include "providers/macos_metal_provider"
include "arv-studio"
