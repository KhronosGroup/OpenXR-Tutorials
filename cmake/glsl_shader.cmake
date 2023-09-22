# Copyright (c) 2019-2023, The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0
find_package(Vulkan QUIET)

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
find_program(
    GLSLANG_VALIDATOR glslangValidator
    HINTS ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
)
if(GLSL_COMPILER)
    message(STATUS "Found glslc: ${GLSL_COMPILER}")
elseif(GLSLANG_VALIDATOR)
    message(STATUS "Found glslangValidator: ${GLSLANG_VALIDATOR}")
endif()

function(glsl_spv_shader)
    set(options GENERATE_HEADER HAVE_PRECOMPILED)
    set(oneValueArgs INPUT OUTPUT STAGE TARGET_ENV)
    set(multiValueArgs EXTRA_DEPENDS)
    cmake_parse_arguments(
        _glsl_spv
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if(GLSL_COMPILER)
        if(_glsl_spv_GENERATE_HEADER)
            set(_glsl_spv_output_type_arg "-mfmt=c")
        else()
            set(_glsl_spv_output_type_arg)
        endif()
        add_custom_command(
            OUTPUT "${_glsl_spv_OUTPUT}"
            COMMAND
                "${GLSL_COMPILER}" #
                ${_glsl_spv_output_type_arg} #
                "-fshader-stage=${_glsl_spv_STAGE}" #
                "${_glsl_spv_INPUT}" #
                -o "${_glsl_spv_OUTPUT}" #
                "--target-env=${_glsl_spv_TARGET_ENV}" #
                $<IF:$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>,-g,> #
                $<IF:$<CONFIG:Debug>,-O0,-O> #
            MAIN_DEPENDENCY "${_glsl_spv_INPUT}"
            DEPENDS "${_glsl_spv_INPUT}" ${_glsl_spv_EXTRA_DEPENDS}
            USES_TERMINAL VERBATIM
        )

    elseif(GLSLANG_VALIDATOR)
        if(_glsl_spv_GENERATE_HEADER)
            set(_glsl_spv_output_type_arg "-x")
        else()
            set(_glsl_spv_output_type_arg)
        endif()
        add_custom_command(
            OUTPUT "${_glsl_spv_OUTPUT}"
            COMMAND
                "${GLSLANG_VALIDATOR}" #
                -S "${_glsl_spv_STAGE}" #
                #--nan-clamp #
                -x # output as hex
                -o "${_glsl_spv_OUTPUT}" #
                $<$<CONFIG:Debug,RelWithDebInfo>:-gVS> #
                $<$<CONFIG:Debug>:-Od> #
                $<$<CONFIG:Release>:-g0> #
                "--target-env" "${_glsl_spv_TARGET_ENV}" #
                "${_glsl_spv_INPUT}" #
            MAIN_DEPENDENCY "${_glsl_spv_INPUT}"
            DEPENDS "${_glsl_spv_INPUT}" ${_glsl_spv_EXTRA_DEPENDS}
            USES_TERMINAL VERBATIM
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
