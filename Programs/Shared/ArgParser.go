// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "fmt"
    "strings"
)

func GetArgValue(args []string, arg string) (bool /* bFound */, string /* stringRepresentation */) {
    for _, a := range args {
        if strings.HasPrefix(fmt.Sprintf("%s=", a), arg) {
            return true, strings.TrimPrefix(a, arg+"=")
        }
    }

    return false, ""
}

func GetArgValueKind(args []string, arg string) (bool /* bFound */, ProjectKind /* stringRepresentation */) {
    if bFound, value := GetArgValue(args, arg); bFound {
        return true, ProjectKindFromString(value)
    }

    return false, ProjectKind(0)
}
