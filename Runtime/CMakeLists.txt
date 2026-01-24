# Copyright mzoesch. All rights reserved.

include_guard(_guard)
if(_guard)
    jafg_add_module(JAFG_MODULE_TYPE_LAUNCH)
    jafg_add_dependency("Engine" "private")
    target_link_libraries(${_scoped_module_name} PUBLIC "Engine")
endif()
