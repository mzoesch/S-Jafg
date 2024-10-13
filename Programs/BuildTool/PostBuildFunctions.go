// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/BuildTool/Info"
    "Jafg/Shared"
    "fmt"
)

func CopyContentFolder() {
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
    fmt.Println("Copying shared project binaries ...")

    var ShippedExtension []string = []string{ ".dll" }
    var DevelopmentExtension []string = []string{ ".pdb" }

    for _, proj := range Shared.GApp.Projects {
        if !proj.Kind.IsShared() {
            continue
        }

        var sourceRel string = Shared.GetBinTargetRelativePath(Info.GProjectBuildInfo.System, Info.GProjectBuildInfo.Architecture,
            Info.GProjectBuildInfo.BuildConfig.ToString(), proj.Name)
        var targetRel string = Info.GProjectBuildInfo.GetCurrentBinTargetRelative()

        if !Info.GProjectBuildInfo.BuildConfig.IsShipping() {
            Shared.CopyDirectoryByExts(sourceRel, targetRel, true, DevelopmentExtension, false)
        }
        Shared.CopyDirectoryByExts(sourceRel, targetRel, true, ShippedExtension, false)

        continue
    }

    fmt.Println("Shared project binaries copied.")

    return
}
