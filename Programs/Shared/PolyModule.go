// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "fmt"
    "os"
    "strings"
)

type PolyModule struct {
    Module

    Name string
    FriendlyName string

    /** The path to the .jafgmod file. */
    Path string

    bIsPrimary bool

    Parent *Project

    PublicDependencies []string

    PreProcInt int64
}

func (m *PolyModule) Initialize(parent *Project, name string, preProcInt int64) {
    m.Parent = parent

    m.Name = name
    m.FriendlyName = name
    m.PreProcInt = preProcInt

    m.Path = parent.GetRelativeDirPath() + "/" + name + "/" + name + ".jafgmod"

    var file *os.File = OpenFile(m.Path, false, os.O_RDWR)
    var jsonMap map[string]interface{} = GetJsonMap(GetFileContents(file))

    for k, v := range jsonMap {
        switch vv:= v.(type) {
        case string:
            m.InitializeWithSingleJsonFieldString(k, vv)
        case []interface{}:
            m.InitializeWithSingleJsonFiledArray(k, vv)
        default:
            panic("Unknown type in .jafgmod file: " + k)
        }
    }

    if len(m.FriendlyName) == 0 {
        m.FriendlyName = m.Name
    }

    return
}

func (m *PolyModule) InitializeWithSingleJsonFieldString(key string, value string) {
    switch key {
    case "FriendlyName":
        m.FriendlyName = value
    default:
        panic("Unknown key in .jafgmod file: " + key)
    }

    return
}

func (m *PolyModule) InitializeWithSingleJsonFiledArray(key string, value []interface{}) {
    switch key {
    case "PublicDependencies":
        for _, dep := range value {
            var depName string = dep.(string)

            if depName == "CORE_DEPENDENCIES" {
                m.PublicDependencies = append(m.PublicDependencies, "CORE_DEPENDENCIES")
                continue
            }

            // We do this to verify that the dependency exists.
            var _ *Project = GApp.GetCheckedProjectByName(depName)

            m.PublicDependencies = append(m.PublicDependencies, depName)

            continue
        }

    default:
        panic("Unknown key in .jafgmod file: " + key)
    }

    return
}

func (m *PolyModule) GetName() string {
    return m.Name
}

func (m *PolyModule) GetFriendlyName() string {
    return m.FriendlyName
}

func (m *PolyModule) GetRelDirPath() string {
    return m.Path
}

func (m *PolyModule) IsPrimary() bool {
    return m.bIsPrimary
}

func (m *PolyModule) SetIsPrimary(bPrimary bool) {
    m.bIsPrimary = bPrimary
}

func (m *PolyModule) GetParent() *Project {
    return m.Parent
}

func (m *PolyModule) IsPoly() bool {
    return true
}

func (m *PolyModule) ToFancyString(indent int) string {
    var builder strings.Builder = strings.Builder{}
    var primaryRepresentation string = ""
    if m.bIsPrimary {
        primaryRepresentation = " [primary]"
    }
    builder.WriteString(strings.Repeat(" ", indent))
    builder.WriteString(fmt.Sprintf("| %s [mod]%s [%d]", m.Name, primaryRepresentation, m.GetPreProcInt()))

    return builder.String()
}

func (m *PolyModule) GetPreProcInt() int64 {
    return m.PreProcInt
}

func (m *PolyModule) SetPreProcInt(i int64) {
    m.PreProcInt = i
}

func (m *PolyModule) GetPublicDependencies() []string {
    return m.PublicDependencies
}
