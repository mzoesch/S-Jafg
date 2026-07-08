# S-Jafg
[Vulkan](https://vulkan.org/)-based engine. Currently, in very early alpha.

As of now, it runs natively on Linux and on Windows through the UCRT.

# Getting Up and Running

## 1.) Prerequisites
### Windows
- Run `Setup.bat` to install msys2 and required packages (all downloaded content and installed packages will reside in the engine root directory and are not installed system-wide).
- Open the `msys64/ucrt64.exe` shell.
- Follow the instructions for _Build and Run_.
### Linux
- Jafg targets [Wayland](https://wayland.freedesktop.org/) as its main backend. One should always prefer to use Wayland.
- [X11](https://www.x.org/) is supported due to gfx debugging (e.g. w/ [RenderDoc](https://renderdoc.org/)) but Jafg intentionally does not take full advantage of the X11 API (as Wayland is the future).
- Additionally install [Clang](https://clang.llvm.org/), [CMake](https://cmake.org/), and [Vulkan](https://vulkan.org/):
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
  mkdir build && cmake -B build --preset "<PLATFORM>-x64_86--<TYPE>-<CONFIG>" 
  ```
  where:
  - `PLATFORM`: The target platform you want to build for [`lnx`, `win`]
  - `TYPE`: The target type you want to build for [`editor`, `cl`, `daemon`, `test`]
  - `CONFIG`: The target configuration you want to build for [`sanitized`, `dbg`, `dev`, `ship`]
2. Compile it with `cmake --build build --target JgcRuntime` (The `JgcRuntime` target is the main executable of Jafg with core plugins enabled).
3. Then run the executable located in `build/Runtime/Runtime(.exe)`.
