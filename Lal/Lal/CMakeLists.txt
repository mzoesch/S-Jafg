# Copyright mzoesch. All rights reserved.

include_guard(_guard)
if(_guard)
    jafg_add_module(JAFG_MODULE_TYPE_STATIC)
    jafg_add_dependency("Engine/Core" "private")
endif()
