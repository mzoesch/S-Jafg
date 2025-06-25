# S-Jafg
Voxel-based engine / game. Currently, in very early development. <br />
As of now, it runs natively on Windows and Linux (Wayland) and is emulated in on the [browser](https://api.zoeschinger.com/static/Jafg/Runtime.html).

When developing @see [Fundamental Project Structure](Documentation/ProjectStructure.md)


# Getting up and running

## Prerequisites

You will need to have [CMake](https://cmake.org/) installed for your host platform.

You will need to have a `C/C++ toolchain` installed for your host platform for the `C++23` dialect:
- For Windows: [Visual Studio 2022](https://visualstudio.microsoft.com/en-Us/vs/features/cplusplus/) with at least:
  - "Desktop development with C++"
- For Linux: 
  - [Clang](https://clang.llvm.org/) and only clang.
  - [Wayland](https://wayland.freedesktop.org/) backend (X11 is not supported) with [wayland development software kit](https://wiki.archlinux.org/title/Wayland).
- For the Web:
  - [GCC](https://gcc.gnu.org/) and [Emscripten](https://emscripten.org/) for your host platform.
  - Windows only: Additionally install either [WSL](https://learn.microsoft.com/en-Us/windows/wsl/install) (if WSL install GCC,... for WSL not for Windows) or [MSYS2 MINGW-x64](https://www.mingw-w64.org/).
  - For serving only:
    - Any web-server that allows XOrigin-Embedder policies AND XOrigin-Opener policies. Opening the generated `.html` files emitted by the compiler directly in the browser will not work as the browser will block the execution of the WebAssembly code from untrusted sources.
    - An optional development [Node](https://nodejs.org/) server is provided. (If you want to use that service, then you will obviously need to have Node and [npm](https://www.npmjs.com/) installed.)

Only when making changes to the reflection system you will additionally need `>= Rust 1.85.x` installed otherwise prebuild binaries will be downloaded.

## Generate solution and build

### For all native target platforms
This project uses a CMake build system. You may either use the predefined presets or run it manually by defining the following: 
   - `JAFG_TARGET_PLATFORM`: The target platform you want to build for [`Linux`, `Windows`, `Wasm`].
   - `JAFG_TARGET_TYPE`: The target type you want to build for [`Client`, `Daemon`, `TestUnit`].
   - `JAFG_TARGET_CONFIG`: The target configuration you want to build for [`Debug`, `Development`, `Shipping`].

### For WebAssembly (Wasm)
1. Emscripten must be invoked before generating the CMake files. This is done by calling `emcmake` with the `cmake` command. E.g., to generate the `Wasm` CMake files for the `Client` in the `Development` configuration, you would run:
   ```bash
   emcmake cmake -B . -S <BuildDirectory> --preset "Wasm | Client-Development"
   ```
   After running this command, you may open your project with any IDE that supports CMake. If you firstly open the project (before running the `emcmake` command) then the IDE will get confused with the code and will fail to compile it (as well as Intellisense will not work properly).
2. Compile it normally (as any CMake project would be compiled). 
3. To serve, go to `~/Programs/ServerWasm` and run `npm install http-server`.
4. Launch the resulting Node server with `node Launch.js`.
5. In your favorite browser go to `http://localhost:8080/`.
