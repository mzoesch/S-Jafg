// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "fmt"
    "io/fs"
    "slices"
    "strings"
)

// Application is holds the structure for the main application.
// An application consists of projects, projects consist of at least one module and
// possibly more projects.
// Modules can reference other modules as dependencies.
// Public dependencies are transitive and public circular dependencies are forbidden.
type Application struct {
    Projects []Project
    Config   ApplicationConfig
    // Private global pre proc cursor; Use getter to get a new value
    PreProcCursor int64
}

// GApp is the single global instance of the Application struct.
var GApp *Application = nil

func (app *Application) Initialize() {
    fmt.Println("Initializing application.")

    if GApp != nil {
        panic("Application already initialized.")
    }

    GApp = app

    app.PreProcCursor = 1

    app.Config.Initialize()
    fmt.Printf("Application configuration: %s.\n", app.Config.ToString())

    app.DetectAllProjects()
    app.PrettyPrintApplicationStructure()

    return
}

func (app *Application) DetectAllProjects() {
    if len(app.Projects) > 0 {
        panic("Application already has projects.")
    }

    var absRootDir string = GetAbsolutePathToEngineRootDir()
    var entries []fs.DirEntry = ReadAllEntriesFromAbsolutePath(absRootDir)

    for _, entry := range entries {
        // A top level dir cannot be a project
        if entry.IsDir() == false {
            continue
        }
        app.DetectAllProjectsImpl(absRootDir, &entry, true)
    }

    app.ValidateAllDependencies()

    return
}

func (app *Application) DetectAllProjectsImpl(searchedDir string, entry *fs.DirEntry, bTopLevel bool) {
    var topLevelsToSkip []string = []string{
        ConfigDir, BinariesDir, IntermediateDir, ContentDir, VslfDir, ProgramsDir,
    }
    var alwaysSkip []string = []string{
        ".vs", ".git", ".vscode", ".idea", ".venv", "venv", "__pycache__",
    }

    if (*entry).IsDir() == false {
        panic(fmt.Sprintf("Entry [%s] is not a directory.", (*entry).Name()))
    }
    if bTopLevel {
        if slices.Contains(topLevelsToSkip, (*entry).Name()) {
            return
        }
    }
    if slices.Contains(alwaysSkip, (*entry).Name()) {
        return
    }

    var nextSearchedDir string = fmt.Sprintf("%s/%s", searchedDir, (*entry).Name())
    subEntries := ReadAllEntriesFromAbsolutePath(nextSearchedDir)

    if len(subEntries) == 0 {
        return
    }

    if ContainsByPredicate(subEntries, func(entry fs.DirEntry) bool {
        if entry.IsDir() {
            return false
        }

        return strings.HasSuffix(entry.Name(), ".jafgproj")
    }) {
        app.LoadProjectFromPath(nextSearchedDir, (*entry).Name())
        return
    }

    for _, subEntry := range subEntries {
        if subEntry.IsDir() == false {
            continue
        }

        app.DetectAllProjectsImpl(nextSearchedDir, &subEntry, false)
    }

    return
}

func (app *Application) LoadProjectFromPath(absoluteDirPath string, name string) {
    var newProject Project = Project{}

    newProject.Name = name
    newProject.AbsProjFilePath = fmt.Sprintf("%s/%s.jafgproj", absoluteDirPath, name)

    VerifyAbsoluteFileExistence(newProject.AbsProjFilePath)

    app.Projects = append(app.Projects, newProject)

    app.Projects[len(app.Projects)-1].LoadProject()

    return
}

func (app *Application) ValidateAllDependencies() {
    for _, proj := range app.Projects {
        proj.ValidateAllDependencies()
    }

    app.CheckForCircularDependencies()

    return
}

func (app *Application) CheckForCircularDependencies() {

    /* Layer 8 problem for now. Just don't do it. */

    return
}

func (app *Application) RequireModuleExists(name string) {
    if app.GetModuleByName(name) == nil {
        panic(fmt.Sprintf("Module [%s] does not exist. But was required.", name))
    }

    return
}

func (app *Application) GetModuleByName(name string) *Module {
    for _, proj := range app.Projects {
        for _, mod := range proj.Modules {
            if mod.GetUsableName() == name {
                return &mod
            }
        }
    }

    return nil
}

func (app *Application) GetCheckedModuleByName(name string) *Module {
    var out *Module = app.GetModuleByName(name)

    if out == nil {
        panic(fmt.Sprintf("Module [%s] does not exist.", name))
    }

    return out
}

func (app *Application) PrettyPrintApplicationStructure() {
    fmt.Println("| Application structure:")
    for _, proj := range app.Projects {
        proj.PrettyPrint(2)
    }

    return
}

func (app *Application) GetNewPreProcCursor() int64 {
    var Out int64 = app.PreProcCursor
    app.PreProcCursor *= 2
    return Out
}

func (app *Application) GetAllProjects() []Project {
    return app.Projects
}

func (app *Application) GetAllModules() []*Module {
    var Out []*Module

    for _, proj := range app.GetAllProjects() {
        for _, mod := range proj.Modules {
            Out = append(Out, &mod)
        }
    }

    return Out
}
