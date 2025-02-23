// Copyright mzoesch. All rights reserved.

package Core

import (
    "Jafg/Shared"
    "fmt"
    "strings"
)

type Solution struct {
    Name        string
    RelativeDir string
    Startup     string
    Targets     []Target
}

func (sln *Solution) GetFunctionalRelativeDir() string {
    if len(sln.RelativeDir) == 0 {
        panic("RelativeDir is empty")
    }
    if sln.RelativeDir[0] != '~' {
        panic("RelativeDir must start with ~")
    }

    return sln.RelativeDir[1:]
}

func (sln *Solution) GetSavedRelativeDir() string {
    return fmt.Sprintf("%s/%s/SLN_%s", DirPath_Saved, sln.GetFunctionalRelativeDir(), sln.Name)
}

func (sln *Solution) GetSavedRelativeDir_Premake() string {
    return fmt.Sprintf("%s/%s/P_SLN_%s", DirPath_Saved, sln.GetFunctionalRelativeDir(), sln.Name)
}

func (sln *Solution) GetSavedRelativeDir_CMake() string {
    return fmt.Sprintf("%s/%s/C_SLN_%s", DirPath_Saved, sln.GetFunctionalRelativeDir(), sln.Name)
}

func (sln *Solution) GetChdirUpRelToBuildFile() string {
    return "../.." + strings.Repeat("/..", Shared.CountRuneInString(sln.RelativeDir, '/')+1)
}
