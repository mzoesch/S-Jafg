# Copyright mzoesch. All rights reserved.

FROM archlinux:latest

RUN pacman -Syu --noconfirm

RUN pacman -S --noconfirm \
    base-devel \
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

RUN pacman -S --noconfirm rustup
RUN rustup default stable

RUN pacman -S --noconfirm \
    vulkan-icd-loader \
    vulkan-headers \
    vulkan-tools

RUN pacman -S --noconfirm \
    mesa \
    libxrandr \
    libxkbcommon \
    libxinerama \
    libxcursor \
    libxi

# Required by NFD-Ext.
RUN pacman -S --noconfirm gtk3

RUN pacman -Scc --noconfirm

# Use clang as default.
ENV CC=/usr/bin/clang
ENV CXX=/usr/bin/clang++

ENV CCACHE_DIR=/root/.ccache

RUN echo "alias ll='ls -las --color=auto'" >> ~/.bashrc

WORKDIR /Jafg
CMD [ "/bin/bash" ]
