# The fundamental structure of this project

## Top Level Directories
### Content
A non-code plugin that contains the content that come as a default with this application. 
This includes default fonts, shaders, widget-textures, and other assets that are used by the application.
### Lal
A static library that abstracts C++ to a more suitable platform agnostic language.
Defining platform specific abstractions and providing fundamental classes and data types
such as arrays, strings, math-stuff etc. that are optimized for this application. 
### Core
A dynamic linked library that contains the core functionality of this application.
This library contains public interfaces, classes, etc. that are used by other Plugins.
### Runtime
Private entry point for this application containing runtime related code that is not meant to be shared.
### Programs
Containing programs that are not part of the C++ master application such as boilerplate code
generators, build scripts, documentation generators, website integration, etc.
