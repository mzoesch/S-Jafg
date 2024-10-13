// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/BuildTool/Info"
    "Jafg/Shared"
    "fmt"
)

func CopyContentFolder() {
    if !Shared.GApp.Config.DoCopyContent() {
        fmt.Println("Skipping content folder copy.")
        return
    }

    fmt.Println("Copying content folder ...")

    Shared.CopyDirectory(
        Shared.ContentDir,
        Info.GProjectBuildInfo.GetCurrentBinTargetRelative() + "/" + Shared.ContentDir,
        true,
    )

    fmt.Println("Content folder copied.")

    return
}

func CopySharedProjectBinaries() {
    if !Shared.GApp.Config.DoCopySharedProjectBinaries() {
        fmt.Println("Skipping shared project binaries copy for project", Info.GProjectBuildInfo.ProjectName + ".")
        return
    }

    fmt.Println("Copying shared project binaries ...")

    var ShippedExtension []string = []string{ ".dll" }
    var DevelopmentExtension []string = []string{ ".pdb" }

    for _, proj := range Shared.GApp.Projects {
        if !proj.Kind.IsLaunch() {
            continue
        }

        var sourceRel string = Shared.GetBinTargetRelativePath(Info.GProjectBuildInfo.System, Info.GProjectBuildInfo.Architecture,
            Info.GProjectBuildInfo.BuildConfig.ToString(), Info.GProjectBuildInfo.ProjectName)
        var targetRel string = Shared.GetBinTargetRelativePath(Info.GProjectBuildInfo.System, Info.GProjectBuildInfo.Architecture,
            Info.GProjectBuildInfo.BuildConfig.ToString(), proj.Name)

        if !Info.GProjectBuildInfo.BuildConfig.IsShipping() {
            Shared.CopyDirectoryByExts(sourceRel, targetRel, true, DevelopmentExtension, false)
        }
        Shared.CopyDirectoryByExts(sourceRel, targetRel, true, ShippedExtension, false)


        continue
    }

    fmt.Println("Shared project binaries copied for project", Info.GProjectBuildInfo.ProjectName + ".")

    return
}
