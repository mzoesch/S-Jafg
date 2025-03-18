# S-Jafg
Voxel based engine / game. Currently in very early development.
Currently runs natively on Windows and on the browser.

When developing: <br/>
@see [Fundamental Project Structure](Documentation/ProjectStructure.md)



# Getting up and running


## Prerequisites
You will need `Python 3.11.x` or higher.

You will need to have a `C/C++ toolchain` installed for your hosting platform for the `C++20` dialect:
- For Windows: [Visual Studio 2022](https://visualstudio.microsoft.com/en-Us/vs/features/cplusplus/) with at least:
  - "Desktop development with C++"
  - "Game development with C++"
- For the Web:
  - [GCC](https://gcc.gnu.org/), [Emscripten](https://emscripten.org/) (and [CMake](https://cmake.org/) - only recommended) for your (host) platform.
  - Windows only: Additionally install either [WSL](https://learn.microsoft.com/en-Us/windows/wsl/install) (if WSL install GCC,... for WSL not for Windows) or [MSYS2 MINGW-x64](https://www.mingw-w64.org/).
  - For serving only:
    - Any web-server that allows XOrigin-Embedder policies AND XOrigin-Opener policies.
    - An optional development Node server is provided. (If you want to use that, then you will obviously need to have Node and npm installed.)

Only when making changes to the reflection system you will additionally need `>= Rust 1.85.x`.


## Generate solution and build

### For Native Target Platforms
1. Double click / run the `~/Setup` script for your platform (`~/Setup.bat` (Win), `~/Setup.command` (Darwin) or `~/Setup.sh` (Linux)). This will download and generate the solution files for your host platform.
2. Open the generated `~/Jafg-All.sln` file with JetBrain's Rider, Visual Studio, XCode or any IDE that can read and understand solution files. Select your native Platform, `Client-Debug` and the !!`Runtime`!! configuration and run the project.

### For WebAssembly (Wasm)
1. Double click / run the `~/Setup` script for your platform. This will download and generate the solution files for your host platform.
2. Go to `~/Saved/<Target-Solution>`. E.g to compile only the minimal required engine modules go to `~/Saved/Runtime/C_SLN_EngineOnly`. In this directory there is the `CMakeLists.txt` located to generate all CMake targets. 
3. Then run under the `Wasm` directory the `generate_files.X` inside your build configuration, e.g. `Debug-Client`.
4. After that you will have to run the `build.X` (located in the same directory) !at least once! before opening the CMake-Project in any IDE (the directory where the `CMakeLits.txt` is located at). !!!This is extremely important. If the CMake-Project is opened with Visual Studio, CLion (or any other IDE) first, then Emscripten will not have time to modify the generated build-files and the IDE will fail to compile!!!
5. The compiled Java-Script and Wasm code with an additional minimalistic HTML-Body is located under the `~/Binaries` directory.
6. Go to`~/Programs/ServerWasm` and run `npm install http-server`.
7. Launch the resulting Node server with `node Launch.js`.
8. In your favorite browser go to `http://localhost:8080/`.

### XCompiling
Figure it out yourself.
