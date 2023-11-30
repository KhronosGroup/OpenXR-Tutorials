# Copyright (c) 2019-2023, The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0

if(WIN32)
    find_program(DXC_EXECUTABLE dxc)
endif()
function(dxc_shader)
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
        _dxc
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if(DXC_EXECUTABLE)
        set(_dxc "${DXC_EXECUTABLE}")
    else()
        # Hope/assume that it will be in the path at build time
        set(_dxc "dxc.exe")
    endif()

    set(_dxc_output_args /Fo)

    add_custom_command(
        OUTPUT "${_dxc_OUTPUT}"
        BYPRODUCTS "${_dxc_OUTPUT_PDB}"
        COMMAND
            "${_dxc}"
            /nologo
            ${_dxc_output_args} ${_dxc_OUTPUT}
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/Fd>
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:${_dxc_OUTPUT_PDB}>
            /T${_dxc_PROFILE}
            /E${_dxc_ENTRY_POINT}
            $<$<CONFIG:Debug>:/Od> 
            $<$<CONFIG:Debug>:/Zss>
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/Zi>
            "${_dxc_INPUT}"
        MAIN_DEPENDENCY "${_dxc_INPUT}"
        DEPENDS "${_dxc_INPUT}" ${_dxc_EXTRA_DEPENDS}
        COMMENT "dxc: ${_dxc_INPUT}"
        USES_TERMINAL
        VERBATIM
        COMMAND_EXPAND_LISTS
    )

endfunction()
