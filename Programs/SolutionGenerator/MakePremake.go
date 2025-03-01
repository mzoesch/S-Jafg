// Copyright mzoesch. All rights reserved.

package SolutionGenerator

import (
    "Jafg/BuildTool"
    "Jafg/Core"
    "Jafg/Shared"
    "fmt"
    "os"
    "os/exec"
    "strings"
)

func StringToPremakeBool(b bool) string {
    if b {
        return "On"
    }
    return "Off"
}

func GenerateSolutionFromPremake(sln *Core.Solution, bEmulate bool) error {
    MakePchForAllModules(sln)

    err := MakePremakeSolutionScript(sln)
    if err != nil {
        return err
    }

    if bEmulate {
        fmt.Printf("Emulating all compiler pre-build commands for solution [%s/%s] ...\n", sln.GetSavedRelativeDir_Premake(), sln.Name)
        for idxT, _ := range sln.Targets {
            var tar *Core.Target = &sln.Targets[idxT]
            for idxM, _ := range tar.Modules {
                var mod *Core.Module = &tar.Modules[idxM]

                var args []string = []string{
                    "pre-build", fmt.Sprintf("SLN=%s", sln.Name), fmt.Sprintf("MODULE=%s", mod.GetUniqueName()),
                    fmt.Sprintf("KIND=%s", mod.Kind.ToLuaString()), "ARCH=NOT_SET",
                    fmt.Sprintf("TARGET=%s", tar.Name), "PLATFORM=NOT_SET",
                }
                fmt.Printf(
                    "Emulating compiler pre-build commands for module [%s/%s] with args [%v] ...\n",
                    sln.GetSavedRelativeDir_Premake(), mod.Name, args,
                )
                BuildTool.Launch(args)
                continue
            }
            continue
        }
    }

    var cmd *exec.Cmd = nil
    if Shared.IsWindows() {
        var targetExecutable string = fmt.Sprintf("%s/Programs/Vendor/Premake/Bin/premake5.exe", Shared.GetAbsoluteEngineRoot())
        var targetScript string = Shared.ToAbsolutePath(Core.MakeFilePath_LuaOut(sln))
        fmt.Printf("Executing premake in [%s] with the script [%s] ...\n", targetExecutable, targetScript)
        cmd = exec.Command(targetExecutable, fmt.Sprintf("--file=%s", targetScript), "vs2022")
    } else {
        panic("Not implemented.")
    }
    if cmd == nil {
        panic("Command is nil.")
    }

    stdout, err := cmd.Output()
    if err != nil {
        fmt.Println(string(stdout))
        panic(err)
    }
    fmt.Println(string(stdout))

    // Make sure JetBrains Rider can detect the vcs in the root engine dir.
    {
        if Shared.DoesRelativeFileExist(fmt.Sprintf("%s/.idea/.idea.Jafg/.idea/vcs.xml", sln.GetSavedRelativeDir_Premake())) == false {
            Shared.CheckRelativeFile(fmt.Sprintf("%s/.idea/.idea.Jafg/.idea/vcs.xml", sln.GetSavedRelativeDir_Premake()))
            Shared.OpenAndWriteToRelativeFileIfDifferent(
                fmt.Sprintf("%s/.idea/.idea.Jafg/.idea/vcs.xml", sln.GetSavedRelativeDir_Premake()),
                fmt.Sprintf(`<?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  <component name="VcsDirectoryMappings">
    <mapping directory="$PROJECT_DIR$/%s" vcs="Git" />
  </component>
</project>
`,
                    sln.GetChdirUpRelToBuildFile(),
                ),
                true,
            )
        }
    }

    // Symlink the generated solution file to the top level dir.
    {
        var slnF string = ""
        if Shared.IsWindows() {
            slnF = fmt.Sprintf("%s/Jafg.sln", sln.GetSavedRelativeDir_Premake())
        } else {
            panic("Not implemented.")
        }
        if Shared.DoesRelativeFileExist(slnF) == false {
            panic(fmt.Sprintf("Solution file [%s] does not exist.", slnF))
        }

        var symlinkPath string = fmt.Sprintf("Jafg-%s.sln.lnk", sln.Name)
        if Shared.DoesRelativeFileExist(symlinkPath) == false {

            err2 := os.Symlink(slnF, symlinkPath)
            if err2 != nil {
                // Windows will probably fail here. Just ignore it and call PowerShell to create the symlink.
                if Shared.IsWindows() {
                    var absSlnF string = Shared.ToAbsolutePath(slnF)
                    var absSymlinkPath string = Shared.ToAbsolutePath(symlinkPath)
                    absSlnF = strings.ReplaceAll(absSlnF, "/", "\\")
                    absSymlinkPath = strings.ReplaceAll(absSymlinkPath, "/", "\\")
                    var ps1 string = "Programs/Shell/CreateSymlink.ps1"
                    cmdPs1 := exec.Command("powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", ps1, absSlnF, absSymlinkPath)

                    cmdPs1.Stdout = os.Stdout
                    cmdPs1.Stderr = os.Stderr

                    err3 := cmdPs1.Run()
                    if err3 != nil {
                        panic(err3)
                    }
                } else {
                    panic(err2)
                }
            }

            fmt.Printf("Created symlink [%s] -> [%s] ...\n", slnF, symlinkPath)
        }
    }

    return nil
}

