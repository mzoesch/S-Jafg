// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "fmt"
    "slices"
    "strings"
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
    } else if slices.Contains(args, "--debug-run-make-vsystem") {
        GCurrentHeaderState = new(CurrentHeaderState)
        GCurrentHeaderState.filename = "VFileSystem"
        GCurrentHeaderState.absoluteFilePath = Shared.NormalizePath(Shared.GetCheckedAbsolutePath("Engine/Engine/Source/Public/System/VFileSystem.h"))
        GCurrentHeaderState.builder = new(strings.Builder)
        GCurrentHeaderState.translationBuilder = new(strings.Builder)
        MakeVirtualFilesystem()
        ConditionallyWriteGeneratedFiles()
        GCurrentHeaderState = nil
    } else {
        panic("No build task specified.")
    }

    GBuildInfo = nil

    return
}

func LaunchPreBuildTasks(args []string) {
    /*
     * Absolute necessary directories.
     * The standard api for the platform filesystem does not allow, for whatever reason, to create a file
     * inside a directory that currently does not exist. So we just recursively create all potential points
     * of interest directories.
     */
    Shared.CheckRelativeDir(Shared.GeneratedHeadersDir)
    Shared.CheckRelativeDir(Shared.GeneratedTranslationsDir)

    var relativeTargetDir string = GBuildInfo.GetRelativeModuleDir()
    var allFiles []string = Shared.RecursivelyGetAllFilesInRelativeDir(relativeTargetDir)
    var allHeaders []string
    for _, file := range allFiles {
        if Shared.IsHeaderFile(file) {
            allHeaders = append(allHeaders, file)
        }

        continue
    }

    GObjectStructure = new(ObjectHierarchy)
    GObjectStructure.LoadCache()
    ScanAllHeadersForObjectStructure(allHeaders)
    GObjectStructure.ResolveDeferredNodes()
    GObjectStructure.RecursivelySortByName()
    GObjectStructure.SaveToCache()

    RecursivelyScanAndOperateOnHeaders(allHeaders)

    var targetedModule *Shared.Module = Shared.GApp.GetCheckedModuleByName(GBuildInfo.ModuleName)
    Shared.CheckRelativeDir(GBuildInfo.GetRelativeGeneratedTranslationDir())
    var pchSource = fmt.Sprintf("%s/%sAfx%s",
        GBuildInfo.GetRelativeGeneratedTranslationDir(), GBuildInfo.ModuleName, Shared.GeneratedTranslationsExtension)
    ConditionallyWritePchSourceFileForModule(targetedModule.Pch.IsUse(), pchSource)

    GObjectStructure = nil

    return
}

func LaunchPostBuildTasks(args []string) {
    if GBuildInfo.Kind.IsLaunch() {
        GBuildInfo.CopyWorkspaceContentToLaunch()
        GBuildInfo.CopyPrecompiledSharedLibrariesToLaunch()
    }

    if GBuildInfo.Kind.IsShared() {
        GBuildInfo.CopyModuleBinariesToLaunch()
    }

    return
}
