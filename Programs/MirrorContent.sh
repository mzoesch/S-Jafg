#!/bin/bash
#  Copyright mzoesch. All rights reserved.

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <src> <mirror>"
    exit 1
fi

SRC="$1"
MIRROR="$2"

LFs="$SRC/lfs"

if [ ! -d "$SRC" ]; then
    echo "$SRC: No such directory."
    exit 1
fi

if [ ! -f "$LFs" ]; then
    echo "$LFs: No such file."
    exit 1
fi

if [ -d "$MIRROR" ]; then
    echo "$MIRROR: Directory already exists."
    exit 1
fi

mkdir -p "$MIRROR"

while IFS= read -r file; do
    [[ -z "$file" ]] && continue

    src="$SRC/$file"
    dst="$MIRROR/$file"

    if [ ! -f "$src" ]; then
        echo "$src: No such file."
        exit 1
    fi

    mkdir -p "$(dirname "$dst")"
    echo "$src -> $dst"
    cp "$src" "$dst"
done < "$LFs"

echo "$MIRROR"
