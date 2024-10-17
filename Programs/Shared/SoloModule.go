// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "fmt"
    "strings"
)

// SoloModule is a module of a project. It is special in that case
// that the parent only has one module and this module is saved under "~/Source".
type SoloModule struct {
    Module

    Parent *Project
    PreProcInt int64
}

func (m *SoloModule) Initialize(parent *Project, preProcInt int64) {
    m.Parent = parent
    m.PreProcInt = preProcInt
    return
}

func (m *SoloModule) GetName() string {
    return m.Parent.Name
}

func (m *SoloModule) GetFriendlyName() string {
    return m.Parent.FriendlyName
}

func (m *SoloModule) GetRelDirPath() string {
    return m.Parent.GetRelativeDirPath()
}

func (m *SoloModule) IsPrimary() bool {
    return true
}
func (m *SoloModule) SetIsPrimary(bPrimary bool) {
    panic("Called set primary on a solo module inside project: " + m.GetParent().Name)
}

func (m *SoloModule) GetParent() *Project {
    return m.Parent
}

func (m *SoloModule) IsPoly() bool {
    return false
}

func (m *SoloModule) ToFancyString(indent int) string {
    var builder strings.Builder = strings.Builder{}
    builder.WriteString(strings.Repeat(" ", indent))
    builder.WriteString(fmt.Sprintf("| %s [mod] [primary] [%d]", "Source", m.GetPreProcInt()))

    return builder.String()
}

func (m *SoloModule) GetPreProcInt() int64 {
    return m.PreProcInt
}

func (m *SoloModule) SetPreProcInt(i int64) {
    m.PreProcInt = i
}

func (m *SoloModule) GetPublicDependencies() []string {
    return m.Parent.PublicDependencies
}
