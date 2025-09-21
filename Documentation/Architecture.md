# High Level Project Structure 

![Architecture](Images/Architecture.png)

## Directory Structure
- **.github**: GitHub related files such as workflows, ...
- **Binaries (generated)**: Contains the exported binaries of the Jafg engine and plugins.
- **CMake**: Contains CMake files obviously.
- **Content**: The default content used by Jafg. The content may be overridden by plugins enabled by the user. Content includes fonts, shaders, textures, ...
- **Debug**: Contains debug related files and scripts used for pretty printing of Jafg data structures or other debugging purposes.
- **Documentation**: Contains this.
- **Engine**: A collection of shared libraries that contains the core functionality of Jafg. It defines important interfaces that allow plugins to hook themselves into it and communicate with others.
- **Hook**: The Rest Api Motor Hook endpoint that is used to update the Motor binaries and other meta-data automatically.
- **Intermediates (generated)**: Generated header and translation unit files that are used by the engine and plugins to compile their objects which inherit from `Jafg::JObjectBase` or other custom Jafg data structures used for reflection.
- **Lal**: A static library that defines core data structures such as arrays, strings, queues, ... that are optimized for Jafg. These types should be used instead of the std implementation when possible.
- **Motor**: A Rust tool for generating compiler files for C++, reflect all modules of Jafg and generate boilerplate code.
- **Plugins**: A collection of shared libraries that are optional to the engine and can be loaded dynamically at runtime.
- **Programs**: A collection of programs and scripts that serve only a semi important role in the Jafg project.
- **Runtime**: Private entry point, containing private runtime related code that is not meant to be shared.
- **Saved (generated)**: Contains saved files such as configs, profiling data, dumps, screenshots, world data, ... This folder is generated but should generally not be deleted as it contains important user data.
