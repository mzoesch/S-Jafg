// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "fmt"
    "slices"
    "strconv"
)

// Module is a struct that holds the structure for a module in the application.
//
// The following fields are optional:
//   "FriendlyName"                  - A human-readable name for the module
//   "PublicDependencies"            - A list of module names that this module depends on
//   "Pch"                           - The precompiled header strategy for the module ("No", "Use", "Generate")
//   "ConditionalPublicDependencies" - A list of conditional dependencies (condition based of target defines).
type Module struct {
    Name         string
    FriendlyName string
    Parent       *Project

    Pch  Pch
    Kind ProjectKind

    PublicDependencies            []string
    ConditionalPublicDependencies []ConditionalInclude

    TargetNames []string
    Targets     []Target

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
        case map[string]interface{}:
            mod.DeserializeObjectField(k, vv)
        default:
            panic(fmt.Sprintf("Could not resolve type: %T.", vv))
        }

        continue
    }

    if len(mod.FriendlyName) == 0 {
        mod.FriendlyName = mod.Name
    }

    if len(mod.TargetNames) != 0 {
        mod.LoadTargets()
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
    case "Kind":
        mod.Kind = ProjectKindFromString(value)
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
    case "Targets":
        for _, v := range value {
            var targetObj map[string]interface{} = v.(map[string]interface{})
            for kObj, vObj := range targetObj {
                switch vvObj := vObj.(type) {
                case string:
                    if kObj != "Suffix" {
                        continue
                    }
                    if len(vvObj) == 0 {
                        panic(fmt.Sprintf("Target name is empty in module [%s].", mod.GetUsableName()))
                    }
                    mod.TargetNames = append(mod.TargetNames, vvObj)
                default:
                    /* When we actually load the target, we loop over everything. */
                    continue
                }

                continue
            }

            continue
        }
    default:
        panic(fmt.Sprintf("Unknown module field: %s.", key))
    }

    return
}

func (mod *Module) DeserializeObjectField(key string, value map[string]interface{}) {
    switch key {
    case "ConditionalPublicDependencies":
        for k, v := range value {
            mod.ConditionalPublicDependencies = append(mod.ConditionalPublicDependencies, ConditionalInclude{})
            var condInc *ConditionalInclude = &mod.ConditionalPublicDependencies[len(mod.ConditionalPublicDependencies)-1]

            condInc.Condition = k
            for _, vv := range v.([]interface{}) {
                condInc.Include = vv.(string)
            }

            continue
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

func (mod *Module) LoadTargets() {
    if len(mod.TargetNames) == 0 {
        panic(fmt.Sprintf("Module [%s] has no targets defined.", mod.GetUsableName()))
    }

    for _, targetName := range mod.TargetNames {
        mod.Targets = append(mod.Targets, Target{})
        var targ *Target = &mod.Targets[len(mod.Targets)-1]

        targ.Parent = mod
        targ.Suffix = targetName

        targ.LoadTarget()

        continue
    }

    return
}

func (mod *Module) GetAllDependenciesTransitive(targ *Target) []string {
    var out []string
    mod.GetAllDependenciesTransitiveImpl(targ, &out)
    return out
}

func (mod *Module) GetAllDependenciesTransitiveImpl(targ *Target, inOutSlice *[]string) {
    for _, dep := range mod.PublicDependencies {
        if slices.Contains(*inOutSlice, dep) {
            continue
        }

        *inOutSlice = append(*inOutSlice, dep)

        // Special dependencies that are not modules and are hardcoded into the lua generation.
        if dep == "CORE_DEPENDENCIES" {
            continue
        }

        GApp.GetCheckedModuleByName(dep).GetAllDependenciesTransitiveImpl(targ, inOutSlice)
    }

    for _, condInc := range mod.ConditionalPublicDependencies {
        if condInc.IsConditionTrue(targ) {
            if slices.Contains(*inOutSlice, condInc.Include) {
                continue
            }

            *inOutSlice = append(*inOutSlice, condInc.Include)

            // Special dependencies that are not modules and are hardcoded into the lua generation.
            if condInc.Include == "CORE_DEPENDENCIES" {
                continue
            }

            GApp.GetCheckedModuleByName(condInc.Include).GetAllDependenciesTransitiveImpl(targ, inOutSlice)
        }

        continue
    }

    return
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
    fmt.Println(fmt.Sprintf(
        "%s| Module [%s] [%s] [%s] [%d] ",
        Indent(indent), mod.Name, mod.Kind.ToString(), mod.GetRelativeModuleFilePath(), mod.PreProcInt,
    ))

    for _, targ := range mod.Targets {
        targ.PrettyPrint(indent + 2)
    }

    return
}

func (mod *Module) GetKind() ProjectKind {
    if mod.Kind.IsInherit() {
        return mod.Parent.DefaultKind
    }

    return mod.Kind
}

func (mod *Module) GetPreProcIntAsString() string {
    return strconv.FormatInt(mod.PreProcInt, 10)
}

func (mod *Module) GetPreProcIntAsStringBase2() string {
    return strconv.FormatInt(mod.PreProcInt, 2)
}

func (mod *Module) GetPredictedTestIncludeFileName() string {
    return fmt.Sprintf("Test%s.h", mod.GetUsableName())
}

func (mod *Module) GetPredictedRelativeTestIncludeFilePath() string {
    return fmt.Sprintf("%s/Source/Test/%s", mod.GetRelativeModuleDir(), mod.GetPredictedTestIncludeFileName())
}

func (mod *Module) HasValidTestIncludeFile() bool {
    var relativeFileInQuestion string = mod.GetPredictedRelativeTestIncludeFilePath()
    return DoesRelativeFileExist(relativeFileInQuestion)
}
