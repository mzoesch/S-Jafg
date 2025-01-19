// Copyright mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Core"
    "fmt"
    "strings"
)

type BuildTargetInfo struct {
    Sln      string
    Module   string
    Arch     string
    Target   string
    Platform string
    Kind     string
}

// GBuildTargetInfo is the global BuildTargetInfo instance. Might be nil, so do not use without checking.
var GBuildTargetInfo *BuildTargetInfo = nil

func (bti *BuildTargetInfo) Initialize(args []string) {
    for _, arg := range args {
        if strings.Contains(arg, "SLN=") {
            bti.Sln = strings.Split(arg, "=")[1]
        } else if strings.Contains(arg, "MODULE=") {
            bti.Module = strings.Split(arg, "=")[1]
        } else if strings.Contains(arg, "ARCH=") {
            bti.Arch = strings.Split(arg, "=")[1]
        } else if strings.Contains(arg, "TARGET=") {
            bti.Target = strings.Split(arg, "=")[1]
        } else if strings.Contains(arg, "PLATFORM=") {
            bti.Platform = strings.Split(arg, "=")[1]
        } else if strings.Contains(arg, "KIND=") {
            bti.Kind = strings.Split(arg, "=")[1]
        }
        continue
    }

    if bti.Sln == "" ||
        bti.Module == "" ||
        bti.Arch == "" ||
        bti.Target == "" ||
        bti.Platform == "" ||
        bti.Kind == "" {
        panic("Could not find all necessary build target information.")
    }

    return
}

func (bti *BuildTargetInfo) PrettyPrint() {
    fmt.Println("Build target information:")
    fmt.Println(fmt.Sprintf("SLN:      %s", bti.Sln))
    fmt.Println(fmt.Sprintf("MODULE:   %s", bti.Module))
    fmt.Println(fmt.Sprintf("PLATFORM: %s", bti.Platform))
    fmt.Println(fmt.Sprintf("ARCH:     %s", bti.Arch))
    fmt.Println(fmt.Sprintf("TARGET:   %s", bti.Target))

    return
}

func (bti *BuildTargetInfo) GetSlnPointer() *Core.Solution {
    for idx, _ := range Core.GApp.Solutions {
        if Core.GApp.Solutions[idx].Name == bti.Sln {
            return &Core.GApp.Solutions[idx]
        }
    }

    return nil
}
func (bti *BuildTargetInfo) GetSlnPointerChecked() *Core.Solution {
    var out *Core.Solution = bti.GetSlnPointer()
    if out == nil {
        panic("Could not find the solution.")
    }
    return out
}

func (bti *BuildTargetInfo) GetTargetPointer() *Core.Target {
    var sln *Core.Solution = bti.GetSlnPointer()
    if sln == nil {
        return nil
    }
    for idx, _ := range sln.Targets {
        if sln.Targets[idx].Name == bti.Target {
            return &sln.Targets[idx]
        }
    }
    return nil
}
func (bti *BuildTargetInfo) GetTargetPointerChecked() *Core.Target {
    var out *Core.Target = bti.GetTargetPointer()
    if out == nil {
        panic("Could not find the target.")
    }
    return out
}

func (bti *BuildTargetInfo) GetModulePointer() *Core.Module {
    var tar *Core.Target = bti.GetTargetPointer()
    if tar == nil {
        return nil
    }
    for idx, _ := range tar.Modules {
        if tar.Modules[idx].RelativeDir == bti.Module {
            return &tar.Modules[idx]
        }
    }
    return nil
}
func (bti *BuildTargetInfo) GetModulePointerChecked() *Core.Module {
    var out *Core.Module = bti.GetModulePointer()
    if out == nil {
        panic("Could not find the module.")
    }
    return out
}

func (bti *BuildTargetInfo) GetRelativeSavedModuleDir() string {
    return fmt.Sprintf("%s/%s",
        bti.GetSlnPointerChecked().GetSavedRelativeDir(),
        bti.GetModulePointerChecked().GetFunctionalRelativeDir())
}
func (bti *BuildTargetInfo) GetRelativeModuleGhDir() string {
    return fmt.Sprintf("%s/%s",
        bti.GetRelativeSavedModuleDir(),
        Core.GhDir,
    )
}
func (bti *BuildTargetInfo) GetRelativeModuleGtDir() string {
    return fmt.Sprintf("%s/%s",
        bti.GetRelativeSavedModuleDir(),
        Core.GtDir,
    )
}
func (bti *BuildTargetInfo) GetRelativeSourceDir() string {
    if strings.HasPrefix(bti.Module, "~") == false {
        panic(fmt.Sprintf("Module [%s] has not valid relative path.", bti.Module))
    }

    return bti.Module[1:] + "/Source"
}

func (bti *BuildTargetInfo) GetRelativeBinaryDirNoModules() string {
    return fmt.Sprintf("%s/%s-%s/%s",
        Core.DirPath_Bin,
        bti.Platform, bti.Arch, bti.Target,
    )
}

func (bti *BuildTargetInfo) GetRelativeBinaryDir() string {
    return fmt.Sprintf("%s/%s",
        bti.GetRelativeBinaryDirNoModules(),
        bti.GetModulePointerChecked().GetFunctionalRelativeDir(),
    )
}

func (bti *BuildTargetInfo) GetTranslatedKind() Core.ModuleKind {
    return Core.ModuleKindFromString(bti.Kind)
}

func (bti *BuildTargetInfo) GetSharedLibExtension() string {
    if bti.Platform == "Windows64" {
        return ".dll"
    }
    panic(fmt.Sprintf("Platform [%s] is missing implementation.", bti.Platform))
}

func (bti *BuildTargetInfo) GetSharedLibDebugSymbolsExtension() string {
    if bti.Platform == "Windows64" {
        return ".pdb"
    }
    panic(fmt.Sprintf("Platform [%s] is missing implementation.", bti.Platform))
}
