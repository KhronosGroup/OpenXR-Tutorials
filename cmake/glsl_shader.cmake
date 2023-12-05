# Copyright (c) 2019-2023, The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0
find_package(Vulkan QUIET)

# find glslc
if(ANDROID)
    file(GLOB glslc_folders ${ANDROID_NDK}/shader-tools/*)
    find_program(
        GLSL_COMPILER glslc
        PATHS ${glslc_folders}
        NO_DEFAULT_PATH
    )
else()
    file(GLOB glslc_folders $ENV{VULKAN_SDK}/*)
    find_program(
        GLSL_COMPILER glslc
        PATHS ${glslc_folders}
        HINTS ${Vulkan_GLSLC_EXECUTABLE}
    )
endif()

# glslangValidator
find_program(
    GLSLANG_VALIDATOR glslangValidator
    HINTS ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
)

# log found spirv compilers
if(GLSL_COMPILER)
    message(STATUS "Found glslc: ${GLSL_COMPILER}")
elseif(GLSLANG_VALIDATOR)
    message(STATUS "Found glslangValidator: ${GLSLANG_VALIDATOR}")
endif()

function(glsl_spv_shader)
    set(options GENERATE_HEADER HAVE_PRECOMPILED)
    set(oneValueArgs
        INPUT
        OUTPUT
        STAGE
        ENTRY_POINT
        TARGET_ENV
    )
    set(multiValueArgs EXTRA_DEPENDS)
    cmake_parse_arguments(
        _glsl_spv
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if(GLSL_COMPILER)
        add_custom_command(
            OUTPUT "${_glsl_spv_OUTPUT}"
            COMMAND
                "${GLSL_COMPILER}"
                -o "${_glsl_spv_OUTPUT}"
                "-fshader-stage=${_glsl_spv_STAGE}"
                "-fentry-point=${_glsl_spv_ENTRY_POINT}"
                $<IF:$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>,-g,>
                $<IF:$<CONFIG:Debug>,-O0,-O>
                "--target-env=${_glsl_spv_TARGET_ENV}"
                "${_glsl_spv_INPUT}"
            MAIN_DEPENDENCY "${_glsl_spv_INPUT}"
            DEPENDS "${_glsl_spv_INPUT}" ${_glsl_spv_EXTRA_DEPENDS}
            USES_TERMINAL VERBATIM COMMAND_EXPAND_LISTS
        )
    elseif(GLSLANG_VALIDATOR)
        add_custom_command(
            OUTPUT "${_glsl_spv_OUTPUT}"
            COMMAND
                "${GLSLANG_VALIDATOR}"
                -o "${_glsl_spv_OUTPUT}"
                -S "${_glsl_spv_STAGE}"
                -e "${_glsl_spv_ENTRY_POINT}"
                $<$<CONFIG:Debug,RelWithDebInfo>:-gVS>
                $<$<CONFIG:Debug>:-Od>
                $<$<CONFIG:Release>:-g0>
                "--target-env" "${_glsl_spv_TARGET_ENV}"
                -V 
                "${_glsl_spv_INPUT}"
            MAIN_DEPENDENCY "${_glsl_spv_INPUT}"
            DEPENDS "${_glsl_spv_INPUT}" ${_glsl_spv_EXTRA_DEPENDS}
            USES_TERMINAL VERBATIM COMMAND_EXPAND_LISTS
        )

    elseif(_glsl_spv_HAVE_PRECOMPILED)
        # Use the precompiled .spv files
        get_filename_component(glsl_src_dir "${_glsl_spv_INPUT}" DIRECTORY)

        get_filename_component(glsl_name_we "${_glsl_spv_INPUT}" NAME_WE)
        set(precompiled_file ${glsl_src_dir}/${glsl_name_we}.spv)
        configure_file("${precompiled_file}" "${_glsl_spv_OUTPUT}" COPYONLY)
    else()
        message(
            FATAL_ERROR
                "Cannot compile shaders - no glslc or glslangValidator - and no precompiled versions found"
        )
    endif()
endfunction()
