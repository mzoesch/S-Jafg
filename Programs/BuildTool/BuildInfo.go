// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "fmt"
    "strings"
)

// BuildInfo is a struct that holds information about the current build for a specific module.
type BuildInfo struct {
    ModuleName string

    Kind        Shared.ProjectKind
    BuildConfig Shared.BuildConfig
    buildTarget string

    FullBuildConfig string

    Platform     string
    System       string
    Architecture string
}

var GBuildInfo *BuildInfo = nil

func (bi *BuildInfo) Initialize(args []string) {
    if GBuildInfo != nil {
        panic("BuildInfo already initialized.")
    }

    GBuildInfo = bi

    {
        bFound, Value := Shared.GetArgValue(args, "--MOD_NAME")
        if !bFound {
            panic("Could not find the module name")
        }
        bi.ModuleName = Value
    }

    {
        bFound, Value := Shared.GetArgValueKind(args, "--CFG_KIND")
        if !bFound {
            panic("Could not find the project kind")
        }
        bi.Kind = Value
    }

    {
        bFound, Value := Shared.GetArgValue(args, "--BUILD_CONFIG")
        if !bFound {
            panic("Could not find the build configuration")
        }
        bi.BuildConfig = Shared.GetBuildConfigFromString(Value)
        bi.FullBuildConfig = Value
        // Split value by "-" to get the target name.
        if !strings.Contains(Value, "-") {
            panic(fmt.Sprintf("Could not find the build target inside the build configuration %s.", Value))
        }
        bi.buildTarget = strings.Split(Value, "-")[1]
    }

    {
        bFound, Value := Shared.GetArgValue(args, "--PLATFORM")
        if !bFound {
            panic("Could not find the platform")
        }
        bi.Platform = Value
    }

    {
        bFound, Value := Shared.GetArgValue(args, "--CFG_SYSTEM")
        if !bFound {
            panic("Could not find the system")
        }
        bi.System = Value
    }

    {
        bFound, Value := Shared.GetArgValue(args, "--CFG_ARCHITECTURE")
        if !bFound {
            panic("Could not find the architecture")
        }
        bi.Architecture = Value
    }

    return
}

func (bi *BuildInfo) ToString() string {
    return fmt.Sprintf("{%s, %s, %s, %s, %s, %s}",
        bi.ModuleName, bi.Kind.ToString(),
        bi.BuildConfig.ToString(), bi.Platform,
        bi.System, bi.Architecture,
    )
}

func (bi *BuildInfo) CopyModuleBinariesToLaunch() {
    if !Shared.GApp.Config.DoCopySharedBinaries() {
        fmt.Printf("Skipping copying of shared binaries of modules [%s].\n", bi.ModuleName)
        return
    }

    fmt.Printf("Copying shared binaries of module [%s] to launch directory.\n", bi.ModuleName)

    var ShippedExtension []string = []string{".dll"}
    var DevelopmentExtension []string = []string{".pdb"}

    for _, mod := range Shared.GApp.GetAllModules() {
        if !mod.GetKind().IsLaunch() {
            continue
        }

        var relativeSource string = Shared.GetBinTargetRelativePath(
            bi.System, bi.Architecture, bi.BuildConfig.ToString(), bi.buildTarget,
            Shared.GApp.GetCheckedModuleByName(bi.ModuleName).GetRelativeModuleDir(),
        )
        var relativeTarget string = Shared.GetBinTargetRelativePath(
            bi.System, bi.Architecture, bi.BuildConfig.ToString(), bi.buildTarget,
            mod.GetRelativeModuleDir(),
        )

        if !bi.BuildConfig.IsShipping() {
            Shared.CopyRelativeDirectoryByExtension(relativeSource, relativeTarget, true, DevelopmentExtension, false)
        }
        Shared.CopyRelativeDirectoryByExtension(relativeSource, relativeTarget, true, ShippedExtension, false)

        continue
    }

    fmt.Printf("Finished copying shared binaries of module [%s] to launch directory.\n", bi.ModuleName)

    return
}

func (bi *BuildInfo) CopyWorkspaceContentToLaunch() {
    if !Shared.GApp.Config.DoCopySharedContent() {
        fmt.Println("Skipping copying of shared content.")
        return
    }

    fmt.Printf("Copying shared content to launch directory of module [%s].\n", bi.ModuleName)

    Shared.CopyRelativeDirectory(
        Shared.ContentDir,
        Shared.GetBinTargetRelativePath(bi.System, bi.Architecture, bi.BuildConfig.ToString(), bi.buildTarget, bi.ModuleName),
        true,
        true,
    )

    fmt.Printf("Finished copying shared content to launch directory of module [%s].\n", bi.ModuleName)

    return
}

