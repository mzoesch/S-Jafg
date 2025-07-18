# Copyright mzoesch. All rights reserved.

#
# An option, that, if not defined in CMake, will be automatically resolved in C++ code to a default value
# based of the target platform, target type and target configuration.
#
macro(unspecified_option
    flag_name
    flag_description
    )
    if(NOT DEFINED CACHE{${flag_name}})
        set(
            ${flag_name}
            "Unspecified"
            CACHE STRING
            "${flag_description}"
            )
    endif()
    set_property(
        CACHE ${flag_name} PROPERTY STRINGS
        "Unspecified" "1" "0"
        )
endmacro()

#
# A flag that, if not defined in CMake, will be automatically resolved in C++ code to a default value
# based of the target platform, target type and target configuration.
#
macro(unspecified_flag
    flag_name
    flag_description
    )
    if(NOT DEFINED CACHE{${flag_name}})
        set(
            ${flag_name}
            "Unspecified"
            CACHE STRING
            "${flag_description}"
            )
    endif()
endmacro()
