# Copyright mzoesch. All rights reserved.

###############################################################################
# Try to automatically predict the best configuration if not given.
# This means:
#   - Check the host system and set the JAFG_TARGET_PLATFORM variable to the host system if not set and Jafg supports
#     compiling for it.
#   - Set the JAFG_TARGET_TYPE variable to Client if not set.
#   - Set the JAFG_TARGET_CONFIG variable to Shipping if not set.
###############################################################################

if(NOT DEFINED JAFG_TARGET_PLATFORM)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        set(JAFG_TARGET_PLATFORM    "lnx")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Emscripten")
        set(JAFG_TARGET_PLATFORM    "wasm")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        set(JAFG_TARGET_PLATFORM    "win")
    else()
        message(
            FATAL_ERROR
            "JAFG_TARGET_PLATFORM is not defined and could not be predicted for the host system [${CMAKE_HOST_SYSTEM_NAME}]. "
            "Please define it with -DJAFG_TARGET_PLATFORM=<platform> or use a given preset with --preset \"<preset>\"."
            )
    endif()
    message(WARNING
        "JAFG_TARGET_PLATFORM was not defined. "
        "Tried to predict the best platform for this host system [${CMAKE_HOST_SYSTEM_NAME}]. "
        "JAFG_TARGET_PLATFORM is implicitly set to [${JAFG_TARGET_PLATFORM}]."
        )
endif()

if(NOT DEFINED JAFG_TARGET_TYPE)
    message(WARNING "JAFG_TARGET_TYPE was not defined. Setting it implicitly to [Client].")
    set(JAFG_TARGET_TYPE "cl")
endif()

if(NOT DEFINED JAFG_TARGET_CONFIG)
    message(WARNING "JAFG_TARGET_CONFIG was not defined. Setting it implicitly to [Shipping].")
    set(JAFG_TARGET_CONFIG "ship")
endif()
