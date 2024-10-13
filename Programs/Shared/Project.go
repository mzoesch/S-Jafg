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
    Name         string

    /** What a user might see. */
    FriendlyName string

    /** The path to the .jafgproj file. */
    Path string

    /** Kind of project. */
    Kind ProjectKind
    /** How to respond to Pch usage. */
    Pch  Pch

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
    return "{" + proj.Name + ", " + proj.Kind.ToString() + "}"
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
            proj.InitializeWithSingleJsonField(k, vv)
        case int:
            fmt.Println(k, "is int", vv)
        case []interface{}:
            fmt.Println(k, "is an array:")
            for i, u := range vv {
                fmt.Println(i, u)
            }
        default:
            fmt.Println(k, "is of a type I don't know how to handle.")
        }
    }

    return
}

func (proj *Project) InitializeWithSingleJsonField(key string, value string) {
    switch key {
    case "FriendlyName":
        proj.FriendlyName = value
    case "Kind":
        proj.Kind = ProjectKindFromString(value)
    case "Pch":
        proj.Pch = PchFromString(value)
    default:
        panic("Unknown key in project JSON: " + key)
    }

    return
}
