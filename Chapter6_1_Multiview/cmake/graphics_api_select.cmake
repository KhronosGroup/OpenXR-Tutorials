# Copyright 2023, The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0

include_guard()

set(XR_TUTORIAL_GRAPHICS_API "VULKAN" CACHE STRING "Which graphics API to use when building the tutorial projects.")
if(WIN32)
    set_property(CACHE XR_TUTORIAL_GRAPHICS_API PROPERTY STRINGS D3D11 D3D12 OPENGL VULKAN)
elseif(LINUX AND NOT ANDROID)
    set_property(CACHE XR_TUTORIAL_GRAPHICS_API PROPERTY STRINGS OPENGL VULKAN)
elseif(ANDROID)
    set_property(CACHE XR_TUTORIAL_GRAPHICS_API PROPERTY STRINGS OPENGL_ES VULKAN)
endif()

function(AddGraphicsAPIDefine PROJECT_NAME)
    target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_GRAPHICS_API=${XR_TUTORIAL_GRAPHICS_API})
endfunction(AddGraphicsAPIDefine)
