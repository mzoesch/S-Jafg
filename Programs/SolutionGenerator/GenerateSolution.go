// Copyright 2024 mzoesch. All rights reserved.

package SolutionGenerator

import (
    "Jafg/Shared"
    "fmt"
    "os"
    "strings"
)

func GenerateSolution() {
    var handle *os.File = PrepareLuaBuildFile()
    WriteLuaBuildFileBody(handle)

    Shared.CloseFile(handle)

    fmt.Println("Finished generating solution.")

    return
}

func PrepareLuaBuildFile() *os.File {
    fmt.Println("Preparing Lua build file...")

    Shared.CheckRelativeFile(Shared.SolutionLuaFile)

    var file *os.File = Shared.OpenRelativeFile(Shared.SolutionLuaFile, true, os.O_RDWR)

    Shared.WriteToFile(
        file,
        "-- Copyright 2024 mzoesch. All rights reserved.\n"+
            "-- This file is automatically generated by Jafg and can be deleted with no fear.\n\n",
    )

    return file
}

func WriteWithIndent(builder *strings.Builder, indent int, content string) {
    builder.WriteString(fmt.Sprintf("%s%s", Shared.Indent(indent), content))
}

func GetAllBuildConfigurations() []string {
    var out []string
    for _, targ := range Shared.GApp.GetAllTargets() {
        out = append(out, fmt.Sprintf("Debug-%s", targ.GetSuffix()))
        out = append(out, fmt.Sprintf("Development-%s", targ.GetSuffix()))
        out = append(out, fmt.Sprintf("Shipping-%s", targ.GetSuffix()))

        continue
    }


    return out
}

func GetAllDebugBuildConfigurations() []string {
    var out []string
    for _, targ := range GetAllBuildConfigurations() {
        if strings.Contains(targ, "Debug") {
            out = append(out, targ)
        }

        continue
    }

    return out
}

func GetAllDevelopmentBuildConfigurations() []string {
    var out []string
    for _, targ := range GetAllBuildConfigurations() {
        if strings.Contains(targ, "Development") {
            out = append(out, targ)
        }

        continue
    }

    return out
}

func GetAllShippingBuildConfigurations() []string {
    var out []string
    for _, targ := range GetAllBuildConfigurations() {
        if strings.Contains(targ, "Shipping") {
            out = append(out, targ)
        }

        continue
    }

    return out
}

func GetAllBuildTargetConfigurations(target *Shared.Target) []string {
    var out []string
    for _, buildConfiguration := range GetAllBuildConfigurations() {
        if strings.Contains(buildConfiguration, target.GetSuffix()) {
            out = append(out, buildConfiguration)
        }

        continue
    }

    return out
}

func WriteLuaBuildFileBody(handle *os.File) {
    if handle == nil {
        panic("WriteLuaBuildFileBody called with nil handle.")
    }

    var builder strings.Builder = strings.Builder{}

    WriteWithIndent(&builder, 0, "language 'C++'\n")
    WriteWithIndent(&builder, 0, "cppdialect 'C++20'\n")

    WriteWithIndent(&builder, 0, "workspace 'Jafg'\n")
    WriteWithIndent(&builder, 4, "architecture 'x64'\n")
    WriteWithIndent(&builder, 4, "startproject 'Runtime'\n")
    for _, targ := range GetAllBuildConfigurations() {
        WriteWithIndent(&builder, 4, fmt.Sprintf("configurations { '%s' }\n", targ))
    }
    WriteWithIndent(&builder, 4, "platforms { 'Windows64'}\n")

    WriteWithIndent(&builder, 4, "filter 'platforms:Windows64'\n")
    WriteWithIndent(&builder, 8, "systemversion 'latest'\n")
    WriteWithIndent(&builder, 8, "defines { 'PLATFORM_WINDOWS' }\n")
    WriteWithIndent(&builder, 8, "linkoptions { '/SUBSYSTEM:WINDOWS' }\n")
    WriteWithIndent(&builder, 4, "filter {}\n")

    for _, buildConfiguration := range GetAllDebugBuildConfigurations() {
        WriteWithIndent(&builder, 4, fmt.Sprintf("filter 'configurations:%s'\n", buildConfiguration))
        WriteWithIndent(&builder, 8, "defines { 'IN_DEBUG' }\n")
        WriteWithIndent(&builder, 8, "runtime 'Debug'\n")
        WriteWithIndent(&builder, 8, "symbols 'On'\n")
        WriteWithIndent(&builder, 8, "optimize 'Off'\n")
        WriteWithIndent(&builder, 4, "filter {}\n")
    }

    for _, buildConfiguration := range GetAllDevelopmentBuildConfigurations() {
        WriteWithIndent(&builder, 4, fmt.Sprintf("filter 'configurations:%s'\n", buildConfiguration))
        WriteWithIndent(&builder, 8, "defines { 'IN_DEVELOPMENT' }\n")
        WriteWithIndent(&builder, 8, "runtime 'Release'\n")
        WriteWithIndent(&builder, 8, "symbols 'On'\n")
        WriteWithIndent(&builder, 8, "optimize 'On'\n")
        WriteWithIndent(&builder, 4, "filter {}\n")
    }

    for _, buildConfiguration := range GetAllShippingBuildConfigurations() {
        WriteWithIndent(&builder, 4, fmt.Sprintf("filter 'configurations:%s'\n", buildConfiguration))
        WriteWithIndent(&builder, 8, "defines { 'IN_SHIPPING' }\n")
        WriteWithIndent(&builder, 8, "runtime 'Release'\n")
        WriteWithIndent(&builder, 8, "symbols 'Off'\n")
        WriteWithIndent(&builder, 8, "optimize 'On'\n")
        WriteWithIndent(&builder, 4, "filter {}\n")
    }

    for _, targ := range Shared.GApp.GetAllTargets() {
        for _, buildConfiguration := range GetAllBuildTargetConfigurations(targ) {
            WriteWithIndent(&builder, 4, fmt.Sprintf("filter 'configurations:%s'\n", buildConfiguration))
            for _, def := range targ.AdditionalDefines {
                WriteWithIndent(&builder, 8, fmt.Sprintf("defines { '%s' }\n", def))
            }
            WriteWithIndent(&builder, 4, "filter {}\n")

            continue
        }

        continue
    }

    WriteLuaBuildFileProjectSpecificSection(&builder)
    WriteLuaBuildFileGeneratedSection(&builder)

    Shared.WriteToFile(handle, builder.String())

    return
}

