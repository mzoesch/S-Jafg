# S-Jafg
Voxel based game. Currently in very early development.

@see [Fundamental Project Structure](Documentation/ProjectStructure.md)

# Getting up and running

## Prerequisites
You will need `Python 3.11.x`.

You will need to have the `C/C++ toolchain` installed for your platform. GNU/GCC and MSVC
are supported. The target dialect is `C++20`. For some [platforms](Documentation/Platforms.md)
you will also need to have CMake installed. Cmake scripts will always be generated even
if it is not installed on the host machine.

When making changes to the reflection system you will need `Go 1.23.` installed.
Otherwise a precompiled binary is automatically downloaded by the build system.

## Generate solution and build
Double click / run the `Setup` script for your platform (`Setup.bat` for 
Windows, `Setup.command` for Linux and MacOS). This will download and generate
the solution files for your platform.

Open the generated `Jafg-All.sln` file with either Rider or Visual Studio, select
`Debug-Client` and the `Runtime` configuration and run the project.

Alternatively you can use the `CMakeLists.txt` file to generate for any (multi)-generator
although this is not recommended as it is near impossible to get good intellisense (in
context of developing multi platforms files) with the resulting project files - even
when generating for Visual Studio.
