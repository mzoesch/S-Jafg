// Copyright 2024 mzoesch. All rights reserved.

package Shared

import "fmt"

type Pch int
const (
    PCH_NO Pch = iota
    PCH_USE Pch = iota
    PCH_GENERATE Pch = iota
)

func (pch Pch) ToString() string {
    switch pch {
    case PCH_NO:
        return "No"
    case PCH_USE:
        return "Use"
    case PCH_GENERATE:
        return "Generate"
    default:
        panic("Unknown Pch")
    }
}

func (pch Pch) IsNo() bool {
    return pch == PCH_NO
}

func (pch Pch) IsUse() bool {
    return pch == PCH_USE
}

func (pch Pch) IsGenerate() bool {
    return pch == PCH_GENERATE
}

func IsValidPch(kind string) bool {
    switch kind {
    case "No":
        return true
    case "Use":
        return true
    case "Generate":
        return true
    default:
        return false
    }
}

func PchFromString(kind string) Pch {
    switch kind {
    case "No":
        return PCH_NO
    case "Use":
        return PCH_USE
    case "Generate":
        return PCH_GENERATE
    default:
        panic(fmt.Sprintf("Unknown Pch: Found %s.", kind))
    }
}
