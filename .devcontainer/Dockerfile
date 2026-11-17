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

# Windows cross toolchain (x86_64-w64-mingw32, UCRT).
# Compiler and linker: clang + lld from llvm-mingw. Deliberately not added to PATH, as its bin
# directory also ships an unprefixed clang that would shadow the system compiler.
# @note Use with JAFG_WINDOWS_* env variables below.
ARG LLVM_MINGW_VERSION=20261104
ENV JAFG_WINDOWS_TOOLCHAIN_ROOT=/opt/llvm-mingw
RUN mkdir -p "${JAFG_WINDOWS_TOOLCHAIN_ROOT}" \
    && curl -fsSL "https://github.com/mstorsjo/llvm-mingw/releases/download/${LLVM_MINGW_VERSION}/llvm-mingw-${LLVM_MINGW_VERSION}-ucrt-ubuntu-22.04-x86_64.tar.xz" \
        | bsdtar -xf - -C "${JAFG_WINDOWS_TOOLCHAIN_ROOT}" --strip-components=1

# Windows sysroot: CRT, Windows headers, winpthreads, libgcc, libstdc++ and Vulkan from the MSYS2
# UCRT64 repository, the same packages a native Windows build uses (see
# Programs/msys2_install_pkgs.sh). The engine requires libstdc++; the libc++ bundled with llvm-mingw
# lacks C++26 library features it relies on.
ENV JAFG_WINDOWS_SYSROOT=/opt/msys2-ucrt64
RUN set -eu \
    && mkdir -p /tmp/msys2 "${JAFG_WINDOWS_SYSROOT}" \
    && cd /tmp/msys2 \
    && curl -fsSL -o ucrt64.db https://repo.msys2.org/mingw/ucrt64/ucrt64.db \
    && for pkg in crt headers winpthreads libwinpthread gcc libgcc libstdc++ vulkan-headers vulkan-loader; do \
        file="$(bsdtar -xOf ucrt64.db "mingw-w64-ucrt-x86_64-${pkg}-[0-9]*/desc" | sed -n '/^%FILENAME%$/{n;p;q}')" \
        && test -n "${file}" \
        && curl -fsSL -o "${file}" "https://repo.msys2.org/mingw/ucrt64/${file}" \
        && bsdtar -xf "${file}" -C "${JAFG_WINDOWS_SYSROOT}" --strip-components=1 \
            --exclude 'ucrt64/bin/*.exe' \
            --exclude 'ucrt64/lib/gcc/*/*/*.exe' \
            --exclude 'ucrt64/libexec' \
            --exclude 'ucrt64/share' \
            ucrt64 \
        || exit 1; \
    done \
    && test -f "${JAFG_WINDOWS_SYSROOT}/include/windows.h" \
    && test -f "${JAFG_WINDOWS_SYSROOT}/lib/libstdc++.a" \
    && test -n "$(find "${JAFG_WINDOWS_SYSROOT}/include/c++" -name inplace_vector)" \
    && test -f "${JAFG_WINDOWS_SYSROOT}/include/vulkan/vulkan.h" \
    && test -f "${JAFG_WINDOWS_SYSROOT}/lib/libvulkan-1.dll.a" \
    && cd / \
    && rm -rf /tmp/msys2

# Rust
RUN pacman -S --noconfirm rustup
RUN rustup default stable

# Vulkan
RUN pacman -S --noconfirm \
    vulkan-icd-loader \
    vulkan-headers \
    vulkan-tools

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
ENV JAFG_WINDOWS_C_COMPILER=${JAFG_WINDOWS_TOOLCHAIN_ROOT}/bin/x86_64-w64-mingw32-clang
ENV JAFG_WINDOWS_CXX_COMPILER=${JAFG_WINDOWS_TOOLCHAIN_ROOT}/bin/x86_64-w64-mingw32-clang++
ENV JAFG_WINDOWS_RC_COMPILER=${JAFG_WINDOWS_TOOLCHAIN_ROOT}/bin/x86_64-w64-mingw32-windres

WORKDIR /Jafg
