#!/bin/bash
#  Copyright mzoesch. All rights reserved.

set -eo pipefail

if [ "$EUID" -ne 0 ]; then
    echo "Not root."
    exit 1
fi

cerbot renew --dry-run && cerbot renew && systemctl reload nginx
