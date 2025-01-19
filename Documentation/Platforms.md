# Windows
Requires MSVC for `C++20`.

# Wasm
Requires Cmake installed and [emscripten](https://emscripten.org/).
Emscripten needs to be added to the hosts PATH or set manually in
one of the generated `CMakeLists.txt` files.

The latest version of Jafg currently does not compile to Wasm.
[This](https://github.com/mzoesch/S-Jafg/releases/tag/Wasm-lookup)
is the latest version of Jafg with Wasm fully working.
Wasm support will be re-added later on when more of Windows specific
code is abstracted and other platforms such as OSX and Linux are fully supported.
