#!/bin/bash
# Copyright mzoesch. All rights reserved.

#
# An automated script to generate a manifest file for Jafg content to speedup download checks.
#

set -eo pipefail

if [ -z "$lfs_dst" ]; then
    if [ -z "$lfs_root" ]; then
        echo "Not defined: <$lfs_root>"
        exit 1
    fi
    if [ -z "$lfs_version" ]; then
        echo "Not defined: <$lfs_version>"
        exit 1
    fi
    lfs_dst="$lfs_root/$lfs_version"
fi

if [ ! -d "$lfs_dst" ]; then
    echo "Directory does not exist: $lfs_dst"
    exit 1
fi

OUTPUT="$lfs_dst/manifest"
if [ -f "$OUTPUT" ]; then
    rm "$OUTPUT"
fi

find "$lfs_dst" -type f | while read -r file; do
    relative_path="${file#$lfs_dst/}"
    hash=$(sha256sum "$file" | awk '{print $1}')
    echo ${hash} ${relative_path} >> "$OUTPUT"
done

echo $OUTPUT
