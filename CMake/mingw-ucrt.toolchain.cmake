if((NOT DEFINED ENV{JAFG_IS_DOCKER_CONTAINER}) OR (NOT ($ENV{JAFG_IS_DOCKER_CONTAINER} STREQUAL "1")))
    message(FATAL_ERROR "Toolchain files are only allowed inside docker containers.")
endif()

# Cross compiles to x86_64 Windows (UCRT) with clang and lld from llvm-mingw against an MSYS2 UCRT64
# sysroot (mingw-w64 CRT, libgcc, libstdc++), which mirrors a native MSYS2 UCRT64 build.
# @see .devcontainer/Dockerfile.
foreach(_var JAFG_WINDOWS_C_COMPILER JAFG_WINDOWS_CXX_COMPILER JAFG_WINDOWS_RC_COMPILER JAFG_WINDOWS_SYSROOT)
    if(NOT DEFINED ENV{${_var}})
        message(FATAL_ERROR "Environment variable [${_var}] is not defined. Rebuild the dev container.")
    endif()
endforeach()

set(CMAKE_SYSTEM_NAME      Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER    $ENV{JAFG_WINDOWS_C_COMPILER})
set(CMAKE_CXX_COMPILER  $ENV{JAFG_WINDOWS_CXX_COMPILER})
set(CMAKE_RC_COMPILER   $ENV{JAFG_WINDOWS_RC_COMPILER})

set(CMAKE_SYSROOT         $ENV{JAFG_WINDOWS_SYSROOT})
set(CMAKE_FIND_ROOT_PATH $ENV{JAFG_WINDOWS_SYSROOT})

# llvm-mingw defaults to its own libc++, compiler-rt and libunwind. Use the GNU runtime of the sysroot.
# With libgcc as the runtime library the driver links the libgcc unwinder on its own.
set(CMAKE_CXX_FLAGS_INIT "-stdlib=libstdc++")
# libstdc++ uses winpthreads for its threading primitives. GCC links it implicitly, clang only with -pthread.
foreach(_kind EXE SHARED MODULE)
    set(CMAKE_${_kind}_LINKER_FLAGS_INIT "-rtlib=libgcc -pthread")
endforeach()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