func WriteLuaBuildFileProjectSpecificSection(builder *strings.Builder) {
    if builder == nil {
        panic("WriteLuaBuildFileProjectSpecificSection called with nil builder.")
    }

    for _, proj := range Shared.GApp.Projects {
        WriteLuaBuildFileForSpecificProject(builder, 0, &proj)
    }

    return
}

func WriteLuaBuildFileForSpecificProject(builder *strings.Builder, indent int, proj *Shared.Project) {
    if builder == nil {
        panic("WriteLuaBuildFileForSpecificProject called with nil builder.")
    }
    if proj == nil {
        panic("WriteLuaBuildFileForSpecificProject called with nil project.")
    }

    WriteWithIndent(builder, indent, fmt.Sprintf("group '%s'\n", proj.GetRelativeProjectDir()))

    for _, mod := range proj.Modules {
        WriteLuaBuildFileForSpecificModule(builder, indent+4, &mod)
    }

    WriteWithIndent(builder, indent, "group ''\n")

    return
}

func GetRelativePythonProgramLaunchPath(mod *Shared.Module) string {
    var relativeDir string = mod.GetRelativeModuleDir()
    var slashCount int = strings.Count(relativeDir, "/")

    var prefix string = ""
    for i := 0; i < slashCount + 1; i++ {
        prefix += "../"
    }

    return fmt.Sprintf("%sProgram.py", prefix)
}


