// Copyright 2024 mzoesch. All rights reserved.

package Shared

import "fmt"

// Project is a struct that holds the structure for a project in the application
//
// The following fields are required:
//   "PrimaryModule"    - The name of the primary module
//   "Modules"          - A list of module names that this project consists of (SOLO if only
//                        one annotated with just a ~Source/)
//   "Kind"             - The kind of project ("Static", "Shared", "Launch")
// The following fields are optional:
//   "FriendlyName"     - A human-readable name for the project
type Project struct {
    Name            string
    FriendlyName    string
    AbsProjFilePath string

    DefaultKind ProjectKind

    IsSoloModuleArchitecture bool
    ModuleNames              []string
    Modules                  []Module

    PluginNames []string
}

func (proj *Project) LoadProject() {
    if len(proj.Name) == 0 {
        panic("Project name is empty.")
    }
    if len(proj.AbsProjFilePath) == 0 {
        panic("Project file path is empty.")
    }

    fmt.Printf("Loading project [%s] from [%s].\n", proj.Name, proj.GetRelativeProjectFilePath())

    var jsonMap map[string]interface{} = GetJsonMap(ReadFileContentsFromRelativePath(proj.GetRelativeProjectFilePath()))
    for k, v := range jsonMap {
        switch vv := v.(type) {
        case string:
            proj.DeserializeStringField(k, vv)
        case []interface{}:
            proj.DeserializeArrayField(k, vv)
        default:
            panic(fmt.Sprintf("Could not resolve type: %T.", vv))
        }
    }

    if len(proj.FriendlyName) == 0 {
        proj.FriendlyName = proj.Name
    }

    proj.ValidateJsonLoading()

    for _, module := range proj.ModuleNames {
        proj.Modules = append(proj.Modules, Module{})
        var mod *Module = &proj.Modules[len(proj.Modules)-1]
        mod.Name = module
        mod.Parent = proj

        mod.LoadModule()

        continue
    }

    return
}

func (proj *Project) DeserializeStringField(key string, value string) {
    switch key {
    case "FriendlyName":
        proj.FriendlyName = value
    case "DefaultKind":
        proj.DefaultKind = ProjectKindFromString(value)
    default:
        panic(fmt.Sprintf("Could not resolve key [%s] inside project [%s].", key, proj.Name))
    }

    return
}

func (proj *Project) DeserializeArrayField(key string, value []interface{}) {
    switch key {
    case "Modules":
        for _, v := range value {
            proj.ModuleNames = append(proj.ModuleNames, v.(string))

            if v.(string) == "SOLO" {
                proj.IsSoloModuleArchitecture = true

                if len(value) > 1 {
                    panic(fmt.Sprintf(
                        "SOLO module architectures cannot have more than one module. Faulty project: %s.",
                        proj.Name,
                    ))
                }
            }

            continue
        }
    case "Projects":
        for _, v := range value {
            proj.PluginNames = append(proj.PluginNames, v.(string))
        }
    default:
        panic(fmt.Sprintf("Could not resolve key: %s.", key))
    }

    return
}

func (proj *Project) ValidateJsonLoading() {
    if len(proj.Name) == 0 {
        panic("Project name is empty.")
    }

    if len(proj.FriendlyName) == 0 {
        panic(fmt.Sprintf("Project [%s] has no friendly name. A friendly name is required.", proj.Name))
    }

    if len(proj.ModuleNames) == 0 {
        panic(fmt.Sprintf("Project [%s] has no modules. At least one module is required.", proj.Name))
    }

    return
}

func (proj *Project) ValidateAllDependencies() {
    for _, mod := range proj.Modules {
        mod.ValidateAllDependencies()
    }

    return
}

func (proj *Project) GetProjectFileName() string {
    return fmt.Sprintf("%s.jafgproj", proj.Name)
}

func (proj *Project) GetRelativeProjectFilePath() string {
    return proj.AbsProjFilePath[len(GetAbsolutePathToEngineRootDir())+1:]
}

func (proj *Project) GetRelativeProjectDir() string {
    var relProjFile string = proj.GetRelativeProjectFilePath()
    return relProjFile[:len(relProjFile)-len(proj.GetProjectFileName())-1]
}

func (proj *Project) PrettyPrint(indent int) {
    fmt.Println(fmt.Sprintf("%s| Project [%s] [%s]:", Indent(indent), proj.Name, proj.DefaultKind.ToString()))
    for _, mod := range proj.Modules {
        mod.PrettyPrint(indent + 2)
    }

    return
}
