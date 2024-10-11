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

type Application struct {
    Projects []Project
}

var GApp *Application

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

func DetectAllProjects() {
    var rootDir string = GApp.GetEngineRootDir()

    entries, err := os.ReadDir(rootDir)
    if err != nil {
        panic(err)
    }

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

        proj.Initialize(entry.Name(), resolvedProjFile, string(file[:]))

        GApp.Projects = append(GApp.Projects, proj)

        continue
    }

    return
}
