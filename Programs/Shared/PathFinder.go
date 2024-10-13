// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "fmt"
    "os"
)

func GetAbsolutePath(relPath string) (string /* absPath */, bool /* bExists */) {
    var absPath string = GApp.GetEngineRootDir() + "/" + relPath

    if _, err := os.Stat(absPath); !os.IsNotExist(err) {
        return absPath, true
    }

    return absPath, false
}

func GetAbsoluteFilePath(relPath string) (string /* absPath */, bool /* bExists */) {
    var absPath string = GApp.GetEngineRootDir() + "/" + relPath

    if _, err := os.Stat(absPath); !os.IsNotExist(err) {
        return absPath, true
    }

    return absPath, false
}

func GetBinTargetRelativePath(system string, architecture string, buildConfig string, subPath string) string {
    return fmt.Sprintf("%s/%s-%s/%s/%s", BinariesDir, system, architecture, buildConfig, subPath)
}
