# S-Jafg
Vulkan-based engine. Currently, in very early alpha.

As of now, it runs natively on Windows and Linux.

# Getting Up and Running

## 1.) Prerequisites
### Windows (MSVC) - recommended
- Install [Visual Studio 2026](https://visualstudio.microsoft.com) with at least the "Desktop development with C++" workload.
- Install the [Vulkan SDK](https://vulkan.lunarg.com).
- If not developing with Visual Studio, install additionally [CMake](https://cmake.org).
- Follow the instructions for _Build and Run_.
### Windows (clang)
- Run `Setup.bat` to install msys2 and required packages.
- Open the `msys64/ucrt64` shell. 
- Follow the instructions for _Build and Run_.
### Linux
- Currently only [Wayland](https://wayland.freedesktop.org/) backends are supported (with [wayland development software kit](https://wiki.archlinux.org/title/Wayland)).
- Install Clang, CMake, and Vulkan:
   ```bash
    pacman -S clang cmake vulkan-icd-loader vulkan-tools
   ```
- Follow the instructions for _Build and Run_.
### Common
Only when making changes to the reflection system you will additionally need `>= Rust 1.85.x` installed otherwise prebuild binaries will be downloaded.

## 2.) Build and Run
1. To build for your hosting platform in release mode, run:
   ```bash
   mkdir build && cmake -B build 
   ```
   If you want to customize the build configuration, run the following instead:
   ```bash
   mkdir build && cmake -B build --preset "<PLATFORM>-x64--<TYPE>-<CONFIG>" 
   ```
   with:
   - `PLATFORM`: The target platform you want to build for [`lnx`, `wub`]
   - `TYPE`: The target type you want to build for [`client`, `daemon`, `testunit`]
   - `CONFIG`: The target configuration you want to build for [`sanitized`, `debug`, `development`, `shipping`]
2. Compile it with `cmake --build build --target JgcRuntime` (The `JgcRuntime` target is the main executable of Jafg with core plugins enabled).
3. Then run the executable located in `Binaries/.../Runtime(.exe)`.
