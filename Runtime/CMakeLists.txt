# Copyright mzoesch. All rights reserved.

include_guard(_guard)
if(_guard)
    jafg_add_module(JAFG_MODULE_TYPE_LAUNCH)
    jafg_add_dependency("Engine/Engine" "private")
    jafg_add_dependency("Engine/Core" "private")

    if(JAFG_TARGET_TYPE STREQUAL JAFG_TARGET_TESTUNIT)
        jafg_add_dependency("Engine/Tester" "private")
    endif()
endif()
