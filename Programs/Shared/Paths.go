// Copyright mzoesch. All rights reserved.

package Shared

import (
    "fmt"
    "strings"
)

var _AbsoluteEngineRoot string = ""

func SetAbsoluteEngineRoot(path string) {
    _AbsoluteEngineRoot = path
}
func GetAbsoluteEngineRoot() string {
    if _AbsoluteEngineRoot == "" {
        panic("Engine root directory is not set.")
    }
    return _AbsoluteEngineRoot
}

func ToAbsolutePath(relPath string) string {
    return fmt.Sprintf("%s/%s", GetAbsoluteEngineRoot(), relPath)
}

func ToRelativePath(absPath string) string {
    if !strings.Contains(absPath, GetAbsoluteEngineRoot()) {
        panic(fmt.Sprintf("Path [%s] is not a subpath of [%s].", absPath, GetAbsoluteEngineRoot()))
    }

    return absPath[len(GetAbsoluteEngineRoot())+1:]
}

func NormalizePath(path string) string {
    return strings.ReplaceAll(path, "\\", "/")
}
