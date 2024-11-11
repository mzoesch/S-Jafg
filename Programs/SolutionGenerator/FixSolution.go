// Copyright 2024 mzoesch. All rights reserved.

package SolutionGenerator

import (
    "Jafg/Shared"
    "bufio"
    "fmt"
    "os"
    "strings"
)

func PostLuaRun() {
    FixAfx()

    fmt.Println("Finished post lua run.")

    return
}

func FixAfx() {
    fmt.Println("Fixing Afx from /Yu to /Yc ...")

    for _, proj := range Shared.GApp.Projects {
        for _, mod := range proj.Modules {
            FixAfxForModule(&mod)
            continue
        }

        continue
    }

    fmt.Printf("Excluding c files from afx.")
    ExcludeCFilesFromAfx()

    return
}

func FixAfxForModule(mod *Shared.Module) {
    //if mod.Pch.IsUse() {
    //    var afxFile string = Shared.GetCheckedAbsolutePath(fmt.Sprintf(
    //        "%s/%s.vcxproj",
    //        mod.GetRelativeModuleDir(),
    //        mod.GetUsableName(),
    //    ))
    //
    //    FixAfxForFile(afxFile)
    //}

    //if mod.Pch.IsUse() {
    //    var afxFile string = Shared.GetCheckedAbsolutePath(fmt.Sprintf(
    //        "%s/Vslf-%s.vcxproj",
    //        GetVslfModuleProjectRelativeDir(mod),
    //        mod.GetUsableName(),
    //    ))
    //
    //    FixAfxForFile(afxFile)
    //}

    return
}

func FixAfxForFile(absFilePath string) {
    fmt.Printf("Fixing Afx for file: %s.\n", absFilePath)

    var file *os.File = Shared.OpenAbsoluteFile(absFilePath, false, os.O_RDWR)
    var lines []string
    var scanner *bufio.Scanner = bufio.NewScanner(file)

    for scanner.Scan() {
        var line string = scanner.Text()
        if strings.Contains(line, "<PrecompiledHeader>Use</PrecompiledHeader>") {
            line = strings.ReplaceAll(line, "<PrecompiledHeader>Use</PrecompiledHeader>",
                "<PrecompiledHeader>Create</PrecompiledHeader>")
        }
        lines = append(lines, line)
    }

    Shared.TruncateFile(file)
    Shared.SeekFileBeginning(file)

    for _, line := range lines {
        Shared.WriteToFile(file, line+"\n")
    }

    Shared.CloseFile(file)

    return
}

func ExcludeCFilesFromAfx() {
    for _, proj := range Shared.GApp.Projects {
        for _, mod := range proj.Modules {
            ExcludeCFilesFromAfxForModule(&mod)
            continue
        }

        continue
    }

    return
}

func ExcludeCFilesFromAfxForModule(mod *Shared.Module) {
    if mod.Pch.IsUse() {
        var afxFile string = Shared.GetCheckedAbsolutePath(fmt.Sprintf(
            "%s/%s.vcxproj",
            mod.GetRelativeModuleDir(),
            mod.GetUsableName(),
        ))

        ExcludeCFilesFromAfxForFile(afxFile)
    }

    return
}

func GetIncludeFromNameForAfxCFix(line string) string {

    var out string = ""

    var bAddUntilEndQuote bool = false
    var curIndexInStartString int = 0
    var startString string = "Include=\""
    var countRuneInStartString int = Shared.CountRunesInString(startString) - 1
    for i := 0; i < Shared.CountRunesInString(line); i++ {
        var char string = string(line[i])

        if bAddUntilEndQuote {
            if char == "\"" {
                out += char
                break
            }

            out += char
            continue
        }

        if len(startString) > len(line) {
            panic("Include=\" not found in line.")
        }

        if curIndexInStartString > countRuneInStartString {
            out += startString
            out += char
            bAddUntilEndQuote = true
            continue
        }

        if char == string(startString[curIndexInStartString]) {
            curIndexInStartString++
        } else {
            curIndexInStartString = 0
        }

        continue
    }

    if len(out) == 0 {
        panic("Include=\" not found in line.")
    }

    return out
}

func ExcludeCFilesFromAfxForFile(absFilePath string) {
    fmt.Printf("Excluding c files from afx for file: %s.\n", absFilePath)

    var file *os.File = Shared.OpenAbsoluteFile(absFilePath, false, os.O_RDWR)
    var lines []string
    var scanner *bufio.Scanner = bufio.NewScanner(file)

    for scanner.Scan() {
        var line string = scanner.Text()
        if !strings.Contains(line, "ClCompile Include=\"") {
            lines = append(lines, line)
            continue
        }

        var include = GetIncludeFromNameForAfxCFix(line)

        if !strings.Contains(line, ".c\"") {
            lines = append(lines, line)
            continue
        }

        if !strings.Contains(line, "/>") {
            lines = append(lines, line)
            continue
        }

        var conditions = string(`      <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='Shipping-TestUnit Windows64|x64'">NotUsing</PrecompiledHeader>
      <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='Debug-TestUnit Windows64|x64'">NotUsing</PrecompiledHeader>
      <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='Debug-Server Windows64|x64'">NotUsing</PrecompiledHeader>
      <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='Development-Client Windows64|x64'">NotUsing</PrecompiledHeader>
      <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='Shipping-Server Windows64|x64'">NotUsing</PrecompiledHeader>
      <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='Development-TestUnit Windows64|x64'">NotUsing</PrecompiledHeader>
      <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='Shipping-Client Windows64|x64'">NotUsing</PrecompiledHeader>
      <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='Debug-Client Windows64|x64'">NotUsing</PrecompiledHeader>
      <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='Development-Server Windows64|x64'">NotUsing</PrecompiledHeader>`,
        )

        lines = append(lines, fmt.Sprintf("    <ClCompile %s>", include))
        lines = append(lines, conditions)
        lines = append(lines, "    </ClCompile>")

        continue
    }

    Shared.TruncateFile(file)
    Shared.SeekFileBeginning(file)

    for _, line := range lines {
        Shared.WriteToFile(file, line+"\n")
    }

    Shared.CloseFile(file)

    return
}
