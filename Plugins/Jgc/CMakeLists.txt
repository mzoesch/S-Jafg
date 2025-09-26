# Copyright mzoesch. All rights reserved.

include_guard(_guard)
if(_guard)
    set(this_plugin_identifier "JafgGameplayCore")
    set(this_plugin_friendly_name "Jafg Gameplay Core")

    jafg_add_module(JAFG_MODULE_TYPE_PLUGIN)
    jafg_add_dependency("Engine/Engine" "public")

    add_custom_target(JgcRuntime DEPENDS Runtime Jgc)
endif()
