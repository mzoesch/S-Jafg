// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "errors"
    "fmt"
    "os"
    "strings"
)

var SolutionLuaFile string = "__buildSolution.lua"

func FullSolutionLuaPath() string {
    return GApp.GetEngineRootDir() + "/" + SolutionLuaFile
}

var GeneratedHeaderExtension string = ".generated.h"

var GeneratedHeadersDir string = "Vslf/gh"
var VendorIncludeDir string = "Engine/Vendor/Include"
var VendorLibDir string = "Engine/Vendor/Lib"
var ContentDir string = "Content"
var BinariesDir string = "Binaries"
var IntermediateDir string = "Intermediate"

func FullGeneratedHeadersDir() string {
    return GApp.GetEngineRootDir() + "/" + GeneratedHeadersDir
}

type Application struct {
    Projects []Project
}

var GApp *Application = nil

func (app *Application) Initialize() {
    GApp = app
    DetectAllProjects()

    fmt.Println("Projects detected:")
    for _, proj := range app.Projects {
        fmt.Println("  - " + proj.ToString())
    }

    return
}

func (app *Application) GetEngineRootDir() string {
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

func (app *Application) GetSolutionName() string {
    return "Jafg"
}

func (app *Application) GetProjectByName(name string) *Project {
    for _, proj := range app.Projects {
        if proj.Name == name {
            return &proj
        }
    }

    return nil
}

func (app *Application) GetCheckedProjectByName(name string) *Project {
    var proj *Project = app.GetProjectByName(name)
    if proj == nil {
        panic("Project not found: " + name)
    }

    return proj
}

func DetectAllProjects() {
    var rootDir string = GApp.GetEngineRootDir()

    entries, err := os.ReadDir(rootDir)
    if err != nil {
        panic(err)
    }

    var curProjPreProcInteger int64 = 1
    for _, entry := range entries {
        if !entry.IsDir() {
            continue
        }

        if len(entry.Name()) < 1 || entry.Name()[0] == '.' {
            continue
        }

        /* Dir may be valid for a project. Checking if it contains a *.jafgproj file. */
        subEntries, err := os.ReadDir(rootDir + "/" + entry.Name())
        if err != nil {
            panic(err)
        }

        var bValid bool = false
        for _, subEntry := range subEntries {
            if subEntry.IsDir() {
                continue
            }

            if strings.HasSuffix(subEntry.Name(), ".jafgproj") {
                if bValid {
                    panic("Multiple projects detected in the same directory")
                }

                if GetRuneCountInString(subEntry.Name(), '.') != 1 {
                    panic("Invalid project file name (to few or to many dots)")
                }

                var predictedName string = entry.Name() + ".jafgproj"
                if subEntry.Name() != predictedName {
                    panic("Invalid project file name (does not match the directory name): " + subEntry.Name() + " != " + predictedName)
                }

                bValid = true
            }

            continue
        }

        if !bValid {
            continue
        }

        var proj Project = Project{}

        var resolvedProjFile string = rootDir + "/" + entry.Name() + "/" + entry.Name() + ".jafgproj"

        file, err := os.ReadFile(resolvedProjFile)
        if err != nil {
            panic(err)
        }

        proj.Initialize(curProjPreProcInteger, entry.Name(), resolvedProjFile, string(file[:]))
        curProjPreProcInteger *= 2

        GApp.Projects = append(GApp.Projects, proj)

        continue
    }

    return
}
