# Copyright mzoesch. All rights reserved.

include_guard(_guard)
if(_guard)
    jafg_add_module(JAFG_MODULE_TYPE_SHARED)

    set_target_properties(${module_name} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${JAFG_ENGINE_ROOT}/bin/${JAFG_COMPOUND_CONFIG_PATH}"
            LIBRARY_OUTPUT_DIRECTORY "${JAFG_ENGINE_ROOT}/bin/${JAFG_COMPOUND_CONFIG_PATH}"
            RUNTIME_OUTPUT_DIRECTORY "${JAFG_ENGINE_ROOT}/bin/${JAFG_COMPOUND_CONFIG_PATH}"
            )

    ###############################################################################
    # Engine only compile time definitions
    if(NOT DEFINED _detail_jafg_version)
        message(FATAL_ERROR "_detail_jafg_version is not defined.")
    endif()

    execute_process(
        COMMAND             git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY   ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE     VCS_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    if(NOT VCS_BRANCH)
        set(VCS_BRANCH "UnknownVcsBranch")
        set(VCS_REMOTE "UnknownVcsRemote")
        set(VCS_URL "UnknownVcsUrl")
    else()
        execute_process(
            COMMAND             git config --get branch.${VCS_BRANCH}.remote
            WORKING_DIRECTORY   ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE     VCS_REMOTE
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            )
        if(NOT VCS_REMOTE)
            set(VCS_REMOTE "UnknownVcsRemote")
            set(VCS_URL "UnknownVcsUrl")
        else()
            execute_process(
                COMMAND             git remote get-url ${VCS_REMOTE}
                WORKING_DIRECTORY   ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE     VCS_URL
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET
                )
            if(NOT VCS_URL)
                set(VCS_URL "UnknownVcsUrl")
            endif()
        endif()
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

    execute_process(
        COMMAND             git log -1 --format=%H -- ${JAFG_ENGINE_ROOT}/Config/.ver
        WORKING_DIRECTORY   ${JAFG_ENGINE_ROOT}
        OUTPUT_VARIABLE     PROJECT_VERSION_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    if(PROJECT_VERSION_HASH)
        execute_process(
            COMMAND             git rev-list HEAD --count ${PROJECT_VERSION_HASH}..HEAD
            WORKING_DIRECTORY   ${JAFG_ENGINE_ROOT}
            OUTPUT_VARIABLE     PROJECT_VERSION_REVISION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
    else()
        set(PROJECT_VERSION_HASH "0")
    endif()
    if(NOT PROJECT_VERSION_REVISION)
        set(PROJECT_VERSION_REVISION "0")
    endif()

    set(JAFG_EXPECTED_RUNTIME "Runtime${CMAKE_EXECUTABLE_SUFFIX}")

    message(STATUS "DETAIL_ENGINE_VERSION: ${_detail_jafg_version}")
    message(STATUS "DETAIL_ENGINE_VERSION_MAJOR: ${PROJECT_VERSION_MAJOR}")
    message(STATUS "DETAIL_ENGINE_VERSION_MINOR: ${PROJECT_VERSION_MINOR}")
    message(STATUS "DETAIL_ENGINE_VERSION_PATCH: ${PROJECT_VERSION_PATCH}")
    message(STATUS "DETAIL_ENGINE_VERSION_REVISION: ${PROJECT_VERSION_REVISION}")
    message(STATUS "DETAIL_ENGINE_VERSION_HASH: ${PROJECT_VERSION_HASH}")
    message(STATUS "DETAIL_ENGINE_VCS_BRANCH: ${VCS_BRANCH}")
    message(STATUS "DETAIL_ENGINE_VCS_REVISION: ${VCS_REVISION}")

    message(STATUS "DETAIL_ENGINE_VCS_REMOTE: ${VCS_REMOTE}")
    message(STATUS "DETAIL_ENGINE_VCS_UPSTREAM: ${VCS_URL}")
    message(STATUS "DETAIL_ENGINE_HOMEPAGE_URL: ${PROJECT_HOMEPAGE_URL}")

    message(STATUS "DETAIL_ENGINE_TARGET_PLATFORM: ${JAFG_TARGET_PLATFORM}")
    message(STATUS "DETAIL_ENGINE_TARGET_ARCHITECTURE: ${JAFG_TARGET_ARCHITECTURE}")
    message(STATUS "DETAIL_ENGINE_TARGET_TYPE: ${JAFG_TARGET_TYPE}")
    message(STATUS "DETAIL_ENGINE_TARGET_CONFIGURATION: ${JAFG_TARGET_CONFIG}")
    message(STATUS "DETAIL_ENGINE_TARGET_COMPOUND: ${JAFG_TARGET_COMPOUND}")
    message(STATUS "DETAIL_ENGINE_PLATFORM_COMPOUND: ${JAFG_PLATFORM_COMPOUND}")
    message(STATUS "DETAIL_ENGINE_CONFIG_COMPOUND: ${JAFG_COMPOUND_CONFIG_PATH}")
    message(STATUS "DETAIL_ENGINE_EXPECTED_RUNTIME: ${JAFG_EXPECTED_RUNTIME}")

    target_compile_definitions(${_scoped_module_name} PRIVATE
        DETAIL_ENGINE_VERSION="${_detail_jafg_version}"
        DETAIL_ENGINE_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        DETAIL_ENGINE_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        DETAIL_ENGINE_VERSION_PATCH=${PROJECT_VERSION_PATCH}
        DETAIL_ENGINE_VERSION_REVISION=${PROJECT_VERSION_REVISION}
        DETAIL_ENGINE_VERSION_HASH="${PROJECT_VERSION_HASH}"
        DETAIL_ENGINE_VCS_BRANCH="${VCS_BRANCH}"
        DETAIL_ENGINE_VCS_REVISION="${VCS_REVISION}"

        DETAIL_ENGINE_VCS_REMOTE="${VCS_REMOTE}"
        DETAIL_ENGINE_VCS_UPSTREAM="${VCS_URL}"
        DETAIL_ENGINE_HOMEPAGE_URL="${PROJECT_HOMEPAGE_URL}"

        DETAIL_ENGINE_TARGET_PLATFORM="${JAFG_TARGET_PLATFORM}"
        DETAIL_ENGINE_TARGET_ARCHITECTURE="${JAFG_TARGET_ARCHITECTURE}"
        DETAIL_ENGINE_TARGET_TYPE="${JAFG_TARGET_TYPE}"
        DETAIL_ENGINE_TARGET_CONFIGURATION="${JAFG_TARGET_CONFIG}"
        DETAIL_ENGINE_TARGET_COMPOUND="${JAFG_TARGET_COMPOUND}"
        DETAIL_ENGINE_PLATFORM_COMPOUND="${JAFG_PLATFORM_COMPOUND}"
        DETAIL_ENGINE_CONFIG_COMPOUND="${JAFG_COMPOUND_CONFIG_PATH}"
        DETAIL_ENGINE_EXPECTED_RUNTIME="${JAFG_EXPECTED_RUNTIME}"
        )

    ###############################################################################
    # FastNoise2
    message(STATUS "Fetching FastNoise2...")
    FetchContent_Declare(
        FetchContent_FastNoise2
        GIT_REPOSITORY      https://github.com/Auburn/FastNoise2.git
        GIT_TAG             v0.10.0-alpha
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/fastnoise2"
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
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/freetype"
        )
    FetchContent_MakeAvailable(FetchContent_Freetype)
    target_link_libraries(${_scoped_module_name} PRIVATE freetype-interface)

    ###############################################################################
    # harfbuzz
    message(STATUS "Fetching harfbuzz...")
    FetchContent_Declare(
        FetchContent_Harfbuzz
        GIT_REPOSITORY      https://github.com/harfbuzz/harfbuzz.git
        GIT_TAG             13.0.1
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/harfbuzz"
        )
    FetchContent_MakeAvailable(FetchContent_Harfbuzz)
    target_link_libraries(${_scoped_module_name} PRIVATE harfbuzz)

    ###############################################################################
    # msdfgen
    set(MSDFGEN_CORE_ONLY OFF)
    set(MSDFGEN_BUILD_STANDALONE OFF)
    set(MSDFGEN_USE_VCPKG OFF)
    set(MSDFGEN_USE_SKIA OFF)
    set(MSDFGEN_DISABLE_SVG ON)
    message(STATUS "Fetching msdfgen...")
    FetchContent_Declare(
        FetchContent_MsdfGen
        GIT_REPOSITORY      https://github.com/Chlumsky/msdfgen.git
        GIT_TAG             v1.13
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/msdfgen"
        )
    FetchContent_MakeAvailable(FetchContent_MsdfGen)
    target_link_libraries(${_scoped_module_name} PRIVATE msdfgen-core)
    target_link_libraries(${_scoped_module_name} PRIVATE msdfgen-ext)

    ###############################################################################
    # msdf-adtlas-gen
    set(MSDF_ATLAS_BUILD_STANDALONE OFF)
    set(MSDF_ATLAS_USE_VCPKG OFF)
    set(MSDF_ATLAS_USE_SKIA OFF)
    set(MSDF_ATLAS_MSDFGEN_EXTERNAL ON)
    message(STATUS "Fetching msdf-adtlas-gen...")
    FetchContent_Declare(
        FetchContent_MsdfAtlasGen
        GIT_REPOSITORY      https://github.com/Chlumsky/msdf-atlas-gen.git
        GIT_TAG             v1.4
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/msdf-atlas-gen"
        )
    FetchContent_MakeAvailable(FetchContent_MsdfAtlasGen)
    target_link_libraries(${_scoped_module_name} PRIVATE msdf-atlas-gen)

    ###############################################################################
    # Glfw3
    message(STATUS "Fetching Glfw3...")
    FetchContent_Declare(
        FetchContent_Glfw3
        GIT_REPOSITORY      https://github.com/glfw/glfw.git
        GIT_TAG             master
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/glfw3"
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
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/sqlite3"
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
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/vma"
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
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/glm"
        )
    FetchContent_MakeAvailable(FetchContent_Glm)
    target_include_directories(${_scoped_module_name} PUBLIC ${fetchcontent_glm_SOURCE_DIR})

    ###############################################################################
    # Nlohmann JSON
    message(STATUS "Fetching Nlohmann JSON...")
    FetchContent_Declare(
        FetchContent_NlohmannJson
        GIT_REPOSITORY      https://github.com/nlohmann/json
        GIT_TAG             master
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/nlohmannjson"
        )
    FetchContent_MakeAvailable(FetchContent_NlohmannJson)
    target_include_directories(${_scoped_module_name} PUBLIC ${fetchcontent_nlohmannjson_SOURCE_DIR}/include)

    ###############################################################################
    # Stb
    message(STATUS "Fetching Stb...")
    FetchContent_Declare(
        FetchContent_Stb
        GIT_REPOSITORY      https://github.com/nothings/stb
        GIT_TAG             master
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/stb"
        )
    FetchContent_MakeAvailable(FetchContent_Stb)
    target_include_directories(${_scoped_module_name} PUBLIC ${fetchcontent_stb_SOURCE_DIR})

    ###############################################################################
    # http
    message(STATUS "Fetching cpp-httplib...")
    FetchContent_Declare(
        FetchContent_httplib
        GIT_REPOSITORY      https://github.com/yhirose/cpp-httplib
        GIT_TAG             v0.28.0
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/httplib"
        )
    FetchContent_MakeAvailable(FetchContent_httplib)
    target_include_directories(${_scoped_module_name} PRIVATE ${fetchcontent_httplib_SOURCE_DIR})

    ###############################################################################
    # tinyobjloader
    message(STATUS "Fetching tinyobjloader...")
    FetchContent_Declare(
        FetchContent_tinyobjloader
        GIT_REPOSITORY      https://github.com/tinyobjloader/tinyobjloader
        GIT_TAG             release
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/tinyobjloader"
        )
    FetchContent_MakeAvailable(FetchContent_tinyobjloader)
    target_include_directories(${_scoped_module_name} PRIVATE ${fetchcontent_tinyobjloader_SOURCE_DIR})

    ###############################################################################
    # tinygltf
    message(STATUS "Fetching tinygltf...")
    FetchContent_Declare(
        FetchContent_tinygltf
        GIT_REPOSITORY      https://github.com/syoyo/tinygltf.git
        GIT_TAG             release
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/tinygltf"
        )
    FetchContent_MakeAvailable(FetchContent_tinygltf)
    target_include_directories(${_scoped_module_name} PRIVATE ${fetchcontent_tinygltf_SOURCE_DIR})

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
        else()
            message(STATUS "Slangc archive already exists at [${_archive}].")
        endif()
        set(_dst_slang_extraction_dir "${JAFG_ENGINE_ROOT}/Vendor/Slang-${_os}")
        if(NOT EXISTS "${_dst_slang_extraction_dir}/LICENSE")
            file(MAKE_DIRECTORY "${_dst_slang_extraction_dir}")
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xzf "${_archive}"
                WORKING_DIRECTORY "${_dst_slang_extraction_dir}"
                )
        else()
            message(STATUS "Slangc already extracted for OS [${_os}] at [${_dst_slang_extraction_dir}].")
        endif()
    endfunction()
    _DownalodSlangc("win-x86_64" "${JAFG_ENGINE_ROOT}/Vendor/win-x86_64-slangc.zip" "https://github.com/shader-slang/slang/releases/download/v${SLANGC_VERSION}/slang-${SLANGC_VERSION}-windows-x86_64.zip")
    _DownalodSlangc("lnx-x86_64" "${JAFG_ENGINE_ROOT}/Vendor/lnx-x86_64-slangc.zip" "https://github.com/shader-slang/slang/releases/download/v${SLANGC_VERSION}/slang-${SLANGC_VERSION}-linux-x86_64.zip")

    ###############################################################################
    # Fonts
    message(STATUS "Including fonts...")
    set(CASCADIA_MONO_URL "https://github.com/microsoft/cascadia-code/releases/download/v2407.24/CascadiaCode-2407.24.zip")
    set(CASCADIA_MONO_ZIP "${JAFG_ENGINE_ROOT}/Vendor/CascadiaCode.zip")
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
