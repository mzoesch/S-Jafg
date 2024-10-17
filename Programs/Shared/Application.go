// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "errors"
    "fmt"
    "os"
    "strings"
)

type Application struct {
    Projects []Project
    Config AppConfiguration
    CurPreProcCursor int64
}

var GApp *Application = nil

func (app *Application) Initialize() {
    GApp = app
    DetectAllProjects()

    fmt.Println("| Projects:")
    for _, proj := range app.Projects {
        fmt.Print(proj.ToFancyString(2, true))
    }

    app.Config = AppConfiguration{}
    app.Config.Initialize()

    fmt.Println(app.Config.ToString())

    var preProcInts map[int64]bool = make(map[int64]bool)
    for _, proj := range app.Projects {
        procs := proj.GetAllPreProcIntegers()

        for _, proc := range procs {
            if preProcInts[proc] {
                panic(fmt.Sprintf("PreProcInt collision: %d. Faulty project: %s", proc, proj.Name))
            }

            preProcInts[proc] = true
        }

        continue
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

    GApp.CurPreProcCursor = 1
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

        var thisProjPreProcCursor int64 = GApp.CurPreProcCursor
        GApp.CurPreProcCursor *= 2
        proj.Initialize(thisProjPreProcCursor, entry.Name(), resolvedProjFile, string(file[:]))

        GApp.Projects = append(GApp.Projects, proj)

        continue
    }

    CheckForCircularDependencies()

    return
}

func CheckForCircularDependencies() {
    for _, proj := range GApp.Projects {
        CheckForCircularDependenciesImpl(&proj, make(map[string]bool))
    }

    return
}

func CheckForCircularDependenciesImpl(proj *Project, visited map[string]bool) {
    if visited[proj.Name] {
        panic("Circular dependency detected: " + proj.Name)
    }

    visited[proj.Name] = true

    for _, subProj := range proj.Projects {
        CheckForCircularDependenciesImpl(&subProj, visited)
    }

    return
}
