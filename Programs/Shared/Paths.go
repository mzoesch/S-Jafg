// Copyright mzoesch. All rights reserved.

package Shared

import (
    "Jafg/Core"
    "fmt"
    "strings"
)

func ToAbsolutePath(relPath string) string {
    return fmt.Sprintf("%s/%s", Core.GetAbsoluteEngineRoot(), relPath)
}

func ToRelativePath(absPath string) string {
    if !strings.Contains(absPath, Core.GetAbsoluteEngineRoot()) {
        panic(fmt.Sprintf("Path [%s] is not a subpath of [%s].", absPath, Core.GetAbsoluteEngineRoot()))
    }

    return absPath[len(Core.GetAbsoluteEngineRoot())+1:]
}

func NormalizePath(path string) string {
    return strings.ReplaceAll(path, "\\", "/")
}
