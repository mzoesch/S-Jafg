// Copyright 2024 mzoesch. All rights reserved.

package Info

import (
    "Jafg/Shared"
    "fmt"
)

type ProjectBuildInfo struct {
    ProjectName     string
    Kind            Shared.ProjectKind
    BuildConfig     Shared.BuildConfig
    Platform        string
    System          string
    Architecture    string
}

// GProjectBuildInfo The current project that is being built
var GProjectBuildInfo *ProjectBuildInfo = nil

func (info *ProjectBuildInfo) ToString() string {
    return fmt.Sprintf("{%s, %s, %s, %s, %s, %s}",
        info.ProjectName, info.Kind.ToString(),
        info.BuildConfig.ToString(), info.Platform,
        info.System,      info.Architecture,
    )
}

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

    {
        bFound, Value := Shared.GetArgValue(args, "--BUILD_CONFIG")
        if !bFound {
            panic("Could not find the build configuration")
        }
        info.BuildConfig = Shared.GetBuildConfigFromString(Value)
    }

    {
        bFound, Value := Shared.GetArgValue(args, "--PLATFORM")
        if !bFound {
            panic("Could not find the platform")
        }
        info.Platform = Value
    }

    {
        bFound, Value := Shared.GetArgValue(args, "--CFG_SYSTEM")
        if !bFound {
            panic("Could not find the system")
        }
        info.System = Value
    }

    {
        bFound, Value := Shared.GetArgValue(args, "--CFG_ARCHITECTURE")
        if !bFound {
            panic("Could not find the architecture")
        }
        info.Architecture = Value
    }

    return
}

func (info *ProjectBuildInfo) GetCurrentBinTargetRelative() string {
    return Shared.GetBinTargetRelativePath(info.System, info.Architecture, info.BuildConfig.ToString(), info.ProjectName)
}

func CheckIfCurrentProjectExists() bool {
    for _, proj := range Shared.GApp.Projects {
        if proj.Name == GProjectBuildInfo.ProjectName {
            return true
        }
    }

    panic("Could not find the project " + GProjectBuildInfo.ProjectName + " in the application")
}
