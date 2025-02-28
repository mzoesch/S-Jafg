# High Level Project Structure 

### Content
The default content used by Jafg. The content may be overridden by plugins enabled by the user. Content includes fonts, shaders, textures, ... 

### Programs
Collection of programs and scripts that allow Jafg to compile and link. These include solution  generation, reflection, boilerplate code generation, etc.

### Engine
A collection of shared libraries that contains the core functionality of Jafg. It defines important interfaces that allow plugins to hook themselves into it and communicate with others.

### Lal
Static library that defines core data structures such as arrays, strings, queues, ... that are optimized for Jafg. These types should be used instead of the std implementation.

### Plugins
Shared libraries that are optional to the engine and can be loaded dynamically at runtime.

### Runtime
Private entry point, containing private runtime related code that is not meant to be shared.
