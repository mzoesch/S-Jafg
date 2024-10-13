// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "fmt"
    "os"
)

var SolutionLuaFile string = "__buildSolution.lua"

func FullSolutionLuaPath() string {
    return GApp.GetEngineRootDir() + "/" + SolutionLuaFile
}

var ConfigDir string = "Config"

var BinariesDir string = "Binaries"

var IntermediateDir string = "Intermediate"

var GeneratedHeaderExtension string = ".generated.h"

var VendorIncludeDir string = "Engine/Vendor/Include"

var VendorLibDir string = "Engine/Vendor/Lib"

var ContentDir string = "Content"

var GeneratedHeadersDir string = "Vslf/gh"
func FullGeneratedHeadersDir() string {
    return GApp.GetEngineRootDir() + "/" + GeneratedHeadersDir
}

// CheckFile Assert that a file exists
func CheckFile(relPath string) {
    if _, err := os.Stat(GApp.GetEngineRootDir() + "/" + relPath); os.IsNotExist(err) {
        panic("File not found: " + relPath)
    }
}

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
