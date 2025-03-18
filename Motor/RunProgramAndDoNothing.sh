#!/bin/bash

# Copyright mzoesch. All rights reserved.

# Development only script.

cd ..

echo "Calling Python to generate solution files ..."

./.venv/bin/python ./Program.py -INVOKE -- --DoNothing

cd -

read -n1 -r -p "Press any key to continue..."
