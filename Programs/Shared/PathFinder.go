// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "errors"
    "fmt"
    "io"
    "io/fs"
    "os"
    "strings"
)

var SolutionLuaFile string = "__buildSolution.lua"

func GetAbsolutePathToSolutionLuaFile() string {
    return fmt.Sprintf("%s/%s", GetAbsolutePathToEngineRootDir(), SolutionLuaFile)
}

var ConfigDir string = "Config"
var BinariesDir string = "Binaries"
var IntermediateDir string = "Intermediate"
var ContentDir string = "Content"
var VslfDir string = "Vslf"
var ProgramsDir string = "Programs"
var GeneratedHeadersDir string = fmt.Sprintf("%s/gh", VslfDir)
var GeneratedHeadersExtension string = ".generated.h"
var VendorIncludeDir string = "Engine/Vendor/Include"
var VendorLibDir string = "Engine/Vendor/Lib"

func GetAbsolutePath(relPath string) (string /* absPath */, bool /* bExists */) {
    var absPath string = fmt.Sprintf("%s/%s", GetAbsolutePathToEngineRootDir(), relPath)

    if _, err := os.Stat(absPath); errors.Is(err, os.ErrNotExist) {
        return absPath, false
    } else if err != nil {
        panic(err)
    }

    return absPath, true
}

func GetCheckedAbsolutePath(relPath string) string {
    absPath, bExists := GetAbsolutePath(relPath)
    if !bExists {
        panic(fmt.Sprintf("Could not find the file or directory at path: %s.", absPath))
    }

    return absPath
}

func GetAbsolutePathNoCheck(relPath string) string {
    absPath, _ := GetAbsolutePath(relPath)
    return absPath
}

func GetAbsolutePathToEngineRootDir() string {
    wd, err := os.Getwd()
    if err != nil {
        panic(err)
    }

    if strings.Contains(wd, "\\") {
        wd = strings.ReplaceAll(wd, "\\", "/")
    }

    if !strings.Contains(wd, "/") {
        panic("Could not find the root engine directory")
    }

    wd = wd[:strings.LastIndex(wd, "/")]

    if _, err := os.Stat(wd + "/Jafg.jafgproj"); errors.Is(err, os.ErrNotExist) {
        panic("Could not find the root engine directory. Predicted path: " + wd)
    }

    return wd
}

func ReadAllEntriesFromAbsolutePath(absPath string) []fs.DirEntry {
    entries, err := os.ReadDir(absPath)
    if err != nil {
        panic(err)
    }

    return entries
}

func GetBinTargetRelativePath(system string, architecture string, buildConfig string, buildTarget string, subPath string) string {
    return fmt.Sprintf("%s/%s-%s/%s-%s/%s", BinariesDir, system, architecture, buildConfig, buildTarget, subPath)
}

func CopyRelativeDirectory(relativeSource string, relativeTarget string, bOverwrite bool, bRecursively bool) {
    var absoluteSource string = GetCheckedAbsolutePath(relativeSource)

    absoluteTarget, bExists := GetAbsolutePath(relativeTarget)
    if !bExists {
        CheckRelativeDir(relativeTarget)
        absoluteTarget = GetCheckedAbsolutePath(relativeTarget)
    }

    CopyAbsoluteDirectory(absoluteSource, absoluteTarget, bOverwrite, bRecursively)

    return
}

func CopyAbsoluteDirectory(absoluteSource string, absoluteTarget string, bOverwrite bool, bRecursively bool) {
    if _, err := os.Stat(absoluteSource); os.IsNotExist(err) {
        panic(fmt.Sprintf("Source directory %s does not exist", absoluteSource))
    }
    CheckAbsoluteDir(absoluteTarget)
    if _, err := os.Stat(absoluteTarget); os.IsExist(err) {
        panic("Enclosing block should never be reached.")
    }

    fmt.Println(fmt.Sprintf("Copying directory %s to %s ...", absoluteSource, absoluteTarget))

    entries, err := os.ReadDir(absoluteSource)
    if err != nil {
        panic(err)
    }

    for _, entry := range entries {
        sourceFile := fmt.Sprintf("%s/%s", absoluteSource, entry.Name())
        targetFile := fmt.Sprintf("%s/%s", absoluteTarget, entry.Name())

        fileInfo, err := os.Stat(sourceFile)
        if err != nil {
            panic(fmt.Sprintf("Error with file %s: [%s].", sourceFile, err))
        }

        if fileInfo.IsDir() {
            if bRecursively {
                CopyAbsoluteDirectory(sourceFile, targetFile, bOverwrite, bRecursively)
            }
            continue
        }

        if _, err := os.Stat(targetFile, ); os.IsExist(err) {
            if bOverwrite {
                err := os.Remove(targetFile)
                if err != nil {
                    panic(fmt.Sprintf("Error removing file %s: [%s].", targetFile, err))
                }
            } else {
                panic(fmt.Sprintf("File %s already exists in destination directory %s", sourceFile, targetFile))
            }
        }

        CopyAbsoluteFile(sourceFile, targetFile)

        fmt.Println(fmt.Sprintf("    Copied %s to %s.", sourceFile, targetFile))

        continue
    }

    return
}

