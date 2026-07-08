#  Copyright mzoesch. All rights reserved.

pacman -S --noconfirm \
    mingw-w64-ucrt-x86_64-toolchain \
    mingw-w64-ucrt-x86_64-clang \
    mingw-w64-ucrt-x86_64-lldb \
    mingw-w64-ucrt-x86_64-cmake \
    mingw-w64-ucrt-x86_64-vulkan-headers \
    mingw-w64-ucrt-x86_64-vulkan-loader \
    mingw-w64-ucrt-x86_64-vulkan-validation-layers

pacman -Syu --noconfirm
