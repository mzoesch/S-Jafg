# Copyright mzoesch. All rights reserved.

include_guard(_guard)
if(_guard)
    jafg_add_module(JAFG_MODULE_TYPE_PLUGIN)
    jafg_add_dependency("Engine/Engine" "public")
endif()
