#!/bin/bash
# Copyright mzoesch. All rights reserved.

set -e

pause_if_needed()
{
    if [[ "$1" == "from_setup_command" ]];
    then
        read -p read -n1 -r -p "Press any key to continue..."
    fi
}

cd "$(dirname "$0")"

VENV_DIR="./.venv"

if ! command -v python3 &> /dev/null || [[ "$(python3 -c 'import sys; print(sys.version_info >= (3, 11))')" != "True" ]]; then
    echo "Python >=3.11 is not installed or not in PATH."
    pause_if_needed "$1"
    exit 1
fi

echo "Creating virtual environment ($VENV_DIR/) ..."
python3 -m venv $VENV_DIR

echo "Activating virtual environment ..."
source $VENV_DIR/bin/activate

echo "Virtual environment activated with:"
python --version

echo "Running Program.py ..."
python ./Program.py -UpdateSubmodules -ValidatePython -ValidateRust -ValidatePremake -ValidateCmake -UpdateCachedData -INVOKE -- --SolutionGenerator GenerateAll EmulateCompiler

pause_if_needed "$1"
