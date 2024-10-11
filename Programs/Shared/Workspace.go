// Copyright 2024 mzoesch. All rights reserved.

package Shared

type ProjectConfiguration int
const (
    DEBUG           ProjectConfiguration = iota
    DEVELOPMENT     ProjectConfiguration = iota
    RELEASE         ProjectConfiguration = iota
)

func (config ProjectConfiguration) ToString() string {
    switch config {
    case DEBUG:
        return "Debug"
    case DEVELOPMENT:
        return "Development"
    case RELEASE:
        return "Release"
    default:
        panic("Unknown project configuration")
    }
}
