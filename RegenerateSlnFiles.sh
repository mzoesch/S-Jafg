#!/bin/bash
# Copyright mzoesch. All rights reserved.

set -e

pause_if_needed()
{
    if [[ "$1" == "from_setup_command" ]];
    then
        read -p "Press [Enter] to exit."
    fi
}

VENV_DIR="./.venv"
echo "Running Program.py ..."
$VENV_DIR/bin/python ./Program.py -UpdateCachedData -INVOKE -- --SolutionGenerator GenerateAll

pause_if_needed "$1"
