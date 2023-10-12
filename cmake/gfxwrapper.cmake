# Copyright 2023, The Khronos Group Inc.
#
# SPDX-License-Identifier: MIT

# Build from OpenXR to provide OpenGL (ES) support
if(NOT TARGET openxr-gfxwrapper)
    if(ANDROID)
        find_package(OpenGLES REQUIRED COMPONENTS V31)
        find_package(EGL REQUIRED)
    else()
        find_package(OpenGL)
    endif()

    if(OpenGL_FOUND OR OpenGLES_FOUND)
        add_library(
                openxr-gfxwrapper STATIC
                ${openxr_SOURCE_DIR}/src/common/gfxwrapper_opengl.c
                ${openxr_SOURCE_DIR}/src/common/gfxwrapper_opengl.h
        )
        target_include_directories(
                openxr-gfxwrapper PUBLIC
                ${openxr_SOURCE_DIR}/src/common
                ${openxr_SOURCE_DIR}/external/include
        )
        if(ANDROID)
            target_link_libraries(
                    openxr-gfxwrapper PUBLIC ${OpenGLES_V3_LIBRARY} EGL::EGL
            )
        else()
            target_link_libraries(openxr-gfxwrapper PUBLIC OpenGL::GL)
        endif()
    endif()
endif()