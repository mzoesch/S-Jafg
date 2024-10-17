// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "encoding/json"
    "fmt"
    "strconv"
    "strings"
)

type Project struct {
    /** Internal name / identifier. */
    Name string

    /** What a user might see. */
    FriendlyName string

    /** The path to the .jafgproj file. */
    Path string

    /** Kind of project. */
    Kind ProjectKind
    /** How to respond to Pch usage. */
    Pch  Pch

    /** The modules of the project. The first entry is always the primary module. */
    Modules []Module
    /** The name of the primary module. */
    PrimaryModuleName string

    /** This project contains on these projects. */
    Projects []Project

    /** Possible parent project. */
    Parent *Project

    /**
     * Automatically generated. For preproc only.
     * @see Build.h and Build.generated.h
     */
    PreProcInteger int64
}

func (proj *Project) GetPreProcIntAsString() string {
    return strconv.FormatInt(proj.PreProcInteger, 10)
}

func (proj *Project) GetPreProcIntAsStringBase2() string {
    return strconv.FormatInt(proj.PreProcInteger, 2)
}

func (proj *Project) ToString() string {
    var CatModules string = ""
    for _, mod := range proj.Modules {
        if mod.IsPoly() {
            CatModules += mod.GetName() + ", "
        } else {
            CatModules += "PRIMARY"
        }
    }

    return "{" + proj.Name + ", " + proj.Kind.ToString() + ", {" + CatModules + "}}"
}

func (proj *Project) ToFancyString(indent int, bEndLine bool) string {
    var builder strings.Builder = strings.Builder{}
    builder.WriteString(strings.Repeat(" ", indent))
    builder.WriteString(fmt.Sprintf("| %s [proj] [%s] [%s] [%s]",
        proj.Name,
        proj.Kind.ToString(),
        proj.GetRelativeDirPath(),
        proj.GetPreProcIntAsString(),
    ))

    if (proj.Modules[0]).IsPoly() {
        for _, mod := range proj.Modules {
            builder.WriteString("\n")
            builder.WriteString(fmt.Sprintf("%s", mod.ToFancyString(indent + 2)))
        }
    }

    for _, subProj := range proj.Projects {
        builder.WriteString("\n")
        builder.WriteString(fmt.Sprintf("%s", subProj.ToFancyString(indent + 2, false)))
    }

    if bEndLine {
        builder.WriteString("\n")
    }

    return builder.String()
}

// GetAbsoluteDirPath The path to the project root dir.
func (proj *Project) GetAbsoluteDirPath() string {
    if strings.Contains(proj.Path, "\\") {
        proj.Path = strings.ReplaceAll(proj.Path, "\\", "/")
    }

    return proj.Path[:strings.LastIndex(proj.Path, "/")]
}

func (proj *Project) GetRelativeDirPath() string {
    var absPath string = proj.GetAbsoluteDirPath()
    var engineRootDir string = GApp.GetEngineRootDir()
    var relPath string = absPath[len(engineRootDir):]
    if strings.HasPrefix(relPath, "/") {
        relPath = relPath[1:]
    }

    return relPath
}

func (proj *Project) Initialize(preProcInteger int64, name string, pathToProjFile string, content string) {
    proj.PreProcInteger = preProcInteger

    proj.Name = name
    proj.Path = pathToProjFile

    var jsonInterface interface{}
    var err error = json.Unmarshal([]byte(content), &jsonInterface)
    if err != nil {
        panic(err)
    }

    var jsonMap map[string]interface{} = jsonInterface.(map[string]interface{})

    for k, v := range jsonMap {
        switch vv := v.(type) {
        case string:
            proj.InitializeWithSingleJsonFieldString(k, vv)
        case int:
            fmt.Println(k, "is int", vv)
        case []interface{}:
            proj.InitializeWithSingleJsonFiledArray(k, vv)
        default:
            panic("Unknown type in project JSON: " + k)
        }
    }

    if len(proj.PrimaryModuleName) == 0 {
        if len(proj.Modules) == 0 {
            proj.InitializeSoloModule()
        } else {
            panic("Did not found a primary module in project: " + proj.Name)
        }
    } else {
        for i, mod := range proj.Modules {
            if mod.GetName() == proj.PrimaryModuleName {
                mod.SetIsPrimary(true)
                proj.Modules[0], proj.Modules[i] = proj.Modules[i], proj.Modules[0]
                break
            }
        }
    }

    return
}

func (proj *Project) InitializeWithSingleJsonFieldString(key string, value string) {
    switch key {
    case "FriendlyName":
        proj.FriendlyName = value
    case "Kind":
        proj.Kind = ProjectKindFromString(value)
    case "Pch":
        proj.Pch = PchFromString(value)
    case "PrimaryModule":
        proj.PrimaryModuleName = value
    default:
        panic("Unknown key in project JSON: " + key)
    }

    return
}

func (proj *Project) InitializeWithSingleJsonFiledArray(key string, value []interface{}) {
    switch key {
    case "Modules":
        for _, mod := range value {
            var modName string = mod.(string)

            var modPtr *PolyModule = new(PolyModule)
            var thisModPreProcInt int64 = GApp.CurPreProcCursor
            GApp.CurPreProcCursor *= 2
            modPtr.Initialize(proj, modName, thisModPreProcInt)

            proj.Modules = append(proj.Modules, modPtr)
        }
    case "Projects":
        for _, project := range value {
            var projName string = project.(string)

            var subProjPtr *Project = new(Project)
            var thisProjPreProcCursor int64 = GApp.CurPreProcCursor
            GApp.CurPreProcCursor *= 2
            var subProjJafgProjFile string = fmt.Sprintf("%s/Projects/%s/%s.jafgproj", proj.GetAbsoluteDirPath(), projName, projName)
            subProjPtr.Parent = proj
            subProjPtr.Initialize(
                thisProjPreProcCursor,
                projName,
                subProjJafgProjFile,
                ReadFileContents(subProjJafgProjFile),
            )

            proj.Projects = append(proj.Projects, *subProjPtr)
        }
    default:
        panic("Unknown key in project JSON: " + key)
    }

    return
}

func (proj *Project) InitializeSoloModule() {
    var modPtr *SoloModule = new(SoloModule)
    // Solo models always share the same preproc int.
    modPtr.Initialize(proj, proj.PreProcInteger)

    proj.Modules = append(proj.Modules, modPtr)

    return
}

func (proj *Project) GetAllPreProcIntegers() []int64 {
    var out []int64 = []int64{proj.PreProcInteger}

    for _, mod := range proj.Modules {
        if mod.IsPoly() {
            out = append(out, mod.GetPreProcInt())
        }
    }

    for _, subProj := range proj.Projects {
        out = append(out, subProj.GetAllPreProcIntegers()...)
    }

    return out;
}

func (proj *Project) GetExclusiveParentProjectChain() []string {
    if proj.Parent != nil {
        return append([]string{}, proj.Parent.GetExclusiveParentProjectChain()...)
    }

    return []string{}
}

func (proj *Project) GetInclusiveParentProjectChain() []string {
    var out []string
    if proj.Parent != nil {
        out = append(out, proj.Parent.GetInclusiveParentProjectChain()...)
    }

    out = append(out, proj.Name)

    return out
}