func CopyRelativeDirectoryByExtension(relativeSource string, relativeTarget string, bOverwrite bool, exts []string, bRecursively bool) {
    var absoluteSource string = GetCheckedAbsolutePath(relativeSource)

    absoluteTarget, bExists := GetAbsolutePath(relativeTarget)
    if !bExists {
        CheckRelativeDir(relativeTarget)
        absoluteTarget = GetCheckedAbsolutePath(relativeTarget)
    }

    CopyAbsoluteDirectoryByExtension(absoluteSource, absoluteTarget, bOverwrite, exts, bRecursively)

    return
}

func CopyAbsoluteDirectoryByExtension(absoluteSource string, absoluteTarget string, bOverwrite bool, exts []string, bRecursively bool) {
    if _, err := os.Stat(absoluteSource); errors.Is(err, os.ErrNotExist) {
        panic(fmt.Sprintf("Could not find the source directory at path: %s.", absoluteSource))
    } else if err != nil {
        panic(err)
    }

    CheckAbsoluteDir(absoluteTarget)
    if _, err := os.Stat(absoluteTarget); errors.Is(err, os.ErrNotExist) {
        panic(fmt.Sprintf("Could not find the target directory at path: %s.", absoluteTarget))
    } else if err != nil {
        panic(err)
    }

    fmt.Printf("Conditionally copying directory [%s] to [%s] ...\n", absoluteSource, absoluteTarget)

    entries, err := os.ReadDir(absoluteSource)
    if err != nil {
        panic(err)
    }

    for _, entry := range entries {
        sourceFile := fmt.Sprintf("%s/%s", absoluteSource, entry.Name())
        targetFile := fmt.Sprintf("%s/%s", absoluteTarget, entry.Name())

        fileInfo, err := os.Stat(sourceFile)
        if err != nil {
            panic(fmt.Sprintf("Error with file [%s]: [%s].\n", sourceFile, err))
        }

        if fileInfo.IsDir() {
            if bRecursively {
                CopyAbsoluteDirectoryByExtension(sourceFile, targetFile, bOverwrite, exts, bRecursively)
            }

            continue
        }

        bCopy := false
        for _, ext := range exts {
            if strings.HasSuffix(fileInfo.Name(), ext) {
                bCopy = true
                break
            }

            continue
        }

        if !bCopy {
            continue
        }

        if _, err := os.Stat(targetFile, ); os.IsExist(err) {
            if bOverwrite {
                err := os.Remove(targetFile)
                if err != nil {
                    panic(fmt.Sprintf("Error removing file [%s]: [%s].\n", targetFile, err))
                }
            } else {
                panic(fmt.Sprintf("File [%s] already exists and overwrite is disabled.\n", targetFile))
            }
        }

        CopyAbsoluteFile(sourceFile, targetFile)

        fmt.Printf("    Copied [%s] to [%s].\n", sourceFile, targetFile)

        continue
    }

    return
}

// CopyAbsoluteFile copies a file from source to destination. Dest file must not exist.
func CopyAbsoluteFile(sourceFile string, targetFile string) {
    if _, err := os.Stat(sourceFile); os.IsNotExist(err) {
        panic(fmt.Sprintf("Source file %s does not exist", sourceFile))
    }

    if _, err := os.Stat(targetFile); os.IsExist(err) {
        panic(fmt.Sprintf("Destination file %s already exists", targetFile))
    }

    out, err := os.Create(targetFile)
    if err != nil {
        panic(err)
    }

    defer func(out *os.File) {
        err := out.Close()
        if err != nil {
            panic(err)
        }
    }(out)

    in, err := os.Open(sourceFile)
    if err != nil {
        panic(err)
    }

    defer func(in *os.File) {
        err := in.Close()
        if err != nil {
            panic(err)
        }
    }(in)

    _, err = io.Copy(out, in)
    if err != nil {
        panic(err)
    }

    return
}
