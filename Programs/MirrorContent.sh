#!/bin/bash
# Copyright mzoesch. All rights reserved.
#
# Mirrors every file listed in <src>/.lfs to <mirror>, keeping the relative layout.
#
#   --shallow (default)  Skips files whose destination is newer (rsync --update).
#                        Size + mtime quick check; fast.
#   --deep               Compares file contents by checksum (rsync --checksum).
#                        Slower; catches content changes that keep size and mtime.

set -eo pipefail

usage()
{
    echo "Usage: $0 [--shallow|--deep] <src> <mirror>"
    exit 1
}

MODE="--update"
case "$1" in
    --shallow) MODE="--update";   shift ;;
    --deep)    MODE="--checksum"; shift ;;
    -*)        usage ;;
esac

if [ "$#" -ne 2 ]; then
    usage
fi

SRC="$1"
MIRROR="$2"

LFs="$SRC/.lfs"

if [ ! -d "$SRC" ]; then
    echo "$SRC: No such directory."
    exit 1
fi

if [ ! -f "$LFs" ]; then
    echo "$LFs: No such file."
    exit 1
fi

# Fail before copying anything if a listed file is missing.
Missing=0
while IFS= read -r file; do
    [[ -z "$file" ]] && continue
    if [ ! -f "$SRC/$file" ]; then
        echo "$SRC/$file: No such file."
        Missing=1
    fi
done < "$LFs"
[ "$Missing" -ne 0 ] && exit 1

mkdir -p "$MIRROR"

rsync -av -P "$MODE" --files-from="$LFs" "$SRC/" "$MIRROR/" || exit $?

echo "$MIRROR"