func WriteLuaBuildFileForSpecificModule(builder *strings.Builder, indent int, mod *Shared.Module) {
    if builder == nil {
        panic("WriteLuaBuildFileForSpecificModule called with nil builder.")
    }
    if mod == nil {
        panic("WriteLuaBuildFileForSpecificModule called with nil module.")
    }

    var defines []string
    defines = append(defines, fmt.Sprintf("CURRENT_PROJECT_NAME=%s", mod.Parent.Name))
    defines = append(defines, fmt.Sprintf("CURRENT_MODULE_NAME=%s", mod.GetUsableName()))
    defines = append(defines, fmt.Sprintf("PRIVATE_JAFG_CURRENT_MODULE_PREPROC_IDENT=%s", mod.GetPreProcIntAsString()))

    WriteWithIndent(builder, indent, fmt.Sprintf("project '%s'\n", mod.GetUsableName()))

    WriteWithIndent(builder, indent+4, fmt.Sprintf("location '%s'\n", mod.GetRelativeModuleDir()))
    WriteWithIndent(builder, indent+4, fmt.Sprintf("kind '%s'\n", mod.GetKind().ToLuaString(&mod.Parent.DefaultKind)))

    WriteWithIndent(builder, indent+4, "prebuildcommands {\n")
    WriteWithIndent(builder, indent+8, "'echo Launching pre build programs ...',\n")
    WriteWithIndent(builder, indent+8, fmt.Sprintf("'python %s --fwd --BuildTool --pre-build --BUILD_CONFIG=%%{cfg.buildcfg} " +
        "--PLATFORM=%%{cfg.platform} --MOD_NAME=%%{prj.name} --CFG_KIND=%%{cfg.kind} "+
        "--CFG_SYSTEM=%%{cfg.system} --CFG_ARCHITECTURE=%%{cfg.architecture}',\n",
        GetRelativePythonProgramLaunchPath(mod),
    ))
    WriteWithIndent(builder, indent+8, "'echo Finished all pre build programs.',\n")
    WriteWithIndent(builder, indent+4, "}\n")

    WriteWithIndent(builder, indent+4, "postbuildcommands {\n")
    WriteWithIndent(builder, indent+8, "'echo Launching post build programs ...',\n")
    WriteWithIndent(builder, indent+8, fmt.Sprintf("'python %s --fwd --BuildTool --post-build --BUILD_CONFIG=%%{cfg.buildcfg} " +
        "--PLATFORM=%%{cfg.platform} --MOD_NAME=%%{prj.name} --CFG_KIND=%%{cfg.kind} "+
        "--CFG_SYSTEM=%%{cfg.system} --CFG_ARCHITECTURE=%%{cfg.architecture}',\n",
        GetRelativePythonProgramLaunchPath(mod),
    ))
    WriteWithIndent(builder, indent+8, "'echo Finished all post build programs.',\n")
    WriteWithIndent(builder, indent+4, "}\n")

    WriteWithIndent(builder, indent+4, fmt.Sprintf(
        "targetdir ('%s/%%{cfg.system}-%%{cfg.architecture}/%%{cfg.buildcfg}/%s/')\n",
        Shared.BinariesDir, mod.GetRelativeModuleDir(),
    ))
    WriteWithIndent(builder, indent+4, fmt.Sprintf(
        "objdir ('%s/%%{cfg.system}-%%{cfg.architecture}/%%{cfg.buildcfg}/%s/')\n",
        Shared.IntermediateDir, mod.GetRelativeModuleDir(),
    ))

    if mod.GetKind().IsLaunch() {
        for _, targ := range Shared.GApp.GetAllTargets() {
            if targ.HasCustomEntryPoint() {
                for _, buildConfiguration := range GetAllBuildTargetConfigurations(targ) {
                    WriteWithIndent(builder, indent+4, fmt.Sprintf(
                        "filter { 'platforms:Windows64', 'configurations:%s' }\n",
                        buildConfiguration,
                    ))
                    WriteWithIndent(builder, indent+8, fmt.Sprintf("entrypoint '%s'\n", targ.CustomEntryPoint))
                    WriteWithIndent(builder, indent+4, "filter {}\n")
                }
            } else {
                for _, buildConfiguration := range GetAllBuildTargetConfigurations(targ) {
                    WriteWithIndent(builder, indent+4, fmt.Sprintf(
                        "filter { 'platforms:Windows64', 'configurations:%s' }\n",
                        buildConfiguration,
                    ))
                    WriteWithIndent(builder, indent+8, fmt.Sprintf("entrypoint 'WinMainCRTStartup'\n"))
                    WriteWithIndent(builder, indent+4, "filter {}\n")
                }
            }

            continue
        }
    }

    WriteWithIndent(builder, indent+4, "files {\n")
    WriteWithIndent(builder, indent+8, fmt.Sprintf("'%s/**.md',\n", mod.GetRelativeModuleDir()))
    WriteWithIndent(builder, indent+8, fmt.Sprintf("'%s/**.jafgmod',\n", mod.GetRelativeModuleDir()))
    WriteWithIndent(builder, indent+8, fmt.Sprintf("'%s/**.jafgproj',\n", mod.Parent.GetRelativeProjectDir()))
    WriteWithIndent(builder, indent+8, fmt.Sprintf("'%s/Source/**.h',\n", mod.GetRelativeModuleDir()))
    WriteWithIndent(builder, indent+8, fmt.Sprintf("'%s/Source/**.hpp',\n", mod.GetRelativeModuleDir()))
    WriteWithIndent(builder, indent+8, fmt.Sprintf("'%s/Source/**.c',\n", mod.GetRelativeModuleDir()))
    WriteWithIndent(builder, indent+8, fmt.Sprintf("'%s/Source/**.cpp',\n", mod.GetRelativeModuleDir()))
    WriteWithIndent(builder, indent+4, "}\n")

    for _, targ := range Shared.GApp.GetAllTargets() {
        var includeDirs []string
        var linkedLibs []string
        includeDirs = append(includeDirs, Shared.GeneratedHeadersDir)
        includeDirs = append(includeDirs, fmt.Sprintf("%s/Source/Public", mod.GetRelativeModuleDir()))
        for _, dep := range mod.GetAllDependenciesTransitive(targ) {
            if dep == "CORE_DEPENDENCIES" {
                includeDirs = append(includeDirs, Shared.VendorIncludeDir)
                /* Has to be included directly and not inside the namespace. */
                includeDirs = append(includeDirs, fmt.Sprintf("%s/Freetype", Shared.VendorIncludeDir))

                linkedLibs = append(linkedLibs, fmt.Sprintf("%s/glfw3.lib", Shared.VendorLibDir))
                linkedLibs = append(linkedLibs, fmt.Sprintf("%s/freetype.lib", Shared.VendorLibDir))

                continue
            }

            var modDep *Shared.Module = Shared.GApp.GetCheckedModuleByName(dep)

            includeDirs = append(includeDirs, fmt.Sprintf(
                "%s/Source/Public",
                modDep.GetRelativeModuleDir(),
            ))
            linkedLibs = append(linkedLibs, modDep.GetUsableName())

            /*
             * Special case for the tester module. We should not hard code this but use the mod config file.
             * But this works for now. Just a short-term solution.
             */
            if mod.GetUsableName() == "Tester" {
                includeDirs = append(includeDirs, fmt.Sprintf(
                    "%s/Source",
                    modDep.GetRelativeModuleDir(),
                ))
            }

            continue
        }

        for _, buildConfiguration := range GetAllBuildTargetConfigurations(targ) {
            if len(includeDirs) == 0 {
                continue
            }

            WriteWithIndent(builder, indent+4, fmt.Sprintf("filter { 'configurations:%s' }\n", buildConfiguration))

            WriteWithIndent(builder, indent+8, "includedirs {\n")
            for _, dir := range includeDirs {
                WriteWithIndent(builder, indent+12, fmt.Sprintf("'%s',\n", dir))
            }
            WriteWithIndent(builder, indent+8, "}\n")

            WriteWithIndent(builder, indent+8, "links {\n")
            for _, lib := range linkedLibs {
                WriteWithIndent(builder, indent+12, fmt.Sprintf("'%s',\n", lib))
            }
            WriteWithIndent(builder, indent+8, "}\n")

            WriteWithIndent(builder, indent+4, "filter { }\n")

            continue
        }

        continue
    }

    WriteWithIndent(builder, indent+4, "defines {\n")
    for _, def := range defines {
        WriteWithIndent(builder, indent+8, fmt.Sprintf("'%s',\n", def))
    }
    WriteWithIndent(builder, indent+4, "}\n")

    if mod.Pch.IsGenerate() || mod.Pch.IsUse() {
        if mod.Pch.IsGenerate() {
            /*
             * To fix the below problem, we rerun this program after the lua script has been executed.
             * Fixing this manually by editing the .vcxproj files.
             * @see Arg: --PostLuaRun
             */
            WriteWithIndent(builder, indent+4, "-- Somehow this does not work??\n")
            WriteWithIndent(builder, indent+4, "-- The IDEA will just set the pch to /Yu but we, of course, need /Yc.\n")
        }
        WriteWithIndent(builder, indent+4, "pchheader 'CoreAfx.h'\n")
        WriteWithIndent(builder, indent+4, fmt.Sprintf(
            "pchsource '%s/Source/Private/CoreAfx.cpp'\n",
            Shared.GApp.GetCheckedModuleByName("Lal").GetRelativeModuleDir(),
        ))
    }

    return
}

