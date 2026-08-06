#!/bin/bash
#  Copyright mzoesch. All rights reserved.

#
# An automated script to generate a manifest file for Jafg content to speedup download checks.
#

set -eo pipefail

if [ -z "$jafg_manifest_update_dir" ]; then
    if [ -z "$jafg_content_root" ]; then
        echo "Not defined: <jafg_content_root>"
        exit 1
    fi
    if [ -z "$jafg_content_version" ]; then
        echo "Not defined: <jafg_content_version>"
        exit 1
    fi
    jafg_manifest_update_dir="$jafg_content_root/$jafg_content_version"
fi

if [ ! -d "$jafg_manifest_update_dir" ]; then
    echo "Directory does not exist: $jafg_manifest_update_dir"
    exit 1
fi

OUTPUT="$jafg_manifest_update_dir/manifest"
if [ -f "$OUTPUT" ]; then
    rm "$OUTPUT"
fi

find "$jafg_manifest_update_dir" -type f | while read -r file; do
    relative_path="${file#$jafg_manifest_update_dir/}"
    hash=$(sha256sum "$file" | awk '{print $1}')
    echo ${hash} ${relative_path} >> "$OUTPUT"
done

echo $OUTPUT
