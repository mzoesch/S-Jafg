# Copyright mzoesch. All rights reserved.

include_guard(_guard)
if(_guard)
    jafg_add_module(JAFG_MODULE_TYPE_SHARED)
    jafg_add_dependency("Lal/Lal" "public")
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
        GIT_TAG             v0.10.0-alpha
        )
    FetchContent_MakeAvailable(FetchContent_FastNoise2)
    DisableAllWarningsForTarget(FastNoise) # FastNoise2 is an alias of FastNoise
    target_link_libraries(${_scoped_module_name} PUBLIC FastNoise2)

    ###############################################################################
    # Freetype
    message(STATUS "Fetching Freetype...")
    FetchContent_Declare(
        FetchContent_Freetype
        GIT_REPOSITORY      https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG             VER-2-13-3
        )
    FetchContent_MakeAvailable(FetchContent_Freetype)
    target_link_libraries(${_scoped_module_name} PUBLIC freetype-interface)

    ###############################################################################
    # Glfw3
    message(STATUS "Fetching Glfw3...")
    FetchContent_Declare(
        FetchContent_Glfw3
        GIT_REPOSITORY      https://github.com/glfw/glfw.git
        GIT_TAG             master
        )
    FetchContent_MakeAvailable(FetchContent_Glfw3)
    target_link_libraries(${_scoped_module_name} PUBLIC glfw ${GLFW_LIBRARIES})

    ###############################################################################
    # Vulkan
    find_package(Vulkan REQUIRED)
    target_link_libraries(${_scoped_module_name} PUBLIC Vulkan::Vulkan)

    ###############################################################################
    # Sqlite3
    message(STATUS "Fetching Sqlite3...")
    FetchContent_Declare(
        FetchContent_Sqlite3
        GIT_REPOSITORY      https://github.com/mzoesch/S-Jafg-Sqlite3-Amalgamation.git
        GIT_TAG             trunk
        )
    FetchContent_MakeAvailable(FetchContent_Sqlite3)
    target_link_libraries(${_scoped_module_name} PUBLIC jafg_sqlite3_amalgamation)

    ###############################################################################
    # Vma
    message(STATUS "Fetching Vma...")
    FetchContent_Declare(
        FetchContent_Vma
        GIT_REPOSITORY      https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG             master
        )
    FetchContent_MakeAvailable(FetchContent_Vma)
    target_link_libraries(${_scoped_module_name} PUBLIC GPUOpen::VulkanMemoryAllocator)

    ###############################################################################
    # Glm
    message(STATUS "Fetching GLM...")
    FetchContent_Declare(
        FetchContent_Glm
        GIT_REPOSITORY      https://github.com/g-truc/glm
        GIT_TAG             master
        )
    FetchContent_MakeAvailable(FetchContent_Glm)
    target_include_directories(${_scoped_module_name} PUBLIC ${fetchcontent_glm_SOURCE_DIR})

    ###############################################################################
    # Nlohmann JSON
    message(STATUS "Fetching Nlohmann JSON...")
    FetchContent_Declare(
        FetchContent_NlohmannJson
        GIT_REPOSITORY          https://github.com/nlohmann/json
        GIT_TAG                 master
        )
    FetchContent_MakeAvailable(FetchContent_NlohmannJson)
    target_include_directories(${_scoped_module_name} PUBLIC ${fetchcontent_nlohmannjson_SOURCE_DIR}/include)

    ###############################################################################
    # Stb
    message(STATUS "Fetching Stb...")
    FetchContent_Declare(
        FetchContent_Stb
        GIT_REPOSITORY          https://github.com/nothings/stb
        GIT_TAG                 master
        )
    FetchContent_MakeAvailable(FetchContent_Stb)
    target_include_directories(${_scoped_module_name} PUBLIC ${fetchcontent_stb_SOURCE_DIR})

    ###############################################################################
    # http
    message(STATUS "Fetching cpp-httplib...")
    FetchContent_Declare(
        FetchContent_httplib
        GIT_REPOSITORY          https://github.com/yhirose/cpp-httplib
        GIT_TAG                 v0.28.0
        )
    FetchContent_MakeAvailable(FetchContent_httplib)
    target_include_directories(${_scoped_module_name} PRIVATE ${fetchcontent_httplib_SOURCE_DIR})

    ###############################################################################
    # tinyobjloader
    message(STATUS "Fetching tinyobjloader...")
    FetchContent_Declare(
        FetchContent_tinyobjloader
        GIT_REPOSITORY          https://github.com/tinyobjloader/tinyobjloader
        GIT_TAG                 release
        )
    FetchContent_MakeAvailable(FetchContent_tinyobjloader)
    target_include_directories(${_scoped_module_name} PUBLIC ${fetchcontent_tinyobjloader_SOURCE_DIR})

    ###############################################################################
    # slangc
    message(STATUS "Fetching slang...")
    set(SLANGC_VERSION "2025.24.2")
    function(_DownalodSlangc _os _archive _url)
        if(NOT EXISTS "${_archive}")
            message(STATUS "Downloading Slang from [${_url}] to [${_archive}]...")
            file(DOWNLOAD
                "${_url}"
                "${_archive}"
                SHOW_PROGRESS
                STATUS DOWNLOAD_STATUS
                )
            list(GET DOWNLOAD_STATUS 0 DOWNLOAD_RESULT)
            if(NOT DOWNLOAD_RESULT EQUAL 0)
                message(FATAL_ERROR "Failed to download slang from [${_url}].")
            endif()
            file(MAKE_DIRECTORY "${JAFG_ENGINE_ROOT}/Content/.Slang${_os}")
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xzf "${_archive}"
                WORKING_DIRECTORY "${JAFG_ENGINE_ROOT}/Content/.Slang${_os}"
                )
        else()
            message(STATUS "Slangc archive already exists at [${_archive}].")
        endif()
    endfunction()
    _DownalodSlangc("Windows_x86_64" "${CMAKE_BINARY_DIR}/windows-x86_64-slangc.zip" "https://github.com/shader-slang/slang/releases/download/v${SLANGC_VERSION}/slang-${SLANGC_VERSION}-windows-x86_64.zip")
    _DownalodSlangc("Linux_x86_64" "${CMAKE_BINARY_DIR}/linux-x86_64-slangc.zip" "https://github.com/shader-slang/slang/releases/download/v${SLANGC_VERSION}/slang-${SLANGC_VERSION}-linux-x86_64.zip")

    ###############################################################################
    # Tracked vendors
    message(STATUS "Including tracked vendors...")
    target_include_directories(${_scoped_module_name} PRIVATE
        "${JAFG_ENGINE_ROOT}/Engine/Vendor/Include"
        )

    ###############################################################################
    # Fonts
    message(STATUS "Including fonts...")
    set(CASCADIA_MONO_URL "https://github.com/microsoft/cascadia-code/releases/download/v2407.24/CascadiaCode-2407.24.zip")
    set(CASCADIA_MONO_ZIP "${CMAKE_BINARY_DIR}/CascadiaCode.zip")
    set(CASCADIA_MONO_DIR "${JAFG_ENGINE_ROOT}/Content/Fonts/CascadiaMono")
    if(NOT EXISTS "${CASCADIA_MONO_ZIP}")
        message(STATUS "Downloading Cascadia Mono font...")
        file(DOWNLOAD
            "${CASCADIA_MONO_URL}"
            "${CASCADIA_MONO_ZIP}"
            SHOW_PROGRESS
            STATUS download_status
            )
        list(GET download_status 0 download_result)
        if(NOT download_result EQUAL 0)
            message(FATAL_ERROR "Failed to download Cascadia Mono font from [${CASCADIA_MONO_URL}].")
        endif()
    endif()
    if(NOT EXISTS "${CASCADIA_MONO_DIR}")
        message(STATUS "Extracting Cascadia Mono font...")
        file(ARCHIVE_EXTRACT
            INPUT            "${CASCADIA_MONO_ZIP}"
            DESTINATION      "${CASCADIA_MONO_DIR}"
            )
    endif()

    ###############################################################################
    # Libs
    if(JAFG_TARGET_PLATFORM STREQUAL "Windows")
        message(STATUS "Including Windows libs...")
        target_link_libraries(${_scoped_module_name} PRIVATE
            ws2_32
            setupapi
            )
    endif()
endif()
