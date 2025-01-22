// Copyright mzoesch. All rights reserved.

package main

import (
    "Jafg/Core"
    "Jafg/Shared"
    "errors"
    "fmt"
    "log"
    "os"
    "strings"
)

//
// Routes to subprogram
//
// Argument:
//   - SolutionGenerator [Generates solution] @see SolutionGenerator/Launch.go
//   - BuildTool         [Helps the solution to be compiled and linked] @see BuildTool/Launch.go
//   - EmulationUtility  [Emulates solution build commands without actually building] @see EmulationUtility/Launch.go
//   - DoNothing         [Does nothing (for development purposes)]
//
func main() {
    lastwd, err := os.Getwd()
    if err != nil {
        panic(err)
    }

    wd, err := os.Executable()
    if err != nil {
        panic(err)
    }
    if strings.Contains(wd, "\\") {
        wd = strings.ReplaceAll(wd, "\\", "/")
    }
    if !strings.Contains(wd, "/") {
        panic("Could not determine working directory.")
    }
    wd = wd[:strings.LastIndex(wd, "/")]
    wd = wd[:strings.LastIndex(wd, "/")]

    if _, err := os.Stat(wd + "/jafg.jafgworkspace"); errors.Is(err, os.ErrNotExist) {
        innerWd, err := os.Getwd() // ONLY WHEN LAUNCHING FROM AN IDEA
        if err != nil {
            panic(err)
        }
        if strings.Contains(innerWd, "\\") {
            innerWd = strings.ReplaceAll(innerWd, "\\", "/")
        }
        if !strings.Contains(innerWd, "/") {
            panic("Could not determine working directory.")
        }
        innerWd = innerWd[:strings.LastIndex(innerWd, "/")]
        if _, err := os.Stat(innerWd + "/jafg.jafgworkspace"); errors.Is(err, os.ErrNotExist) {
            panic(fmt.Sprintf("Could not find the engine root directory. Predicted paths [%s] || [%s]", wd, innerWd))
        }
        wd = innerWd
    }

    Shared.SetAbsoluteEngineRoot(wd)
    err = os.Chdir(Shared.GetAbsoluteEngineRoot())
    if err != nil {
        panic(err)
    }

    LoadWorkspace()

    var args []string = os.Args
    var errorLevel error = RouteToSubProgram(args)

    err = os.Chdir(lastwd)
    if err != nil {
        panic(err)
    }

    if errorLevel != nil {
        log.Fatalf("Application failed with error: [%s].", errorLevel.Error())
    }

    os.Exit(0)

    return
}

func LoadWorkspace() {
    fmt.Println("Loading workspace ...")
    if len(Core.GApp.Solutions) != 0 {
        log.Panicf("Workspace already loaded %d.\n", len(Core.GApp.Solutions))
    }

    Shared.VerifyRelativeFileExistence(Core.FilePath_ModuleCache)
    workspaceStructure := Shared.GetJsonMap(Shared.ReadRelativeFile(Core.FilePath_ModuleCache))

    solutions := workspaceStructure["Solutions"].([]interface{})
    for _, solution := range solutions {
        LoadSolution(solution.(map[string]interface{}))
    }

    return
}

func LoadSolution(data map[string]interface{}) {
    relativePath := Shared.GetJsonString(data, "RelativePyPath")
    name := Shared.GetJsonString(data, "Name")

    fmt.Printf("Loading solution [%s] in [%s].\n", name, relativePath)

    Core.GApp.Solutions = append(Core.GApp.Solutions, Core.Solution{})
    var sln *Core.Solution = &Core.GApp.Solutions[len(Core.GApp.Solutions)-1]
    sln.Name = name
    sln.RelativeDir = relativePath
    sln.Startup = Shared.GetJsonString(data, "Startup")
    var slnTargets []interface{} = Shared.GetJsonArray(data, "Targets")
    for _, target := range slnTargets {
        LoadTarget(sln, target.(map[string]interface{}))
    }

    return
}

func LoadTarget(sln *Core.Solution, data map[string]interface{}) {
    target := Core.Target{}
    target.Name = Shared.GetJsonString(data, "Name")
    target.Runtime = Shared.GetJsonString(data, "Runtime")
    target.Symbols = Shared.GetJsonBool(data, "Symbols")
    target.Optimize = Shared.GetJsonBool(data, "Optimize")
    target.Defines = Shared.GetJsonStringArray(data, "Defines")

    var targetModules []interface{} = Shared.GetJsonArray(data, "Modules")
    for _, module := range targetModules {
        LoadModule(&target, module.(map[string]interface{}))
    }

    sln.Targets = append(sln.Targets, target)

    return
}

func LoadModule(target *Core.Target, data map[string]interface{}) {
    var pubDeps []string = Shared.GetJsonStringArray(data, "PublicDependencies")
    var prvDeps []string = Shared.GetJsonStringArray(data, "PrivateDependencies")
    var natIncs []string = Shared.GetJsonStringArray(data, "PrivateNativeIncludeDirs")
    var natDeps []string = Shared.GetJsonStringArray(data, "PrivateNativeDependencies")
    var addiFls []string = Shared.GetJsonStringArray(data, "PrivateAdditionalCopiedFiles")

    module := Core.Module{}
    module.Name = Shared.GetJsonString(data, "Name")
    module.RelativeDir = Shared.GetJsonString(data, "RelativeDir")
    module.PchUsage = Core.PchUsageFromString(Shared.GetJsonString(data, "PchUsage"))
    module.PchContent = Shared.GetJsonString(data, "PchContent")
    module.Kind = Core.ModuleKindFromString(Shared.GetJsonString(data, "Kind"))
    for _, dep := range pubDeps {
        module.PublicDependencies = append(module.PublicDependencies, Core.DependencyFromString(dep))
    }
    for _, dep := range prvDeps {
        module.PrivateDependencies = append(module.PrivateDependencies, Core.DependencyFromString(dep))
    }
    for _, inc := range natIncs {
        module.NativeIncludeDirs = append(module.NativeIncludeDirs, inc)
    }
    for _, dep := range natDeps {
        module.NativeDependencies = append(module.NativeDependencies, dep)
    }
    for _, fl := range addiFls {
        module.AdditionalCopyFiles = append(module.AdditionalCopyFiles, fl)
    }

    target.Modules = append(target.Modules, module)

    return
}
