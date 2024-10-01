# Copyright (c) 2019-2023, The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0

if(WIN32)
    find_program(FXC_EXECUTABLE fxc)
endif()
function(fxc_shader)
    set(options GENERATE_HEADER)
    set(oneValueArgs
        INPUT
        OUTPUT
        OUTPUT_PDB
        TYPE
        VARIABLE
        PROFILE
        ENTRY_POINT
    )
    set(multiValueArgs EXTRA_DEPENDS)
    cmake_parse_arguments(
        _fxc
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if(FXC_EXECUTABLE)
        set(_fxc "${FXC_EXECUTABLE}")
    else()
        # Hope/assume that it will be in the path at build time
        set(_fxc "fxc.exe")
    endif()

    if(_fxc_GENERATE_HEADER)
        if(NOT _fxc_VARIABLE)
            message(
                FATAL_ERROR
                    "fxc_shader: Need to specify a VARIABLE to generate if generating a header"
            )
        endif()
        set(_fxc_output_args /Vn "${_fxc_VARIABLE}" /Fh)
    else()
        set(_fxc_output_args /Fo)
    endif()

    add_custom_command(
        OUTPUT "${_fxc_OUTPUT}"
        BYPRODUCTS "${_fxc_OUTPUT_PDB}"
        COMMAND
            "${_fxc}"
            /nologo
            ${_fxc_output_args} ${_fxc_OUTPUT}
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/Fd>
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:${_fxc_OUTPUT_PDB}>
            /T${_fxc_PROFILE}
            /E${_fxc_ENTRY_POINT}
            $<$<CONFIG:Debug>:/Od>
            $<$<CONFIG:Debug>:/Zss>
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/Zi>
            "${_fxc_INPUT}"
        MAIN_DEPENDENCY "${_fxc_INPUT}"
        DEPENDS "${_fxc_INPUT}" ${_fxc_EXTRA_DEPENDS}
        COMMENT "FXC: ${_fxc_INPUT}"
        USES_TERMINAL
        VERBATIM
        COMMAND_EXPAND_LISTS
    )

endfunction()
