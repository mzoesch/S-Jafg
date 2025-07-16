# Copyright mzoesch. All rights reserved.

# The following are all platforms that Jafg supports.
set(JAFG_PLATFORM_LINUX     "Linux")
set(JAFG_PLATFORM_WINDOWS   "Windows")
set(JAFG_PLATFORM_WASM      "Wasm")

# The following are all architectures that Jafg supports. We only differentiate between x86 and x86_64.
set(JAFG_ARCHITECTURE_x64   "x86_64")
set(JAFG_ARCHITECTURE_x86   "x86")

# The following are all target types that Jafg supports.
set(JAFG_TARGET_CLIENT      "Client")
set(JAFG_TARGET_DAEMON      "Daemon")
set(JAFG_TARGET_TESTUNIT    "TestUnit")

# The following are all configurations that Jafg supports.
set(JAFG_CONFIG_DEBUG       "Debug")
set(JAFG_CONFIG_DEVELOPMENT "Development")
set(JAFG_CONFIG_SHIPPING    "Shipping")

###############################################################################
# Validate the target input.
###############################################################################

set(JAFG_ALLOWED_PLATFORMS
    ${JAFG_PLATFORM_LINUX}
    ${JAFG_PLATFORM_WINDOWS}
    ${JAFG_PLATFORM_WASM}
    )

set(JAFG_ALLOWED_ARCHITECTURES
    ${JAFG_ARCHITECTURE_x64}
    ${JAFG_ARCHITECTURE_x86}
    )

set(JAFG_ALLOWED_TARGET_TYPES
    ${JAFG_TARGET_CLIENT}
    ${JAFG_TARGET_DAEMON}
    ${JAFG_TARGET_TESTUNIT}
    )

set(JAFG_ALLOWED_CONFIGURATIONS
    ${JAFG_CONFIG_DEBUG}
    ${JAFG_CONFIG_DEVELOPMENT}
    ${JAFG_CONFIG_SHIPPING}
    )

function(SetJafgTargetArchitecture)
    if(DEFINED JAFG_TARGET_ARCHITECTURE)
        message(FATAL_ERROR "JAFG_TARGET_ARCHITECTURE is already defined as ${JAFG_TARGET_ARCHITECTURE}.")
    endif()

    set(arch_processor "${CMAKE_SYSTEM_PROCESSOR}")

    if(arch_processor STREQUAL "AMD64")
        set(JAFG_TARGET_ARCHITECTURE ${JAFG_ARCHITECTURE_x64} PARENT_SCOPE)
        message(STATUS "JAFG_TARGET_ARCHITECTURE is not defined. Defaulting to ${JAFG_ARCHITECTURE_x64}.")
    elseif(arch_processor STREQUAL "x86_64")
        set(JAFG_TARGET_ARCHITECTURE ${JAFG_ARCHITECTURE_x64} PARENT_SCOPE)
        message(STATUS "JAFG_TARGET_ARCHITECTURE is not defined. Defaulting to ${JAFG_ARCHITECTURE_x64}.")
    elseif(arch_processor STREQUAL "x86")
        set(JAFG_TARGET_ARCHITECTURE ${JAFG_ARCHITECTURE_x86} PARENT_SCOPE)
        message(STATUS "JAFG_TARGET_ARCHITECTURE is not defined. Defaulting to ${JAFG_ARCHITECTURE_x86}.")

    # Fallback to the usual architecture for the current target platform if defined.
    elseif(DEFINED JAFG_TARGET_PLATFORM)
        if(JAFG_TARGET_PLATFORM STREQUAL ${JAFG_PLATFORM_LINUX})
            set(JAFG_TARGET_ARCHITECTURE ${JAFG_ARCHITECTURE_x64} PARENT_SCOPE)
            message(WARNING "JAFG_TARGET_ARCHITECTURE is not defined. Defaulting to ${JAFG_ARCHITECTURE_x64} for Linux.")
        elseif(JAFG_TARGET_PLATFORM STREQUAL ${JAFG_PLATFORM_WINDOWS})
            set(JAFG_TARGET_ARCHITECTURE ${JAFG_ARCHITECTURE_x64} PARENT_SCOPE)
            message(WARNING "JAFG_TARGET_ARCHITECTURE is not defined. Defaulting to ${JAFG_ARCHITECTURE_x64} for Windows.")
        elseif(JAFG_TARGET_PLATFORM STREQUAL ${JAFG_PLATFORM_WASM})
            set(JAFG_TARGET_ARCHITECTURE ${JAFG_ARCHITECTURE_x86} PARENT_SCOPE)
            message(WARNING "JAFG_TARGET_ARCHITECTURE is not defined. Defaulting to ${JAFG_ARCHITECTURE_x86} for Wasm.")
        endif()
    endif()