func MakePremakeSolutionScript(sln *Core.Solution) error {
    fmt.Printf("Making Premake script for solution [%s/%s] ...\n", sln.GetSavedRelativeDir_Premake(), sln.Name)

    var fPath string = Core.MakeFilePath_LuaOut(sln)
    Shared.CheckRelativeFile(fPath)
    var f *os.File = Shared.OpenRelativeFile(fPath, true, os.O_RDWR)
    defer Shared.CloseFile(f)

    Shared.WriteToFile(
        f,
        "-- Copyright mzoesch. All rights reserved.\n"+
            "-- This file is automatically generated by Jafg and can be deleted with no fear.\n\n",
    )

    var builder strings.Builder = strings.Builder{}
    var b *strings.Builder = &builder

    Wni(b, "local host = os.get()")
    Wni(b, "local scriptPath = debug.getinfo(2, \"S\").source:sub(2)")
    Wni(b, "local scriptDir = path.getdirectory(scriptPath)")
    Wni(b, "language 'C++'")
    Wni(b, "cppdialect 'C++20'")

    Wni(b, "workspace 'Jafg'")
    if len(sln.Startup) == 0 {
        panic("Startup project is not set.")
    }
    Wwi(b, 1, fmt.Sprintf("startproject '%s'", sln.Startup))
    Wwi(b, 1, "platforms { 'Windows64', 'Windows32FastWasm', 'Wasm' }")
    for _, t := range sln.Targets {
        if len(t.Name) == 0 {
            panic("Target name is empty.")
        }
        Wwi(b, 1, fmt.Sprintf("configurations { '%s' }", t.Name))
    }

    if GbPredefineJafgMacros {
        Wwi(b, 1, "defines { 'DECLARE_JAFG_CLASS', 'GENERATED_CLASS_BODY' }")
    }

    Wwi(b, 1, "filter { 'platforms:Windows64' }")
    Wwi(b, 2, "system 'Windows'")
    Wwi(b, 2, "architecture 'x86_64'")
    Wwi(b, 2, "systemversion 'latest'")
    Wwi(b, 2, "toolset 'msc' ")
    Wwi(b, 2, "defines { 'PLATFORM_WINDOWS', 'PLATFORM_WINDOWS_WITH_MSVC' }")
    Wwi(b, 2, "linkoptions { '/SUBSYSTEM:WINDOWS' }")
    Wwi(b, 2, "buildoptions { '/Zc:__cplusplus, /GR-' }")
    Wwi(b, 2, "linkoptions { '/NODEFAULTLIB:LIBCMT', '/NODEFAULTLIB:MSVCRT' }")

    Wwi(b, 1, "filter { 'platforms:Windows32FastWasm' }")
    Wwi(b, 2, "system 'Windows'")
    Wwi(b, 2, "architecture 'x86'")
    Wwi(b, 2, "systemversion 'latest'")
    Wwi(b, 2, "toolset 'gcc' ")
    Wwi(b, 2, "defines { 'PLATFORM_WINDOWS', 'PLATFORM_WINDOWS_WITH_GCC' }")
    Wwi(b, 2, "linkoptions { '-mwindows' }")
    Wwi(b, 2, "buildoptions { '-fno-rtti' }")
    //Wwi(b, 2, "linkoptions { }")

    Wwi(b, 1, "filter { 'platforms:Wasm' }")
    // 'emscripten' is not supported by premake for now ... only some crazy shit with newer versions.
    // So just take a look at the cmake script. We just emulate this here to have pretty syntax highlighting.
    Wwi(b, 2, "system ('windows')") // emscripten
    // Same for the architecture. Just simulate this.
    // We cannot create valid binaries with premake for this platform.
    Wwi(b, 2, "architecture 'x86'") // wasm32 - x86 because wasm is always 32-bit :(
    Wwi(b, 2, "systemversion 'latest'")
    Wwi(b, 2, "defines { 'PLATFORM_WASM' }")
    Wwi(b, 1, "filter {}")

    for _, t := range sln.Targets {
        Wwi(b, 1, fmt.Sprintf("filter { 'configurations:%s' }", t.Name))
        Wwi(b, 2, fmt.Sprintf("runtime '%s'", t.Runtime))
        Wwi(b, 2, fmt.Sprintf("symbols '%s'", StringToPremakeBool(t.Symbols)))
        Wwi(b, 2, fmt.Sprintf("optimize '%s'", StringToPremakeBool(t.Optimize)))
        Wwi(b, 2, fmt.Sprintf("defines { '%s' }", strings.Join(t.Defines, "', '")))
        Wwi(b, 1, "filter {}")
    }

    // All modules have to exist in the same solution. So we just iterate over the first one.
    for idx, _ := range sln.Targets[0].Modules {
        var module *Core.Module = &(&sln.Targets[0]).Modules[idx]
        Wni(b, fmt.Sprintf("group '%s'", module.GetRelativeTopLevelFilter()))

        Wwi(b, 1, fmt.Sprintf("project '%s'", module.Name))
        Wwi(b, 2, fmt.Sprintf("location '%s'", module.GetFunctionalRelativeDir()))
        Wwi(b, 2, fmt.Sprintf("kind '%s'", module.Kind.ToLuaString()))
        Wwi(b, 2, "rtti 'Off'")
        Wwi(b, 2, fmt.Sprintf("targetdir ('%s/Binaries/%%{cfg.platform}-%%{cfg.architecture}/%%{cfg.buildcfg}/%s')",
            sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 2, fmt.Sprintf("objdir ('%s/Intermediate/%%{cfg.platform}-%%{cfg.architecture}/%%{cfg.buildcfg}/%s')",
            sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))

        Wwi(b, 2, "files {")
        Wwi(b, 3, fmt.Sprintf("'%s/%s/**.md',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 3, fmt.Sprintf("'%s/%s/**.py',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 3, fmt.Sprintf("'%s/%s/Source/**.h',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 3, fmt.Sprintf("'%s/%s/Source/**.hpp',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 3, fmt.Sprintf("'%s/%s/Source/**.c',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 3, fmt.Sprintf("'%s/%s/Source/**.cpp',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 3, fmt.Sprintf("'%s/%s/%s/%s/**%s',", sln.GetChdirUpRelToBuildFile(), sln.GetSavedRelativeDir(), module.GetFunctionalRelativeDir(), Core.GhDir, Core.GhExtension))
        Wwi(b, 3, fmt.Sprintf("'%s/%s/%s/%s/**%s',", sln.GetChdirUpRelToBuildFile(), sln.GetSavedRelativeDir(), module.GetFunctionalRelativeDir(), Core.GtDir, Core.GtExtension))
        Wwi(b, 2, "}")

        Wwi(b, 2, "vpaths {")
        Wwi(b, 3, "['/*'] = {")
        Wwi(b, 4, fmt.Sprintf("'%s/%s/*.md',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 4, fmt.Sprintf("'%s/%s/**.py',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 4, fmt.Sprintf("'%s/%s/%s/**%s',", sln.GetChdirUpRelToBuildFile(), sln.GetSavedRelativeDir(), module.GetFunctionalRelativeDir(), Core.GhExtension))
        Wwi(b, 4, fmt.Sprintf("'%s/%s/%s/**%s',", sln.GetChdirUpRelToBuildFile(), sln.GetSavedRelativeDir(), module.GetFunctionalRelativeDir(), Core.GtExtension))
        Wwi(b, 3, "},")
        Wwi(b, 3, "['Source/*'] = { ")
        Wwi(b, 4, fmt.Sprintf("'%s/%s/Source/**.md',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 4, fmt.Sprintf("'%s/%s/Source/**.h',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 4, fmt.Sprintf("'%s/%s/Source/**.hpp',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 4, fmt.Sprintf("'%s/%s/Source/**.c',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 4, fmt.Sprintf("'%s/%s/Source/**.cpp',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 3, "},")
        Wwi(b, 2, "}")

        Wwi(b, 2, "includedirs {")
        Wwi(b, 3, fmt.Sprintf("'%s',", sln.GetChdirUpRelToBuildFile())) // For generated files only. NOT FOR SOURCE FILES.
        Wwi(b, 3, fmt.Sprintf("'%s/%s/Source/Internal',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 3, fmt.Sprintf("'%s/%s/Source/Public',", sln.GetChdirUpRelToBuildFile(), module.GetFunctionalRelativeDir()))
        Wwi(b, 3, fmt.Sprintf("'%s/%s/%s/%s',", sln.GetChdirUpRelToBuildFile(), sln.GetSavedRelativeDir(), module.GetFunctionalRelativeDir(), Core.GhDir))
        Wwi(b, 2, "}")

        for i, _ := range sln.Targets {
            volatileTarget := &sln.Targets[i]
            volatileModule := &volatileTarget.Modules[idx]
            var transitiveDependencies []*Core.Module
            volatileModule.GetTransitiveAllDependencies(volatileTarget, &transitiveDependencies)

            Wwi(b, 2, fmt.Sprintf("filter { 'configurations:%s' }", volatileTarget.Name))

            if volatileModule.PchUsage.IsAllowed() {
                var pchSource string = fmt.Sprintf("%s/%s/%s/%s/%s/%s",
                    sln.GetChdirUpRelToBuildFile(), sln.GetSavedRelativeDir(), volatileModule.GetFunctionalRelativeDir(), Core.CgtDir, volatileTarget.Name, Core.FilePath_PchSource)
                Wwi(b, 3, fmt.Sprintf("pchheader '%s'", Core.FilePath_PchHeader))
                Wwi(b, 3, fmt.Sprintf("pchsource '%s'", pchSource))
                Wwi(b, 3, fmt.Sprintf("filter { 'configurations:%s', 'files:**.c' }", volatileTarget.Name))
                Wwi(b, 4, "flags { 'NoPCH' }")
                Wwi(b, 3, fmt.Sprintf("filter { 'configurations:%s', 'files:**.cpp' }", volatileTarget.Name))
                Wwi(b, 4, fmt.Sprintf("forceincludes { '%s' }", Core.FilePath_PchHeader))
                Wwi(b, 3, fmt.Sprintf("filter { 'configurations:%s' }", volatileTarget.Name))
            }

            if volatileModule.Kind.IsLaunch() {
                Wwi(b, 3, fmt.Sprintf("filter { 'configurations:%s', 'platforms:Windows64 or Windows32FastWasm' }", volatileTarget.Name))
                Wwi(b, 4, "entrypoint 'WinMainCRTStartup'")
                Wwi(b, 3, fmt.Sprintf("filter { 'configurations:%s' }", volatileTarget.Name))
            }

            Wwi(b, 3, "if host == 'windows' then")
            Wwi(b, 4, "prebuildcommands {")
            Wwi(b, 5,
                fmt.Sprintf("_WORKING_DIR .. '/%s/Scripts/python.exe ' .. _WORKING_DIR .. '/Program.py ", Core.DirPath_Venv)+
                    "--INVOKE --BuildTool pre-build "+
                    fmt.Sprintf("SLN=%s ", sln.Name)+
                    fmt.Sprintf("MODULE=%s ", volatileModule.GetUniqueName())+
                    "KIND=%{cfg.kind} "+
                    "PLATFORM=%{cfg.platform} ARCH=%{cfg.architecture} "+
                    "TARGET=%{cfg.buildcfg}'",
            )
            Wwi(b, 4, "}")
            Wwi(b, 4, "postbuildcommands {")
            Wwi(b, 5,
                fmt.Sprintf("_WORKING_DIR .. '/%s/Scripts/python.exe ' .. _WORKING_DIR .. '/Program.py ", Core.DirPath_Venv)+
                    "--INVOKE --BuildTool post-build "+
                    fmt.Sprintf("SLN=%s ", sln.Name)+
                    fmt.Sprintf("MODULE=%s ", volatileModule.GetUniqueName())+
                    "KIND=%{cfg.kind} "+
                    "PLATFORM=%{cfg.platform} ARCH=%{cfg.architecture} "+
                    "TARGET=%{cfg.buildcfg}'",
            )
            Wwi(b, 4, "}")
            Wwi(b, 3, "else")
            Wwi(b, 4, "prebuildcommands {")
            Wwi(b, 5,
                fmt.Sprintf("_WORKING_DIR .. '/%s/bin/python3 ' .. _WORKING_DIR .. '/Program.py ", Core.DirPath_Venv)+
                    "--INVOKE --BuildTool pre-build "+
                    fmt.Sprintf("SLN=%s ", sln.Name)+
                    fmt.Sprintf("MODULE=%s ", volatileModule.GetUniqueName())+
                    "KIND=%{cfg.kind} "+
                    "PLATFORM=%{cfg.platform} ARCH=%{cfg.architecture} "+
                    "TARGET=%{cfg.buildcfg}'",
            )
            Wwi(b, 4, "}")
            Wwi(b, 4, "postbuildcommands {")
            Wwi(b, 5,
                fmt.Sprintf("_WORKING_DIR .. '/%s/bin/python3 ' .. _WORKING_DIR .. '/Program.py ", Core.DirPath_Venv)+
                    "--INVOKE --BuildTool post-build "+
                    fmt.Sprintf("SLN=%s ", sln.Name)+
                    fmt.Sprintf("MODULE=%s ", volatileModule.GetUniqueName())+
                    "KIND=%{cfg.kind} "+
                    "PLATFORM=%{cfg.platform} ARCH=%{cfg.architecture} "+
                    "TARGET=%{cfg.buildcfg}'",
            )
            Wwi(b, 4, "}")
            Wwi(b, 3, "end")

            Wwi(b, 3, "defines {")
            for _, mod := range sln.Targets[i].Modules {
                if Shared.ContainsByPredicate(transitiveDependencies, func(m *Core.Module) bool {
                    return mod.IsEqual(m)
                }) {
                    if mod.IsEqual(volatileModule) {
                        if volatileModule.Kind.IsShared() {
                            Wwi(b, 4, fmt.Sprintf("'%s_API=PLATFORM_CALLSPEC_OUT',", strings.ToUpper(mod.Name)))
                            Wwi(b, 4, fmt.Sprintf("'%s_EXTERN=PLATFORM_EXTERNSPEC_OUT',", strings.ToUpper(mod.Name)))
                        } else {
                            Wwi(b, 4, fmt.Sprintf("'%s_API=',", strings.ToUpper(mod.Name)))
                            Wwi(b, 4, fmt.Sprintf("'%s_EXTERN=',", strings.ToUpper(mod.Name)))
                        }
                    } else if mod.Kind.IsLaunch() {
                        panic(fmt.Sprintf("Launch dependencies are not allowed. Faulty dependency: %s.", mod.Name))
                    } else if mod.Kind.IsShared() {
                        Wwi(b, 4, fmt.Sprintf("'%s_API=PLATFORM_CALLSPEC_IN',", strings.ToUpper(mod.Name)))
                        Wwi(b, 4, fmt.Sprintf("'%s_EXTERN=PLATFORM_EXTERNSPEC_IN',", strings.ToUpper(mod.Name)))
                    } else if mod.Kind.IsStatic() {
                        Wwi(b, 4, fmt.Sprintf("'%s_API=',", strings.ToUpper(mod.Name)))
                        Wwi(b, 4, fmt.Sprintf("'%s_EXTERN=',", strings.ToUpper(mod.Name)))
                    } else {
                        panic(fmt.Sprintf("Unknown module kind: %s.", mod.Kind.ToLuaString()))
                    }
                }
            }
            Wwi(b, 3, "}")

            Wwi(b, 3, "files {")
            Wwi(b, 4, fmt.Sprintf("'%s/%s/%s/%s/%s/**%s',", sln.GetChdirUpRelToBuildFile(), sln.GetSavedRelativeDir(), volatileModule.GetFunctionalRelativeDir(), Core.CghDir, volatileTarget.Name, Core.GhExtension))
            Wwi(b, 4, fmt.Sprintf("'%s/%s/%s/%s/%s/**%s',", sln.GetChdirUpRelToBuildFile(), sln.GetSavedRelativeDir(), volatileModule.GetFunctionalRelativeDir(), Core.CgtDir, volatileTarget.Name, Core.GtExtension))
            Wwi(b, 3, "}")

            Wwi(b, 3, "includedirs {")
            Wwi(b, 4, fmt.Sprintf("'%s/%s/%s/%s/%s',", sln.GetChdirUpRelToBuildFile(), sln.GetSavedRelativeDir(), volatileModule.GetFunctionalRelativeDir(), Core.CghDir, volatileTarget.Name))
            for _, d := range transitiveDependencies {
                if !d.IsEqual(volatileModule) {
                    Wwi(b, 4, fmt.Sprintf("'%s/%s/Source/Public',", sln.GetChdirUpRelToBuildFile(), d.GetFunctionalRelativeDir()))
                    Wwi(b, 4, fmt.Sprintf("'%s/%s/%s/%s',", sln.GetChdirUpRelToBuildFile(), sln.GetSavedRelativeDir(), d.GetFunctionalRelativeDir(), Core.GhDir))
                }
            }
            for _, d := range volatileModule.NativeIncludeDirs {
                Wwi(b, 4, fmt.Sprintf("'%s/%s',", sln.GetChdirUpRelToBuildFile(), d))
            }
            Wwi(b, 3, "}")

            Wwi(b, 3, "links {")
            for _, d := range transitiveDependencies {
                if !d.IsEqual(volatileModule) {
                    Wwi(b, 4, fmt.Sprintf("'%s',", d.Name))
                }
            }
            for _, d := range volatileModule.NativeDependencies {
                Wwi(b, 4, fmt.Sprintf("'%s/%s',", sln.GetChdirUpRelToBuildFile(), d))
            }
            Wwi(b, 3, "}")

            Wwi(b, 2, "filter {}")
            continue
        }

        Wni(b, "group ''")
        continue
    }

    if Shared.DoesRelativeFileExist(fmt.Sprintf("%s/Utility/Reindex/README.md", sln.GetSavedRelativeDir_Premake())) == false {
        Shared.CheckRelativeFile(fmt.Sprintf("%s/Utility/Reindex/README.md", sln.GetSavedRelativeDir_Premake()))
        Shared.OpenAndWriteToRelativeFileIfDifferent(fmt.Sprintf("%s/Utility/Reindex/README.md", sln.GetSavedRelativeDir_Premake()), `
# Reindex
Reindex all modules generated macros to fix IntelliSense errors. Just hit rebuild selected project and you are done.
# Reindex New Files
Same as 'Reindex' but also check if new files were generated and reindex them also.
`, true)
    }
    Wni(b, "group 'AAAA_Utility'")

    Wwi(b, 1, "project 'Reindex'")
    Wwi(b, 2, "location 'Utility/Reindex'")
    Wwi(b, 2, "kind 'Utility'")
    Wwi(b, 2, "files {")
    Wwi(b, 3, "'Utility/Reindex/**.md',")
    Wwi(b, 2, "}")
    Wwi(b, 2, "vpaths {")
    Wwi(b, 3, "['/*'] = {")
    Wwi(b, 4, "'Utility/Reindex/**.md',")
    Wwi(b, 3, "},")
    Wwi(b, 3, "['Internal/*'] = {")
    Wwi(b, 4, fmt.Sprintf("'%s/**.h',", sln.GetChdirUpRelToBuildFile()))
    Wwi(b, 4, fmt.Sprintf("'%s/**.hpp',", sln.GetChdirUpRelToBuildFile()))
    Wwi(b, 3, "},")
    Wwi(b, 2, "}")
    Wwi(b, 2, "buildinputs {}")
    Wwi(b, 2, "filter 'system:windows'")
    Wwi(b, 3, "postbuildcommands {")
    Wwi(b, 4,
        fmt.Sprintf("_WORKING_DIR .. '/%s/Scripts/python.exe ' .. _WORKING_DIR .. '/Program.py ", Core.DirPath_Venv)+
            "--INVOKE --EmulationUtility WHAT=OnlyExisting "+
            fmt.Sprintf("SLN=%s',", sln.Name),
    )
    Wwi(b, 3, "}")
    Wwi(b, 2, "filter 'system:linux or system:macosx'")
    Wwi(b, 3, "postbuildcommands {")
    Wwi(b, 4,
        fmt.Sprintf("_WORKING_DIR .. '/%s/bin/python3 ' .. _WORKING_DIR .. '/Program.py ", Core.DirPath_Venv)+
            "--INVOKE --EmulationUtility WHAT=OnlyExisting "+
            fmt.Sprintf("SLN=%s'", sln.Name),
    )
    Wwi(b, 3, "}")
    Wwi(b, 2, "filter {}")

    Wwi(b, 1, "project 'ReindexNewFiles'")
    Wwi(b, 2, "location 'Utility/Reindex'")
    Wwi(b, 2, "kind 'Utility'")
    Wwi(b, 2, "files {")
    Wwi(b, 3, "'Utility/Reindex/**.md',")
    Wwi(b, 2, "}")
    Wwi(b, 2, "vpaths {")
    Wwi(b, 3, "['/*'] = {")
    Wwi(b, 4, "'Utility/Reindex/**.md',")
    Wwi(b, 3, "},")
    Wwi(b, 3, "['Internal/*'] = {")
    Wwi(b, 4, fmt.Sprintf("'%s/**.h',", sln.GetChdirUpRelToBuildFile()))
    Wwi(b, 4, fmt.Sprintf("'%s/**.hpp',", sln.GetChdirUpRelToBuildFile()))
    Wwi(b, 3, "},")
    Wwi(b, 2, "}")
    Wwi(b, 2, "buildinputs {}")
    Wwi(b, 2, "filter 'system:windows'")
    Wwi(b, 3, "postbuildcommands {")
    Wwi(b, 4,
        fmt.Sprintf("_WORKING_DIR .. '/%s/Scripts/python.exe ' .. _WORKING_DIR .. '/Program.py ", Core.DirPath_Venv)+
            "--INVOKE --EmulationUtility WHAT=All "+
            fmt.Sprintf("SLN=%s',", sln.Name),
    )
    Wwi(b, 3, "}")
    Wwi(b, 2, "filter 'system:linux or system:macosx'")
    Wwi(b, 3, "postbuildcommands {")
    Wwi(b, 4,
        fmt.Sprintf("_WORKING_DIR .. '/%s/bin/python3 ' .. _WORKING_DIR .. '/Program.py ", Core.DirPath_Venv)+
            "--INVOKE --EmulationUtility WHAT=All "+
            fmt.Sprintf("SLN=%s'", sln.Name),
    )
    Wwi(b, 3, "}")
    Wwi(b, 2, "filter {}")

    Wni(b, "group ''")

    Shared.WriteToFile(f, b.String())
    fmt.Println("Finished generating premake solution.")
    return nil
}
