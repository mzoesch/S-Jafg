# Copyright mzoesch. All rights reserved.

include_guard(_guard)
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
        GIT_TAG             903c1f2d2f9d53ddce94cd223f32727d9ab3aeaa # v1.1.1
        GIT_PROGRESS        TRUE
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
        GIT_TAG             0a0221a1347e2f1e07c395263540026e9a0aa7c7 # VER-2-14-3
        GIT_PROGRESS        TRUE
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
        GIT_TAG             4c2aa804671d7276e8a0eb95da07202ead05c843 # 14.3.0
        GIT_PROGRESS        TRUE
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
        GIT_TAG             1874bcf7d9624ccc85b4bc9a85d78116f690f35b # v1.13
        GIT_PROGRESS        TRUE
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
        GIT_TAG             2ede254314a2512252a225fa6c975948d6af559a # v1.4
        GIT_PROGRESS        TRUE
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
        GIT_TAG             d9d6f0f1f967807ffade6598ea9a631ebaf37a56 # 3.5.1
        GIT_PROGRESS        TRUE
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
        GIT_REPOSITORY      https://github.com/mzoesch/S-Jafg-Sqlite3-Amalgamation
        GIT_TAG             0a254b4a7324babba447618edb67b10e6fd2be4f # Latest as of 2026-08-07
        GIT_PROGRESS        TRUE
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
        GIT_TAG             3aa921224c154a0d2c43912bc88e1c42ce1f7607 # v3.4.0
        GIT_PROGRESS        TRUE
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
        GIT_TAG             8d1fd52e5ab5590e2c81768ace50c72bae28f2ed # 1.0.3
        GIT_PROGRESS        TRUE
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
        GIT_TAG             55f93686c01528224f448c19128836e7df245f72 # v3.12.0
        GIT_PROGRESS        TRUE
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
        GIT_TAG             2c980bb59875b0d32144a71867fbdebb2f77cd20 # Latest as of 2026-08-07
        GIT_PROGRESS        TRUE
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
        GIT_TAG             095a5c1caf9e467ff840ee2ffd19be1a7852203b # v0.52.0
        GIT_PROGRESS        TRUE
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
        GIT_TAG             fc168e8605bfa51aaec22ab0c4e46b9de665a437 # v1.3.0
        GIT_PROGRESS        TRUE
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/nativefiledialog-extended"
        )
    FetchContent_MakeAvailable(FetchContent_nativefiledialog_extended)
    target_link_libraries(${_scoped_module_name} PRIVATE nfd)

    ###############################################################################
    # tinyobjloader
    message(STATUS "Fetching tinyobjloader...")
    FetchContent_Declare(
        FetchContent_tinyobjloader
        GIT_REPOSITORY      https://github.com/tinyobjloader/tinyobjloader
        GIT_TAG             2945a967c5303b2c8c14174117c45f3302591150 # v2.0.0rc13
        GIT_PROGRESS        TRUE
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/tinyobjloader"
        )
    FetchContent_MakeAvailable(FetchContent_tinyobjloader)
    target_include_directories(${_scoped_module_name} PRIVATE ${fetchcontent_tinyobjloader_SOURCE_DIR})

    ###############################################################################
    # tinygltf
    message(STATUS "Fetching tinygltf...")
    set(TINYGLTF3_BUILD_TESTS OFF)
    set(TINYGLTF3_INSTALL OFF)
    message(STATUS "TINYGLTF3_BUILD_TESTS: ${TINYGLTF3_BUILD_TESTS}")
    message(STATUS "TINYGLTF3_INSTALL: ${TINYGLTF3_INSTALL}")
    FetchContent_Declare(
        FetchContent_tinygltf
        GIT_REPOSITORY      https://github.com/syoyo/tinygltf.git
        GIT_TAG             0d9ce1a9561688c6c6e7471430d15a50a0b15f1a # v3.0.1
        GIT_PROGRESS        TRUE
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/tinygltf"
        )
    FetchContent_MakeAvailable(FetchContent_tinygltf)
    target_include_directories(${_scoped_module_name} PRIVATE ${fetchcontent_tinygltf_SOURCE_DIR})

    ###############################################################################
    # jolt
    message(STATUS "Fetching jolt...")
    jafg_hint_debug_symbols(JoltPhysics_GENERATE_DEBUG_SYMBOLS)
    message(STATUS "JoltPhysics_GENERATE_DEBUG_SYMBOLS: ${JoltPhysics_GENERATE_DEBUG_SYMBOLS}")
    option(JoltPhysics_USE_ASSERTIONS_IN_DISTRIBUTION "Enable assertions in distribution builds for the physics engine." OFF)
    message(STATUS "JoltPhysics_USE_ASSERTIONS_IN_DISTRIBUTION: ${JoltPhysics_USE_ASSERTIONS_IN_DISTRIBUTION}")
    if(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_SHIPPING)
        if(JoltPhysics_USE_ASSERTIONS_IN_DISTRIBUTION)
            set(JoltPhysics_USE_ASSERTS ON)
        else()
            set(JoltPhysics_USE_ASSERTS OFF)
        endif()
    else()
        set(JoltPhysics_USE_ASSERTS ON)
    endif()
    message(STATUS "JoltPhysics_USE_ASSERTS: ${JoltPhysics_USE_ASSERTS}")
    option(JoltPhysics_DOUBLE_PRECISION "Enable double precision for the physics engine." OFF)
    message(STATUS "JoltPhysics_DOUBLE_PRECISION: ${JoltPhysics_DOUBLE_PRECISION}")
    option(JoltPhysics_CROSS_PLATFORM_DETERMINISTIC "Enable cross-platform deterministic mode for the physics engine." OFF)
    message(STATUS "JoltPhysics_CROSS_PLATFORM_DETERMINISTIC: ${JoltPhysics_CROSS_PLATFORM_DETERMINISTIC}")
    if((JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEBUG) OR (JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEVELOPMENT))
        if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            # When turning this on, in Debug and Release mode, the library will emit extra code to ensure that the 4th component of a 3-vector is kept the same as the 3rd component
            # and will enable floating point exceptions during simulation to detect divisions by zero.
            # Note that this currently only works using MSVC. Clang turns Float2 into a SIMD vector sometimes causing floating point exceptions (the option is ignored).
            set(JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED ON)
        else()
            set(JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED OFF)
        endif()
    else()
        set(JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED OFF)
    endif()
    message(STATUS "JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED: ${JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED}")
    option(JoltPhysics_USE_32_OBJECT_LAYER_BITS "Use 32 object layer bits instead of 16 for the physics engine." OFF)
    message(STATUS "JoltPhysics_USE_32_OBJECT_LAYER_BITS: ${JoltPhysics_USE_32_OBJECT_LAYER_BITS}")
    if(JoltPhysics_USE_32_OBJECT_LAYER_BITS)
        set(JoltPhysics_OBJECT_LAYER_BITS 32)
    else()
        set(JoltPhysics_OBJECT_LAYER_BITS 16)
    endif()
    message(STATUS "JoltPhysics_OBJECT_LAYER_BITS: ${JoltPhysics_OBJECT_LAYER_BITS}")

    option(JoltPhysics_TRACK_BROADPHASE_STATS "Enable tracking of broadphase statistics for the physics engine." OFF)
    message(STATUS "JoltPhysics_TRACK_BROADPHASE_STATS: ${JoltPhysics_TRACK_BROADPHASE_STATS}")
    option(JoltPhysics_TRACK_NARROWPHASE_STATS "Enable tracking of narrowphase statistics for the physics engine." OFF)
    message(STATUS "JoltPhysics_TRACK_NARROWPHASE_STATS: ${JoltPhysics_TRACK_NARROWPHASE_STATS}")
    option(JoltPhysics_TRACK_SIMULATION_STATS "Enable tracking of simulation statistics for the physics engine." OFF)
    message(STATUS "JoltPhysics_TRACK_SIMULATION_STATS: ${JoltPhysics_TRACK_SIMULATION_STATS}")

    option(JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION "Enable debug renderer in distribution builds for the physics engine." OFF)
    message(STATUS "JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION: ${JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION}")
    option(JoltPhysics_PROFILER_IN_DISTRIBUTION "Enable profiler in distribution builds for the physics engine." OFF)
    message(STATUS "JoltPhysics_PROFILER_IN_DISTRIBUTION: ${JoltPhysics_PROFILER_IN_DISTRIBUTION}")

    if(JoltPhysics_USE_ASSERTS)
        set(JoltPhysics_USE_ASSERTS_v 1)
    else()
        set(JoltPhysics_USE_ASSERTS_v 0)
    endif()
    if(JoltPhysics_DOUBLE_PRECISION)
        set(JoltPhysics_DOUBLE_PRECISION_v 1)
    else()
        set(JoltPhysics_DOUBLE_PRECISION_v 0)
    endif()
    if(JoltPhysics_CROSS_PLATFORM_DETERMINISTIC)
        set(JoltPhysics_CROSS_PLATFORM_DETERMINISTIC_v 1)
    else()
        set(JoltPhysics_CROSS_PLATFORM_DETERMINISTIC_v 0)
    endif()
    if(JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED)
        set(JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED_v 1)
    else()
        set(JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED_v 0)
    endif()
    if(JoltPhysics_TRACK_BROADPHASE_STATS)
        set(JoltPhysics_TRACK_BROADPHASE_STATS_v 1)
    else()
        set(JoltPhysics_TRACK_BROADPHASE_STATS_v 0)
    endif()
    if(JoltPhysics_TRACK_NARROWPHASE_STATS)
        set(JoltPhysics_TRACK_NARROWPHASE_STATS_v 1)
    else()
        set(JoltPhysics_TRACK_NARROWPHASE_STATS_v 0)
    endif()
    if(JoltPhysics_TRACK_SIMULATION_STATS)
        set(JoltPhysics_TRACK_SIMULATION_STATS_v 1)
    else()
        set(JoltPhysics_TRACK_SIMULATION_STATS_v 0)
    endif()
    if(JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION)
        set(JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION_v 1)
    else()
        set(JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION_v 0)
    endif()
    if(JoltPhysics_PROFILER_IN_DISTRIBUTION)
        set(JoltPhysics_PROFILER_IN_DISTRIBUTION_v 1)
    else()
        set(JoltPhysics_PROFILER_IN_DISTRIBUTION_v 0)
    endif()
    target_compile_definitions(${_scoped_module_name} PRIVATE
        JoltPhysics_USE_ASSERTS=${JoltPhysics_USE_ASSERTS_v}
        JoltPhysics_DOUBLE_PRECISION=${JoltPhysics_DOUBLE_PRECISION_v}
        JoltPhysics_CROSS_PLATFORM_DETERMINISTIC=${JoltPhysics_CROSS_PLATFORM_DETERMINISTIC_v}
        JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED=${JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED_v}
        JoltPhysics_OBJECT_LAYER_BITS=${JoltPhysics_OBJECT_LAYER_BITS}
        JoltPhysics_TRACK_BROADPHASE_STATS=${JoltPhysics_TRACK_BROADPHASE_STATS_v}
        JoltPhysics_TRACK_NARROWPHASE_STATS=${JoltPhysics_TRACK_NARROWPHASE_STATS_v}
        JoltPhysics_TRACK_SIMULATION_STATS=${JoltPhysics_TRACK_SIMULATION_STATS_v}
        JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION=${JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION_v}
        JoltPhysics_PROFILER_IN_DISTRIBUTION=${JoltPhysics_PROFILER_IN_DISTRIBUTION_v}
        )
    target_compile_definitions(${_scoped_module_name} PUBLIC
        JAFG_PHYSICS_LAYER_BITS=${JoltPhysics_OBJECT_LAYER_BITS}
        )

    if(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_LINUX)
        set(JoltPhysics_LIB "${CMAKE_CURRENT_BINARY_DIR}/joltPhysics/src/joltPhysics-build/libJolt.a")
    else()
        message(FATAL_ERROR "Missing implementation for platform [${JAFG_TARGET_PLATFORM}].")
    endif()
    message(STATUS "JoltPhysics_LIB: ${JoltPhysics_LIB}")

    ExternalProject_Add(
        joltPhysics
        GIT_REPOSITORY      https://github.com/jrouwe/JoltPhysics
        GIT_TAG             e77f175595e64cb44218cc9d9d56fc365ad0e36a # v5.6.0
        GIT_PROGRESS        TRUE
        SOURCE_DIR          "${JAFG_ENGINE_ROOT}/Vendor/joltPhysics"
        SOURCE_SUBDIR       "Build"

        PREFIX              "${CMAKE_CURRENT_BINARY_DIR}/joltPhysics"

        CMAKE_ARGS
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON

            -DCMAKE_CXX_STANDARD=17
            -DCMAKE_CXX_STANDARD_REQUIRED=ON
            -DCMAKE_CXX_EXTENSIONS=OFF
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DUSE_ASSERTS=${JoltPhysics_USE_ASSERTS}
            -DDOUBLE_PRECISION=${JoltPhysics_DOUBLE_PRECISION}
            -DGENERATE_DEBUG_SYMBOLS=${JoltPhysics_GENERATE_DEBUG_SYMBOLS}
            -DOVERRIDE_CXX_FLAGS=ON
            -DCROSS_PLATFORM_DETERMINISTIC=${JoltPhysics_CROSS_PLATFORM_DETERMINISTIC}
            -DINTERPROCEDURAL_OPTIMIZATION=ON
            -DFLOATING_POINT_EXCEPTIONS_ENABLED=${JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED}
            -DCPP_EXCEPTIONS_ENABLED=OFF
            -DCPP_RTTI_ENABLED=OFF
            -DOBJECT_LAYER_BITS=${JoltPhysics_OBJECT_LAYER_BITS}

            -DUSE_SSE4_1=ON
            -DUSE_SSE4_2=ON
            -DUSE_AVX=ON
            -DUSE_AVX2=ON
            -DUSE_AVX512=OFF
            -DUSE_LZCNT=ON
            -DUSE_TZCNT=ON
            -DUSE_F16C=ON
            -DUSE_FMADD=ON

            -DUSE_WASM_SIMD=OFF
            -DJPH_USE_WASM64=OFF

            -DENABLE_ALL_WARNINGS=ON

            -DTRACK_BROADPHASE_STATS=${JoltPhysics_TRACK_BROADPHASE_STATS}
            -DTRACK_NARROWPHASE_STATS=${JoltPhysics_TRACK_NARROWPHASE_STATS}
            -DJPH_TRACK_SIMULATION_STATS=${JoltPhysics_TRACK_SIMULATION_STATS}

            -DDEBUG_RENDERER_IN_DEBUG_AND_RELEASE=ON
            -DDEBUG_RENDERER_IN_DISTRIBUTION=${JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION}
            -DPROFILER_IN_DEBUG_AND_RELEASE=ON
            -DPROFILER_IN_DISTRIBUTION=${JoltPhysics_PROFILER_IN_DISTRIBUTION}

            -DJPH_USE_EXTERNAL_PROFILE=OFF
            -DDISABLE_CUSTOM_ALLOCATOR=OFF
            -DUSE_STD_VECTOR=OFF

            -DENABLE_OBJECT_STREAM=OFF
            -DENABLE_INSTALL=OFF

            -DJPH_USE_DX12=OFF
            -DJPH_USE_VK=OFF
            -DJPH_USE_MTL=OFF
            -DJPH_USE_CPU_COMPUTE=OFF
            -DJPH_SHADER_DEBUG_SYMBOLS=OFF
            -DJPH_SHADER_OPTIMIZATION=ON

        BUILD_BYPRODUCTS
            "${JoltPhysics_LIB}"

        INSTALL_COMMAND ""
        )
    add_library(Jolt STATIC IMPORTED GLOBAL)
    set_target_properties(Jolt PROPERTIES
        IMPORTED_LOCATION "${JoltPhysics_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${JAFG_ENGINE_ROOT}/Vendor/joltPhysics"
        )
    add_dependencies(Jolt joltPhysics)
    target_link_libraries(${_scoped_module_name} PRIVATE Jolt)

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
    if(NOT DEFINED JAFG_CONTENT_VERSION)
        set(JAFG_CONTENT_VERSION "v${_detail_jafg_version}")
    endif()
    message(STATUS "JAFG_CONTENT_VERSION: ${JAFG_CONTENT_VERSION}")
    set(JAFG_REMOTE_CONTENT_SERVER "https://api.zoeschinger.com/get-jafg-content/${JAFG_CONTENT_VERSION}/" CACHE STRING "Remote content server URL for Jafg content files.")
    set(JAFG_CONTENT_FILE "${JAFG_ENGINE_ROOT}/Content/.lfs")
    file(STRINGS "${JAFG_CONTENT_FILE}" JAFG_CONTENT_FILES)

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

    function(jafg_get_manifest_hash OUT_HASH FILE_PATH)
        set(${OUT_HASH} "NONE" PARENT_SCOPE)
        foreach(LINE IN LISTS JAFG_REMOTE_CONTENT_FILES)
            string(REGEX MATCH "^([^ ]+)[ ]+(.+)$" MATCHED "${LINE}")
            if(MATCHED)
                set(MANIFEST_HASH "${CMAKE_MATCH_1}")
                set(MANIFEST_PATH "${CMAKE_MATCH_2}")
                if(MANIFEST_PATH STREQUAL "${FILE_PATH}")
                    set(${OUT_HASH} "${MANIFEST_HASH}" PARENT_SCOPE)
                    return()
                endif()
            endif()
        endforeach()
    endfunction()

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
