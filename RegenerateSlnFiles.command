#!/bin/sh
# Copyright mzoesch. All rights reserved.

# This file is just a wrapper around RegenerateSlnFiles.sh to make it executable
# from the Finder via a smooth and cool double-click.

sh "`dirname "$0"`"/RegenerateSlnFiles.sh from_setup_command
