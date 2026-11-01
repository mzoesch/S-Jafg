# Copyright mzoesch. All rights reserved.

FROM archlinux:latest

ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=1000

RUN pacman -Syu --noconfirm
RUN pacman -Syy --noconfirm

RUN sed -i 's/^#ParallelDownloads.*/ParallelDownloads = 10/' /etc/pacman.conf \
    && pacman -Syu --noconfirm reflector \
    && reflector --latest 10 --protocol https --sort rate --save /etc/pacman.d/mirrorlist

# Core
RUN pacman -S --needed --noconfirm \
    base-devel \
    sudo \
    curl
RUN groupadd --gid "${USER_GID}" "${USERNAME}" \
    && useradd --uid ${USER_UID} --gid ${USER_GID} -m -s /bin/bash ${USERNAME} \
    && echo "${USERNAME} ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/${USERNAME} \
    && chmod 0440 /etc/sudoers.d/${USERNAME}

# Tools
RUN pacman -S --needed --noconfirm \
    python \
    git \
    cmake \
    gcc \
    llvm \
    lld \
    clang \
    ccache \
    gdb \
    ninja

# Toolchains
RUN pacman -S --noconfirm mingw-w64-toolchain

# Rust
RUN pacman -S --noconfirm rustup
RUN rustup default stable

# Vulkan
RUN pacman -S --noconfirm \
    vulkan-icd-loader \
    vulkan-headers \
    vulkan-tools
# RUN pacman -S --noconfirm \
#     mingw-w64-x86_64-vulkan-devel
#https://packages.msys2.org/groups/mingw-w64-x86_64-vulkan-devel
#RUN mkdir -p /opt/mingw-vulkan
#RUN curl -fsSL \
#    https://mirror.msys2.org/mingw/mingw64/mingw-w64-x86_64-vulkan-headers-1~1.4.363.0-1-any.pkg.tar.zst \
#    -o /tmp/vulkan-headers.pkg.tar.zst \
#    && tar --zstd -xf /tmp/vulkan-headers.pkg.tar.zst \
#        -C /opt/mingw-vulkan \
#    && rm /tmp/vulkan-headers.pkg.tar.zst

# Frontend
RUN pacman -S --noconfirm \
    mesa \
    libxrandr \
    libxkbcommon \
    libxinerama \
    libxcursor \
    libxi
# Required by NFD-Ext.
RUN pacman -S --noconfirm gtk3

# Misc
RUN pacman -Scc --noconfirm
USER ${USERNAME}
RUN echo "alias ll='ls -las --color=auto'" >> ~/.bashrc

ENV CCACHE_DIR=/home/${USERNAME}/.ccache

ENV JAFG_IS_DOCKER_CONTAINER=1
ENV JAFG_LINUX_C_COMPILER=/usr/bin/clang
ENV JAFG_LINUX_CXX_COMPILER=/usr/bin/clang++
ENV JAFG_WINDOWS_C_COMPILER=/usr/bin/x86_64-w64-mingw32-gcc
ENV JAFG_WINDOWS_CXX_COMPILER=/usr/bin/x86_64-w64-mingw32-g++

WORKDIR /Jafg