endfunction()

function(ValidateTargetInput)
    if(NOT DEFINED JAFG_TARGET_PLATFORM)
        message(FATAL_ERROR "JAFG_TARGET_PLATFORM is not defined. Please define it with -DJAFG_TARGET_PLATFORM=<platform>.")
    endif()
    if(NOT DEFINED JAFG_TARGET_ARCHITECTURE)
        message(FATAL_ERROR "JAFG_TARGET_ARCHITECTURE is not defined. Please define it with -DJAFG_TARGET_ARCHITECTURE=<architecture>.")
    endif()
    if(NOT DEFINED JAFG_TARGET_TYPE)
        message(FATAL_ERROR "JAFG_TARGET_TYPE is not defined. Please define it with -DJAFG_TARGET_TYPE=<target>.")
    endif()
    if(NOT DEFINED JAFG_TARGET_CONFIG)
        message(FATAL_ERROR "JAFG_TARGET_CONFIG is not defined. Please define it with -JAFG_TARGET_CONFIG=<configuration>.")
    endif()

    if(NOT JAFG_TARGET_PLATFORM IN_LIST JAFG_ALLOWED_PLATFORMS)
        message(FATAL_ERROR "Invalid JAFG_TARGET_PLATFORM: ${JAFG_TARGET_PLATFORM}. Allowed values are: ${JAFG_ALLOWED_PLATFORMS}.")
    endif()
    if(NOT JAFG_TARGET_ARCHITECTURE IN_LIST JAFG_ALLOWED_ARCHITECTURES)
        message(FATAL_ERROR "Invalid JAFG_TARGET_ARCHITECTURE: ${JAFG_TARGET_ARCHITECTURE}. Allowed values are: ${JAFG_ALLOWED_ARCHITECTURES}.")
    endif()
    if(NOT JAFG_TARGET_TYPE IN_LIST JAFG_ALLOWED_TARGET_TYPES)
        message(FATAL_ERROR "Invalid JAFG_TARGET_TYPE: ${JAFG_TARGET_TYPE}. Allowed values are: ${JAFG_ALLOWED_TARGET_TYPES}.")
    endif()
    if(NOT JAFG_TARGET_CONFIG IN_LIST JAFG_ALLOWED_CONFIGURATIONS)
        message(FATAL_ERROR "Invalid JAFG_TARGET_CONFIG: ${JAFG_TARGET_CONFIG}. Allowed values are: ${JAFG_ALLOWED_CONFIGURATIONS}.")
    endif()
endfunction()

if(NOT DEFINED JAFG_TARGET_ARCHITECTURE)
    SetJafgTargetArchitecture()
endif()

if(NOT DEFINED JAFG_TARGET_ARCHITECTURE)
    message(FATAL_ERROR "Could not resolve JAFG_TARGET_ARCHITECTURE automatically. Please define it with -DJAFG_TARGET_ARCHITECTURE=<architecture>.")
endif()

ValidateTargetInput()

message(STATUS "JAFG_TARGET_PLATFORM: ${JAFG_TARGET_PLATFORM}")
message(STATUS "JAFG_TARGET_ARCHITECTURE: ${JAFG_TARGET_ARCHITECTURE}")
message(STATUS "JAFG_TARGET_TYPE: ${JAFG_TARGET_TYPE}")
message(STATUS "JAFG_TARGET_CONFIG: ${JAFG_TARGET_CONFIG}")

#
# The compound target name. This is used for folder structure and output directories.
# As compound names for directories are easier to understand and faster to traverse through.
#
set(JAFG_TARGET_COMPOUND "${JAFG_TARGET_TYPE}-${JAFG_TARGET_CONFIG}")
set(JAFG_PLATFORM_COMPOUND "${JAFG_TARGET_PLATFORM}-${JAFG_TARGET_ARCHITECTURE}")
set(JAFG_COMPOUND_CONFIG_PATH "${JAFG_PLATFORM_COMPOUND}/${JAFG_TARGET_COMPOUND}")
