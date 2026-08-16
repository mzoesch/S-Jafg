# Copyright mzoesch. All rights reserved.

include_guard(_guard)
if(_guard)
    set(this_plugin_identifier "JafgGameplayCore")
    set(this_plugin_friendly_name "Jafg Gameplay Core")
    set(this_plugin_author "mzoesch")
    set(this_plugin_supports_dyn_unload false)

    jafg_add_module(JAFG_MODULE_TYPE_PLUGIN)
    jafg_add_dependency("Engine" "public")

    if(NOT DEFINED JGC_CONTENT_VERSION)
        set(JGC_CONTENT_VERSION "v${_detail_jafg_version}")
    endif()
    set(JGC_CONTENT_VERSION "${JGC_CONTENT_VERSION}")
    set(JGC_CONTENT_DIR "${CMAKE_CURRENT_LIST_DIR}/Content" CACHE STRING "Local content directory for Jgc.")
    set(JGC_REMOTE_CONTENT_SERVER "https://api.zoeschinger.com/get-jgc-content/${JGC_CONTENT_VERSION}" CACHE STRING "Remote content server URL for Jgc content files.")
    message(STATUS "JGC_CONTENT_VERSION: ${JGC_CONTENT_VERSION}")
    message(STATUS "JGC_CONTENT_DIR: ${JGC_CONTENT_DIR}")
    message(STATUS "JGC_REMOTE_CONTENT_SERVER: ${JGC_REMOTE_CONTENT_SERVER}")
    if(NOT JGC_CONTENT_VERSION)
        message(FATAL_ERROR "JGC_CONTENT_VERSION is not defined. Please set it to the desired version of the content.")
    endif()
    HandleContentDependencies(
        "${JGC_CONTENT_DIR}"
        "${JGC_REMOTE_CONTENT_SERVER}"
        )

    # Convenience target to build the runtime and the plugin together.
    add_custom_target(JgcRuntime DEPENDS Runtime Jgc)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "JgcRuntime")
endif()
