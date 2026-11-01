#!/bin/bash
set -euo pipefail

WORKSPACE_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if [ ! -d "${WORKSPACE_ROOT}/Temp" ]; then
    mkdir -p "${WORKSPACE_ROOT}/Temp"
    echo "${WORKSPACE_ROOT}/Temp"
fi

if [ ! -d "${WORKSPACE_ROOT}/Temp/.ccache" ]; then
    mkdir -p "${WORKSPACE_ROOT}/Temp/.ccache"
    echo "${WORKSPACE_ROOT}/Temp/.ccache"
fi
