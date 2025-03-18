#!/bin/bash

# Copyright mzoesch. All rights reserved.

# Will recompile the scripts for this program.

cd ..

echo "Calling Python to generate solution files ..."

./.venv/bin/python ./Program.py -ValidateRust

cd -

read -n1 -r -p "Press any key to continue..."
