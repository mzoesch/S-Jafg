#!/bin/bash
# Copyright mzoesch. All rights reserved.
# Will recompile the scripts for this program.

set -e

pause_if_needed()
{
    if [[ "$1" == "from_setup_command" ]];
    then
        read -n1 -r -p "Press any key to continue..."
    fi
}

cd ..

echo "Calling Python to generate solution files ..."
./.venv/bin/python ./Program.py -ValidateRust -UpdateCachedData -INVOKE -- --SolutionGenerator GenerateAll

cd -

pause_if_needed "$1"
