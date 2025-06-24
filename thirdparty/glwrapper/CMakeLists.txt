# Copyright 2023, The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0

# Built from OpenXR to provide OpenGL (ES) support

include(${CMAKE_CURRENT_LIST_DIR}/glad2/CMakeLists.txt)


if(NOT TARGET tutorial_glwrapper)
    if(ANDROID)
        find_package(OpenGLES REQUIRED COMPONENTS V31)
        find_package(EGL REQUIRED)
    else()
        find_package(OpenGL)
    endif()

    if(OpenGL_FOUND OR OpenGLES_FOUND)
        add_library(
                tutorial_glwrapper STATIC
                ${CMAKE_CURRENT_LIST_DIR}/gfxwrapper_opengl.c
        )
        target_include_directories(
                tutorial_glwrapper PUBLIC
                ${CMAKE_CURRENT_LIST_DIR}
        )
        if(ANDROID)
            target_link_libraries(tutorial_glwrapper PUBLIC tutorial-glad-loader)
            target_link_libraries(
                    tutorial_glwrapper PUBLIC ${OpenGLES_V3_LIBRARY} EGL::EGL
            )
        else()
            target_link_libraries(tutorial_glwrapper PUBLIC OpenGL::GL tutorial-glad-loader)
        endif()
    endif()

    # Determine the presentation backend for Linux systems.
    # Use an include because the code is pretty big.
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        include(presentation)
    endif()

endif()
