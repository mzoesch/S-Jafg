// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "fmt"
    "strconv"
)

// Module is a struct that holds the structure for a module in the application
// A module is always statically compiled and linked into the application.
//
// The following fields are optional:
//   "FriendlyName"         - A human-readable name for the module
//   "PublicDependencies"   - A list of module names that this module depends on
//   "Pch"                  - The precompiled header strategy for the module ("No", "Use", "Generate")
type Module struct {
    Name         string
    FriendlyName string
    Parent       *Project

    Pch Pch

    PublicDependencies []string

    PreProcInt int64
}

func (mod *Module) LoadModule() {
    if mod.Parent == nil {
        panic("Module parent is nil.")
    }

    mod.PreProcInt = GApp.GetNewPreProcCursor()

    var jsonMap map[string]interface{} = GetJsonMap(ReadFileContentsFromRelativePath(mod.GetRelativeModuleFilePath()))
    for k, v := range jsonMap {
        switch vv := v.(type) {
        case string:
            mod.DeserializeStringField(k, vv)
        case []interface{}:
            mod.DeserializeArrayField(k, vv)
        default:
            panic(fmt.Sprintf("Could not resolve type: %T.", vv))
        }

    }

    if len(mod.FriendlyName) == 0 {
        mod.FriendlyName = mod.Name
    }

    mod.ValidateJsonLoading()

    return
}

func (mod *Module) DeserializeStringField(key string, value string) {
    switch key {
    case "FriendlyName":
        mod.FriendlyName = value
    case "Pch":
        mod.Pch = PchFromString(value)
    default:
        panic(fmt.Sprintf("Unknown module field: %s.", key))
    }

    return
}

func (mod *Module) DeserializeArrayField(key string, value []interface{}) {
    switch key {
    case "PublicDependencies":
        for _, v := range value {
            mod.PublicDependencies = append(mod.PublicDependencies, v.(string))
        }
    default:
        panic(fmt.Sprintf("Unknown module field: %s.", key))
    }

    return
}

func (mod *Module) ValidateJsonLoading() {
    if len(mod.Name) == 0 {
        panic("Module name is empty.")
    }

    if len(mod.FriendlyName) == 0 {
        mod.FriendlyName = mod.Name
    }

    return
}

func (mod *Module) ValidateAllDependencies() {
    for _, dep := range mod.PublicDependencies {
        // Special dependencies that are not modules and are hardcoded into the lua generation.
        if dep == "CORE_DEPENDENCIES" {
            continue
        }

        GApp.RequireModuleExists(dep)
    }

    return
}

func (mod *Module) GetAllDependenciesTransitive() []string {
    var out []string

    for _, dep := range mod.PublicDependencies {
        out = append(out, dep)

        // Special dependencies that are not modules and are hardcoded into the lua generation.
        if dep == "CORE_DEPENDENCIES" {
            continue
        }

        out = append(out, GApp.GetCheckedModuleByName(dep).GetAllDependenciesTransitive()...)
    }

    return RemoveDuplicatesFromStringSlice(out)
}

func (mod *Module) IsSoloModuleArchitecture() bool {
    return mod.Name == "SOLO"
}

func (mod *Module) GetUsableName() string {
    if mod.IsSoloModuleArchitecture() {
        return mod.Parent.Name
    }

    return mod.Name
}

func (mod *Module) GetModuleFileName() string {
    if mod.IsSoloModuleArchitecture() {
        return "Source.jafgmod"
    }

    return fmt.Sprintf("%s.jafgmod", mod.Name)
}

func (mod *Module) GetRelativeModuleFilePath() string {
    if mod.IsSoloModuleArchitecture() {
        return fmt.Sprintf("%s/%s", mod.Parent.GetRelativeProjectDir(), mod.GetModuleFileName())
    }

    return fmt.Sprintf("%s/%s/%s", mod.Parent.GetRelativeProjectDir(), mod.Name, mod.GetModuleFileName())
}

func (mod *Module) GetRelativeModuleDir() string {
    var relModFile string = mod.GetRelativeModuleFilePath()
    return relModFile[:len(relModFile)-len(mod.GetModuleFileName())-1]
}

func (mod *Module) PrettyPrint(indent int) {
    var primSpec string = ""
    if mod.Parent.PrimaryModule == mod.Name {
        primSpec = " [Primary]"
    }

    fmt.Println(fmt.Sprintf(
        "%s| Module [%s] [%s] [%d]%s",
        Indent(indent), mod.Name, mod.GetRelativeModuleFilePath(), mod.PreProcInt, primSpec,
    ))

    return
}

func (mod *Module) GetKind() ProjectKind {
    if mod.IsSoloModuleArchitecture() {
        return mod.Parent.Kind
    }

    if mod.Parent.PrimaryModule == mod.Name {
        return mod.Parent.Kind
    }

    return PKIND_STATIC
}

func (mod *Module) GetPreProcIntAsString() string {
    return strconv.FormatInt(mod.PreProcInt, 10)
}

func (mod *Module) GetPreProcIntAsStringBase2() string {
    return strconv.FormatInt(mod.PreProcInt, 2)
}