func (bi *BuildInfo) _PrivateGetTargetDir() string {
    return Shared.GetBinTargetRelativePath(bi.System, bi.Architecture, bi.BuildConfig.ToString(), bi.buildTarget, bi.ModuleName)
}

func (bi *BuildInfo) CopyPrecompiledSharedLibrariesToLaunch() {
    fmt.Println("Copying precompiled shared libraries to launch directory.")

    {
        var source string = fmt.Sprintf("Engine/Vendor/Lib/FastNoise.dll")
        var target string = fmt.Sprintf("%s/FastNoise.dll", bi._PrivateGetTargetDir())
        Shared.CheckRelativeFile(target)
        if Shared.CopyAbsoluteFileIfDifferent(Shared.GetCheckedAbsolutePath(source), Shared.GetAbsolutePathNoCheck(target)) {
            fmt.Println("    Copied FastNoise.dll to[" + target + "].")
        }
    }
    {
        var source string = fmt.Sprintf("Engine/Vendor/Lib/FastNoiseD.dll")
        var target string = fmt.Sprintf("%s/FastNoiseD.dll", bi._PrivateGetTargetDir())
        Shared.CheckRelativeFile(target)
        if Shared.CopyAbsoluteFileIfDifferent(Shared.GetCheckedAbsolutePath(source), Shared.GetAbsolutePathNoCheck(target)) {
            fmt.Println("    Copied FastNoiseD.dll to[" + target + "].")
        }
    }
    {
        var source string = fmt.Sprintf("Engine/Vendor/Lib/FastNoiseD.pdb")
        var target string = fmt.Sprintf("%s/FastNoiseD.pdb", bi._PrivateGetTargetDir())
        Shared.CheckRelativeFile(target)
        if Shared.CopyAbsoluteFileIfDifferent(Shared.GetCheckedAbsolutePath(source), Shared.GetAbsolutePathNoCheck(target)) {
            fmt.Println("    Copied FastNoiseD.pdb to[" + target + "].")
        }
    }
    {
        var source string = fmt.Sprintf("Engine/Vendor/Lib/freetype.pdb")
        var target string = fmt.Sprintf("%s/freetype.pdb", bi._PrivateGetTargetDir())
        Shared.CheckRelativeFile(target)
        if Shared.CopyAbsoluteFileIfDifferent(Shared.GetCheckedAbsolutePath(source), Shared.GetAbsolutePathNoCheck(target)) {
            fmt.Println("    Copied freetype.pdb to[" + target + "].")
        }
    }
    {
        var source string = fmt.Sprintf("Engine/Vendor/Lib/libFastNoise.dll")
        var target string = fmt.Sprintf("%s/libFastNoise.dll", bi._PrivateGetTargetDir())
        Shared.CheckRelativeFile(target)
        if Shared.CopyAbsoluteFileIfDifferent(Shared.GetCheckedAbsolutePath(source), Shared.GetAbsolutePathNoCheck(target)) {
            fmt.Println("    Copied libFastNoise.dll to[" + target + "].")
        }
    }
    {
        var source string = fmt.Sprintf("Engine/Vendor/Lib/libFastNoiseD.dll")
        var target string = fmt.Sprintf("%s/libFastNoiseD.dll", bi._PrivateGetTargetDir())
        Shared.CheckRelativeFile(target)
        if Shared.CopyAbsoluteFileIfDifferent(Shared.GetCheckedAbsolutePath(source), Shared.GetAbsolutePathNoCheck(target)) {
            fmt.Println("    Copied libFastNoiseD.dll to[" + target + "].")
        }
    }

    fmt.Println("Finished copying precompiled shared libraries to launch directory.")

    return
}

func (bi *BuildInfo) GetRelativeModuleDir() string {
    for _, mod := range Shared.GApp.GetAllModules() {
        if mod.GetUsableName() == bi.ModuleName {
            return mod.GetRelativeModuleDir()
        }

        continue
    }

    panic(fmt.Sprintf("Could not find module [%s].", bi.ModuleName))

    return ""
}

func (bi *BuildInfo) GetRelativeGeneratedTranslationDir() string {
    return fmt.Sprintf("%s/%s",
        Shared.GeneratedTranslationsDir,
        bi.GetRelativeModuleDir(),
    )
}
