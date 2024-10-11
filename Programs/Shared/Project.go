// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "encoding/json"
    "fmt"
    "strings"
)

type Project struct {
    Name         string
    FriendlyName string

    /** The path to the .jafgproj file. */
    Path string

    Kind ProjectKind
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

func (proj *Project) Initialize(name string, pathToProjFile string, content string) {
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
            fmt.Println(k, "is of a type I don't know how to handle")
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
    default:
        panic("Unknown key in project JSON: " + key)
    }

    return
}
