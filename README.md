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

# When using Jetbrains Rider!!!
Multiline comments and docs should / must never be encountered with the Jafg Build Tool. These include:
```C++
///     1.
///

/*      2.
 */

/**     3.
 */

...

// Use this instead for multiline comments.
//

//# And that for multiline docs.
//#
```

__Why?:__ 

Because of [this](https://youtrack.jetbrains.com/issue/RSCPP-34376/Unreal-IDE-and-the-compiler-expand-LINE-macro-differently-after-multi-line-comment-blocks) bug.
The Jafg build tool makes heavy use of the `__LINE__` macro in C++ but [R++](https://www.jetbrains.com/resharper-cpp/) expands the `__LINE__` macro wrong
(by not counting the multiline comments / docs as separated lines but only as one single line - this is of course completely wrong) which results in complete garbage 
errors in the intellisense (it will though compile with no errors). Visual Studio is not affected by this.

The codebase therefore (because I use Rider) does not contain any multiline docs and comments...
