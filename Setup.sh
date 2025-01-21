#!/bin/bash
# Copyright mzoesch. All rights reserved.

pause_if_needed()
{
    if [[ "$1" == "from_setup_command" ]];
    then
        read -p "Press [Enter] to exit."
    fi
}

# Set cwd.
cd "$(dirname "$0")"

VENV_DIR="./.venv"

if ! command -v python3.11 &> /dev/null;
then
    echo "Python 3.11 is not installed or not in PATH."
    pause_if_needed "$1"
    exit 1
fi

echo "Creating virtual environment ($VENV_DIR/) ..."
python3.11 -m venv $VENV_DIR

echo "Activating virtual environment ..."
source $VENV_DIR/bin/activate

echo "Virtual environment activated with:"
python --version

echo "Running Program.py ..."
python ./Program.py --UpdateSubmodules --ValidatePython --ValidateGo --ValidatePremake AllPlatforms --ValidateCmake --UpdateCachedData --INVOKE --SolutionGenerator GenerateAll EmulateCompiler

pause_if_needed "$1"
