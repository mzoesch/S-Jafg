// Copyright 2024 mzoesch. All rights reserved.

package Info

import "Jafg/Shared"

type ProjectBuildInfo struct {
    ProjectName     string
    Kind            Shared.ProjectKind
}

// GProjectBuildInfo The current project that is being built
var GProjectBuildInfo *ProjectBuildInfo = nil

func (info *ProjectBuildInfo) Initialize(args []string) {
    {
        bFound, Value := Shared.GetArgValue(args, "--PROJ_NAME")
        if !bFound {
            panic("Could not find the project name")
        }
        info.ProjectName = Value
    }

    {
        bFound, Value := Shared.GetArgValueKind(args, "--CFG_KIND")
        if !bFound {
            panic("Could not find the project kind")
        }
        info.Kind = Value
    }

    return
}

func CheckIfCurrentProjectExists() bool {
    for _, proj := range Shared.GApp.Projects {
        if proj.Name == GProjectBuildInfo.ProjectName {
            return true
        }
    }

    panic("Could not find the project " + GProjectBuildInfo.ProjectName + " in the application")
}
