# S-Jafg

@see [Fundamental Project Structure](ProjectStructure.md)

// NOTES
mingw32-make for compiling make files on win
https://www.khronos.org/files/webgl/webgl-reference-card-1_0.pdf
https://www.khronos.org/files/webgl20-reference-guide.pdf

# Getting up and running

## Prerequisites
For anything and any platform you will need `Python 3.11.x`.

You will need to have the C/C++ toolchain installed for your platform. GNU/GCC and MSVC
are supported. The target C++ dialect is `C++20`. Cmake or Premake is installed automatically
by the build system.

When developing / making changes to the reflection system you will need `Go 1.23.x`
installed. A precompiled binary is automatically downloaded by the build system if Go is
not present. Not all platforms have precompiled binaries available so for those you will
need to have Go installed.

## Generate solution and build
Double click / run the `Setup` script for your platform (`Setup.bat` for 
Windows, `Setup.command` for Linux and MacOS). This will download and generate
the solution files for your platform.

Open the generated `Jafg.sln` file with either Rider or Visual Studio, select
`Debug-Client` and the `Runtime` configuration and run the project.

Alternatively you can use the `CMakeLists.txt` file to generate for any (multi)-generator
although this is not recommended as it is near impossible to get good intellisense (in
context of developing multi platforms files) with the resulting project files - even
when generating for Visual Studio.
