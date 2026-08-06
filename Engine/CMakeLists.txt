# Copyright mzoesch. All rights reserved.

include_guard(_guard)

function(jafg_get_manifest_hash OUT_HASH FILE_PATH)
    set(${OUT_HASH} "NONE" PARENT_SCOPE)

    foreach(LINE IN LISTS JAFG_REMOTE_CONTENT_FILES)
        # Split line into hash and path
        string(REGEX MATCH "^([^ ]+)[ ]+(.+)$" MATCHED "${LINE}")

        if(MATCHED)
            set(HASH "${CMAKE_MATCH_1}")
            set(PATH "${CMAKE_MATCH_2}")

            if(PATH STREQUAL "${FILE_PATH}")
                set(${OUT_HASH} "${HASH}" PARENT_SCOPE)
                return()
            endif()
        endif()
    endforeach()
endfunction()

if(_guard)
    jafg_add_module(JAFG_MODULE_TYPE_SHARED)

    ###############################################################################
    # Engine only compile time definitions
    if(NOT DEFINED _detail_jafg_version)
        message(FATAL_ERROR "_detail_jafg_version is not defined.")
    endif()

    execute_process(
        COMMAND             git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY   ${JAFG_ENGINE_ROOT}
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
            WORKING_DIRECTORY   ${JAFG_ENGINE_ROOT}
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
                WORKING_DIRECTORY   ${JAFG_ENGINE_ROOT}
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

    file(RELATIVE_PATH DETAIL_ENGINE_ROOT_BINARY_PATH "${JAFG_ENGINE_ROOT}" "${CMAKE_BINARY_DIR}")
    if(DETAIL_ENGINE_ROOT_BINARY_PATH MATCHES "(^|[\\/])\.\.([\\/]|$)")
        message(FATAL_ERROR "[${DETAIL_ENGINE_ROOT_BINARY_PATH}]: DETAIL_ENGINE_ROOT_BINARY_PATH is not a valid relative path.")
    endif()

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

    message(STATUS "DETAIL_ENGINE_ROOT_BINARY_PATH: ${DETAIL_ENGINE_ROOT_BINARY_PATH}")
    message(STATUS "DETAIL_ENGINE_TARGET_PLATFORM: ${JAFG_TARGET_PLATFORM}")
    message(STATUS "DETAIL_ENGINE_TARGET_ARCHITECTURE: ${JAFG_TARGET_ARCHITECTURE}")
    message(STATUS "DETAIL_ENGINE_TARGET_TYPE: ${JAFG_TARGET_TYPE}")
    message(STATUS "DETAIL_ENGINE_TARGET_CONFIGURATION: ${JAFG_TARGET_CONFIG}")

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

        DETAIL_ENGINE_ROOT_BINARY_PATH="${DETAIL_ENGINE_ROOT_BINARY_PATH}"
        DETAIL_ENGINE_TARGET_PLATFORM="${JAFG_TARGET_PLATFORM}"
        DETAIL_ENGINE_TARGET_ARCHITECTURE="${JAFG_TARGET_ARCHITECTURE}"
        DETAIL_ENGINE_TARGET_TYPE="${JAFG_TARGET_TYPE}"
        DETAIL_ENGINE_TARGET_CONFIGURATION="${JAFG_TARGET_CONFIG}"
        )

    ###############################################################################
    # FastNoise2
    message(STATUS "Fetching FastNoise2...")
    FetchContent_Declare(
        FetchContent_FastNoise2
        GIT_REPOSITORY      https://github.com/Auburn/FastNoise2.git
        GIT_TAG             v0.10.0-alpha
        GIT_SHALLOW         TRUE
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
        GIT_SHALLOW         TRUE
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/freetype"
        )
    FetchContent_MakeAvailable(FetchContent_Freetype)
    target_link_libraries(${_scoped_module_name} PRIVATE freetype-interface)
    if(NOT TARGET Freetype::Freetype)
        add_library(Freetype::Freetype ALIAS freetype-interface)
    endif()

    ###############################################################################
    # harfbuzz
    message(STATUS "Fetching harfbuzz...")
    FetchContent_Declare(
        FetchContent_Harfbuzz
        GIT_REPOSITORY      https://github.com/harfbuzz/harfbuzz.git
        GIT_TAG             14.2.1
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
    set(MSDFGEN_DISABLE_PNG ON)
    message(STATUS "Fetching msdfgen...")
    FetchContent_Declare(
        FetchContent_MsdfGen
        GIT_REPOSITORY      https://github.com/Chlumsky/msdfgen.git
        GIT_TAG             v1.13
        GIT_SHALLOW         TRUE
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
        GIT_SHALLOW         TRUE
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
        GIT_SHALLOW         TRUE
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
        GIT_SHALLOW         TRUE
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
        GIT_SHALLOW         TRUE
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
        GIT_SHALLOW         TRUE
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
        GIT_SHALLOW         TRUE
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
        GIT_SHALLOW         TRUE
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
        GIT_SHALLOW         TRUE
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/httplib"
        )
    FetchContent_MakeAvailable(FetchContent_httplib)
    target_include_directories(${_scoped_module_name} PRIVATE ${fetchcontent_httplib_SOURCE_DIR})

    ###############################################################################
    # nativefiledialog-extended
    message(STATUS "Fetching nativefiledialog-extended...")
    FetchContent_Declare(
        FetchContent_nativefiledialog_extended
        GIT_REPOSITORY      https://github.com/btzy/nativefiledialog-extended.git
        GIT_TAG             v1.3.0
        GIT_SHALLOW         TRUE
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/nativefiledialog-extended"
        )
    FetchContent_MakeAvailable(FetchContent_nativefiledialog_extended)
#    target_include_directories(${_scoped_module_name} PRIVATE ${fetchcontent_nativefiledialog_extended_SOURCE_DIR}/include)
    target_link_libraries(${_scoped_module_name} PRIVATE nfd)

    ###############################################################################
    # tinyobjloader
    message(STATUS "Fetching tinyobjloader...")
    FetchContent_Declare(
        FetchContent_tinyobjloader
        GIT_REPOSITORY      https://github.com/tinyobjloader/tinyobjloader
        GIT_TAG             release
        GIT_SHALLOW         TRUE
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
        GIT_SHALLOW         TRUE
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/tinygltf"
        )
    FetchContent_MakeAvailable(FetchContent_tinygltf)
    target_include_directories(${_scoped_module_name} PRIVATE ${fetchcontent_tinygltf_SOURCE_DIR})

    ###############################################################################
    # slang
    message(STATUS "Fetching slang...")
    set(SLANG_VERSION "2026.11")
    function(_DownalodSlang _fldr _archive _url)
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
            message(STATUS "Slang archive already exists at [${_archive}].")
        endif()
        set(_dst_slang_extraction_dir "${JAFG_ENGINE_ROOT}/Vendor/${_fldr}")
        if(NOT EXISTS "${_dst_slang_extraction_dir}/LICENSE")
            file(MAKE_DIRECTORY "${_dst_slang_extraction_dir}")
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xzf "${_archive}"
                WORKING_DIRECTORY "${_dst_slang_extraction_dir}"
                )
        else()
            message(STATUS "Slang already extracted for OS [${_fldr}] at [${_dst_slang_extraction_dir}].")
        endif()
    endfunction()
    _DownalodSlang(
        "slang-${SLANG_VERSION}-win-x86_64"
        "${JAFG_ENGINE_ROOT}/Vendor/slang-${SLANG_VERSION}-win-x86_64.zip"
        "https://github.com/shader-slang/slang/releases/download/v${SLANG_VERSION}/slang-${SLANG_VERSION}-windows-x86_64.zip"
        )
    _DownalodSlang(
        "slang-${SLANG_VERSION}-lnx-x86_64"
        "${JAFG_ENGINE_ROOT}/Vendor/slang-${SLANG_VERSION}-lnx-x86_64.zip"
        "https://github.com/shader-slang/slang/releases/download/v${SLANG_VERSION}/slang-${SLANG_VERSION}-linux-x86_64.zip"
        )

    ###############################################################################
    # Content
    message(STATUS "Including jafg content...")
    set(JAFG_CONTENT_FILES "${JAFG_ENGINE_ROOT}/Content/lfs")
    if(NOT DEFINED JAFG_CONTENT_VERSION)
        set(JAFG_CONTENT_VERSION "v${_detail_jafg_version}")
    endif()
    message(STATUS "JAFG_CONTENT_VERSION: ${JAFG_CONTENT_VERSION}")
    set(JAFG_REMOTE_CONTENT_SERVER "https://api.zoeschinger.com/get-jafg-content/${JAFG_CONTENT_VERSION}/" CACHE STRING "Remote content server URL for Jafg content files.")
    file(STRINGS "${JAFG_ENGINE_ROOT}/Content/lfs" JAFG_CONTENT_FILES)

    if(DEFINED ENV{JAFG_UPGRADE_CONTENT})
        set(JAFG_MANIFEST_URL "${JAFG_REMOTE_CONTENT_SERVER}manifest")
        set(JAFG_REMOTE_MANIFEST "${CMAKE_CURRENT_BINARY_DIR}/Content/manifest")
        message(STATUS "Downloading content manifest from [${JAFG_MANIFEST_URL}] to [${JAFG_REMOTE_MANIFEST}] ...")
        file(DOWNLOAD
            "${JAFG_MANIFEST_URL}"
            "${JAFG_REMOTE_MANIFEST}"
            SHOW_PROGRESS
            STATUS download_status
            )
        list(GET download_status 0 download_result)
        if(NOT download_result EQUAL 0)
            message(FATAL_ERROR "Failed to download content manifest from [${JAFG_MANIFEST_URL}].")
        endif()
        file(STRINGS "${JAFG_REMOTE_MANIFEST}" JAFG_REMOTE_CONTENT_FILES)
        foreach(line IN LISTS JAFG_REMOTE_CONTENT_FILES)
            message(VERBOSE "REMOTE: ${line}")
        endforeach()
    endif()

    foreach(line IN LISTS JAFG_CONTENT_FILES)
        set(_content_file "${JAFG_ENGINE_ROOT}/Content/${line}")
        set(_content_url "${JAFG_REMOTE_CONTENT_SERVER}${line}")
        if((NOT EXISTS "${_content_file}") OR DEFINED ENV{JAFG_FORCE_CONTENT_UPDATE})
            message(STATUS "Downloading [${_content_url}] to [${_content_file}] ...")
            file(DOWNLOAD
                "${_content_url}"
                "${_content_file}"
                SHOW_PROGRESS
                STATUS download_status
                )
            list(GET download_status 0 download_result)
            if(NOT download_result EQUAL 0)
                message(FATAL_ERROR "[${_content_file}]: Failed to download content file from [${_content_url}].")
            endif()
        elseif(DEFINED ENV{JAFG_UPGRADE_CONTENT})
            if(NOT DEFINED JAFG_REMOTE_CONTENT_FILES)
                message(FATAL_ERROR "JAFG_REMOTE_CONTENT_FILES is not defined.")
            endif()
            file(SHA256 "${_content_file}" _hash)
            file(RELATIVE_PATH _relative_content_file "${JAFG_ENGINE_ROOT}/Content" "${_content_file}")
            jafg_get_manifest_hash(_remote_hash "${_relative_content_file}")
            if(_remote_hash STREQUAL "NONE")
                message(STATUS "[${_relative_content_file}]: Not in the remote manifest. Deleting.")
                file(REMOVE "${_content_file}")
            elseif(_hash STREQUAL _remote_hash)
                message(STATUS "[${_relative_content_file}]: Up to date.")
            else()
                message(STATUS "[${_relative_content_file}]: Outdated. Local(${_hash}) != Remote(${_remote_hash}). Downloading...")
                file(DOWNLOAD
                    "${_content_url}"
                    "${_content_file}"
                    SHOW_PROGRESS
                    STATUS download_status
                    )
                list(GET download_status 0 download_result)
                if(NOT download_result EQUAL 0)
                    message(FATAL_ERROR "[${_relative_content_file}]: Failed to download content file from [${_content_url}].")
                endif()
            endif()
        else()
            message(STATUS "Skipping download of [${_content_file}].")
        endif()
    endforeach()

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
    if(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WINDOWS)
        message(STATUS "Including Windows libs...")
        target_link_libraries(${_scoped_module_name} PRIVATE
            ws2_32
            setupapi
            )
    endif()
endif()