func WriteLuaBuildFileGeneratedSection(builder *strings.Builder) {
    // This is totally hard coded. But who in their mind would actually give a shit.

    WriteWithIndent(builder, 0, "group 'Engine'\n")

    WriteWithIndent(builder, 4, fmt.Sprintf("project '%s'\n", Shared.VslfDir))
    WriteWithIndent(builder, 8, fmt.Sprintf("location '%s'\n", Shared.GeneratedHeadersDir))
    WriteWithIndent(builder, 8, fmt.Sprintf("kind '%s'\n", "None"))

    WriteWithIndent(builder, 8, "files {\n")
    WriteWithIndent(builder, 12, fmt.Sprintf("'%s/**.md',\n", Shared.GeneratedHeadersDir))
    WriteWithIndent(builder, 12, fmt.Sprintf("'%s/**.jafgmod',\n", Shared.GeneratedHeadersDir))
    WriteWithIndent(builder, 12, fmt.Sprintf("'%s/**.jafgproj',\n", Shared.GeneratedHeadersDir))
    WriteWithIndent(builder, 12, fmt.Sprintf("'%s/**.h',\n", Shared.GeneratedHeadersDir))
    WriteWithIndent(builder, 12, fmt.Sprintf("'%s/**.hpp',\n", Shared.GeneratedHeadersDir))
    WriteWithIndent(builder, 12, fmt.Sprintf("'%s/**.c',\n", Shared.GeneratedHeadersDir))
    WriteWithIndent(builder, 12, fmt.Sprintf("'%s/**.cpp',\n", Shared.GeneratedHeadersDir))
    WriteWithIndent(builder, 8, "}\n")

    WriteWithIndent(builder, 0, "group ''\n")

    return
}
