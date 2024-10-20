// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "fmt"
    "slices"
)

func Launch(args []string) {
    var myInfo *BuildInfo = new(BuildInfo)
    myInfo.Initialize(args)

    fmt.Println(fmt.Sprintf("Launching build tool for module [%s] ...", GBuildInfo.ModuleName))
    fmt.Println(fmt.Sprintf("Build configuration: [%s].", GBuildInfo.ToString()))

    if slices.Contains(args, "--pre-build") {
        LaunchPreBuildTasks(args)
    } else if slices.Contains(args, "--post-build") {
        LaunchPostBuildTasks(args)
    } else {
        panic("No build task specified.")
    }

    return
}

func LaunchPreBuildTasks(args []string) {
    Shared.CheckRelativeDir(Shared.GeneratedHeadersDir)

    GenerateWorkspaceWideBuildHeaderFile()
    SystematicallyScanHeadersThatAreDue()

    return
}

func LaunchPostBuildTasks(args []string) {
    if GBuildInfo.Kind.IsLaunch() {
        GBuildInfo.CopyWorkspaceContentToLaunch()
    }

    if GBuildInfo.Kind.IsShared() {
        GBuildInfo.CopyModuleBinariesToLaunch()
    }

    return
}

// SystematicallyScanHeadersThatAreDue scans all headers that are due for regeneration.
// Will regenerate generated headers if they are out of date.
func SystematicallyScanHeadersThatAreDue() {
    var relativeTargetDir string = GBuildInfo.GetRelativeModuleDir()
    var allFiles []string = Shared.RecursivelyGetAllFilesInRelativeDir(relativeTargetDir)
    var allHeaders []string
    for _, file := range allFiles {
        if Shared.IsHeaderFile(file) {
            allHeaders = append(allHeaders, file)
        }

        continue
    }

    RecursivelyScanAndOperateOnHeaders(allHeaders)

    return
}
