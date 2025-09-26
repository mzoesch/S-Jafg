# Copyright mzoesch. All rights reserved.

include_guard(_guard)
if(_guard)
    jafg_add_module(JAFG_MODULE_TYPE_SHARED)
    jafg_add_dependency("Lal/Lal" "public")
    jafg_add_dependency("Engine/Core" "private")
    jafg_add_dependency("Engine/EngineForward" "private")

    ###############################################################################
    # Engine only compile time definitions
    if(NOT DEFINED PROJECT_VERSION)
        message(FATAL_ERROR "PROJECT_VERSION is not defined.")
    endif()

    execute_process(
        COMMAND             git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY   ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE     VCS_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    if(NOT VCS_BRANCH)
        set(VCS_BRANCH "UnknownVcsBranch")
    endif()

    execute_process(
            COMMAND             git rev-parse --short HEAD
            WORKING_DIRECTORY   ${JAFG_ENGINE_ROOT}
            OUTPUT_VARIABLE     VCS_REVISION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
    if(NOT VCS_REVISION)
        set(VCS_REVISION "UnknownVcsRevision")
    endif()

    target_compile_definitions(${_scoped_module_name} PRIVATE
        PRIVATE_ENGINE_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        PRIVATE_ENGINE_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        PRIVATE_ENGINE_VERSION_PATCH=${PROJECT_VERSION_PATCH}
        PRIVATE_ENGINE_VERSION="${PROJECT_VERSION}"
        PRIVATE_ENGINE_VCS_BRANCH="${VCS_BRANCH}"
        PRIVATE_ENGINE_VCS_REVISION="${VCS_REVISION}"

        PRIVATE_ENGINE_TARGET_PLATFORM="${JAFG_TARGET_PLATFORM}"
        PRIVATE_ENGINE_TARGET_ARCHITECTURE="${JAFG_TARGET_ARCHITECTURE}"
        PRIVATE_ENGINE_TARGET_TYPE="${JAFG_TARGET_TYPE}"
        PRIVATE_ENGINE_TARGET_CONFIGURATION="${JAFG_TARGET_CONFIG}"
        PRIVATE_ENGINE_TARGET_COMPOUND="${JAFG_TARGET_COMPOUND}"
        PRIVATE_ENGINE_PLATFORM_COMPOUND="${JAFG_PLATFORM_COMPOUND}"
        PRIVATE_ENGINE_CONFIG_COMPOUND="${JAFG_COMPOUND_CONFIG_PATH}"
        )

    message(STATUS "Using engine version: ${PROJECT_VERSION} (${VCS_BRANCH} - ${VCS_REVISION}).")

    ###############################################################################
    # FastNoise2
    message(STATUS "Fetching FastNoise2...")
    FetchContent_Declare(
        FetchContent_FastNoise2
        GIT_REPOSITORY      https://github.com/Auburn/FastNoise2.git
        GIT_TAG             master
        )
    FetchContent_MakeAvailable(FetchContent_FastNoise2)
    DisableAllWarningsForTarget(FastNoise) # FastNoise2 is an alias of FastNoise
    target_link_libraries(${_scoped_module_name} PRIVATE FastNoise2)

    ###############################################################################
    # Freetype
    message(STATUS "Fetching Freetype...")
    FetchContent_Declare(
        FetchContent_Freetype
        GIT_REPOSITORY      https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG             VER-2-13-3
        )
    FetchContent_MakeAvailable(FetchContent_Freetype)
    target_link_libraries(${_scoped_module_name} PRIVATE freetype-interface)

    ###############################################################################
    # Glfw3
    message(STATUS "Fetching Glfw3...")
    FetchContent_Declare(
        FetchContent_Glfw3
        GIT_REPOSITORY      https://github.com/glfw/glfw.git
        GIT_TAG             master
        )
    FetchContent_MakeAvailable(FetchContent_Glfw3)
    target_link_libraries(${_scoped_module_name} PRIVATE glfw)

    ###############################################################################
    # Sqlite3
    message(STATUS "Fetching Sqlite3...")
    FetchContent_Declare(
        FetchContent_Sqlite3
        GIT_REPOSITORY      https://github.com/mzoesch/S-Jafg-Sqlite3-Amalgamation.git
        GIT_TAG             trunk
        )
    FetchContent_MakeAvailable(FetchContent_Sqlite3)
    target_link_libraries(${_scoped_module_name} PRIVATE jafg_sqlite3_amalgamation)

    ###############################################################################
    # Glm
    message(STATUS "Including GLM...")
    set(glm_dir "${JAFG_ENGINE_ROOT}/Engine/Vendor/CommonMath")
    if(NOT EXISTS "${glm_dir}")
        message(FATAL_ERROR "Expected submodule at [${glm_dir}].")
    endif()
    target_include_directories(${_scoped_module_name} PRIVATE "${glm_dir}/glm")

    ###############################################################################
    # Nlohmann JSON
    message(STATUS "Including Nlohmann JSON...")
    set(nlohmann_json_dir "${JAFG_ENGINE_ROOT}/Engine/Vendor/Json")
    if(NOT EXISTS "${nlohmann_json_dir}")
        message(FATAL_ERROR "Expected submodule at [${nlohmann_json_dir}].")
    endif()
    target_include_directories(${_scoped_module_name} PRIVATE "${nlohmann_json_dir}/single_include")

    ###############################################################################
    # Stb
    message(STATUS "Including Stb...")
    set(stb_dir "${JAFG_ENGINE_ROOT}/Engine/Vendor/Stb")
    if(NOT EXISTS "${stb_dir}")
        message(FATAL_ERROR "Expected submodule at [${stb_dir}].")
    endif()
    target_include_directories(${_scoped_module_name} PRIVATE "${stb_dir}")

    ###############################################################################
    # Tracked vendors
    message(STATUS "Including tracked vendors...")
    target_include_directories(${_scoped_module_name} PRIVATE
        "${JAFG_ENGINE_ROOT}/Engine/Vendor/Include"
        )
endif()
