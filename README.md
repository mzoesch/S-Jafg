# S-Jafg
OpenGL-based engine. Currently, in very early alpha.

As of now, it runs natively on Windows and Linux; and is emulated in / on the [browser](https://api.zoeschinger.com/static/Jafg/Runtime.html).


# Getting up and running

## Prerequisites

You will need to have the `C/C++ toolchain` with [CMake](https://cmake.org/) installed for your host platform for the `C++23` dialect:
- <b>For Windows</b>: 
    - [Visual Studio 2022](https://visualstudio.microsoft.com/en-Us/vs/features/cplusplus/) with at least [Desktop development with C++](https://learn.microsoft.com/en-us/cpp/build/vscpp-step-0-installation?view=msvc-170).
- <b>For Linux</b>:
    - [Clang](https://clang.llvm.org/); no [GCC](https://gcc.gnu.org/) support for now.
    - [Wayland](https://wayland.freedesktop.org/) backend ([X11](https://www.x.org/wiki/) is not supported) with [wayland development software kit](https://wiki.archlinux.org/title/Wayland).
- <b>For the Web</b>:
    - [GCC](https://gcc.gnu.org/) and [Emscripten](https://emscripten.org/) for your host platform.
    - Windows only: Additionally install either [WSL](https://learn.microsoft.com/en-Us/windows/wsl/install) (if you choose to use WSL, install GCC, EM and CMake for WSL and *not* for Windows) or [MSYS2 MINGW-x64](https://www.mingw-w64.org/).
    - For serving only:
        - Any web-server that allows XOrigin-Embedder policies *AND* XOrigin-Opener policies; because opening the generated `.html` file emitted by EM directly in the browser will not work as the browser will block the execution of Wasm code from untrusted sources; and you are an untrusted source.
        - An optional development [Node](https://nodejs.org/) server is provided. (If you want to use that service, then you will obviously need to have Node and [npm](https://www.npmjs.com/) installed.)

Only when making changes to the reflection system you will additionally need `>= Rust 1.85.x` installed otherwise prebuild binaries will be downloaded.

## Generate solution and build

### For all native target platforms
1. To build for your hosting platform in release mode, run:
   ```bash
   mkdir build && cmake -B build 
   ```
   If you want to customize the build configuration, run the following instead:
   ```bash
   mkdir build && cmake -B build --preset "<PLATFORM> | <TYPE>-<CONFIG>" 
   ```
   with:
   - `PLATFORM`: The target platform you want to build for [`Linux`, `Windows`, `Wasm`]
   - `TYPE`: The target type you want to build for [`Client`, `Daemon`, `TestUnit`]
   - `CONFIG`: The target configuration you want to build for [`Debug`, `Development`, `Shipping`]
2. Compile it with `cmake --build build --target Runtime` (The `Runtime` target is the main executable of Jafg).
3. Then run the executable located in `Binaries/.../Runtime/Runtime(.exe)`

### For WebAssembly (Wasm)
1. Emscripten must be invoked before generating the CMake files. This is done by calling `emcmake` with the `cmake` command:
   ```bash
   mkdir build && emcmake cmake -B build --preset "Wasm | <TYPE>-<CONFIG>" 
   ```
   After running this command, you may open your project with any IDE that supports CMake. If you firstly open the project (before running the `emcmake` command) then the IDE will get confused with the code and will fail to compile it (as well as Intellisense will not work properly).
2. Compile it with `ninja`.
3. To serve, go to `Programs/ServerWasm` and run `npm install http-server`.
4. Launch the resulting Node server with `node Launch.js`.
5. In your favorite browser go to `http://localhost:8080/`.
