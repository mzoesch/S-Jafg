#!/bin/bash
# Copyright mzoesch. All rights reserved.
# Development only script.

set -e

pause_if_needed()
{
    if [[ "$1" == "from_setup_command" ]];
    then
        read -p read -n1 -r -p "Press any key to continue..."
    fi
}

cd ..

echo "Calling Python to generate solution files ..."
./.venv/bin/python ./Program.py -DevRun

cd -

pause_if_needed "$1"
