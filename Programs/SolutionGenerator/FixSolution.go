// Copyright 2024 mzoesch. All rights reserved.

package SolutionGenerator

import (
    "Jafg/Shared"
    "bufio"
    "fmt"
    "os"
    "path/filepath"
    "strings"
)

func PostLuaRun() {
    FixAfx()
    ChangeIntermediateDir()
    CopyPchSourceFilesToIntermediate()
    FixVcsForRider()

    // DEPRECATED
    //fmt.Println("Finished post lua run.")
    fmt.Println("Finished post cmake run.")

    return
}

func FixAfx() {
    fmt.Println("Fixing Afx from /Yu to /Yc ...")

    //for _, proj := range Shared.GApp.Projects {
    //    for _, mod := range proj.Modules {
    //        FixAfxForModule(&mod)
    //        continue
    //    }
    //
    //    continue
    //}

    // fmt.Printf("Excluding c files from afx.")
    // ExcludeCFilesFromAfx()

    return
}

func FixAfxForModule(mod *Shared.Module) {
    if mod.Pch.IsUse() {
        afxFile, bExists := Shared.GetAbsolutePath(fmt.Sprintf(
            "%s/%s.vcxproj",
            mod.GetRelativeModuleDir(),
            mod.GetUsableName(),
        ))

        if !bExists {
            return
        }

        FixAfxForFile(afxFile)
    }

    if mod.Pch.IsUse() {
        afxFile, bExists := Shared.GetAbsolutePath(fmt.Sprintf(
            "%s/Vslf-%s.vcxproj",
            GetVslfModuleProjectRelativeDir(mod),
            mod.GetUsableName(),
        ))

        if !bExists {
            return
        }

        FixAfxForFile(afxFile)
    }

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
            Shared.SolutionDirOut,
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

func ChangeIntermediateDir() {
    fmt.Println("Changing intermediate dir ...")
    Shared.CheckRelativeDir(Shared.SolutionDirOut)

    var absDir string = Shared.GetCheckedAbsolutePath(Shared.SolutionDirOut)
    var files []string

    var entries, err = os.ReadDir(absDir)
    if err != nil {
        panic(err)
    }
    for _, entry := range entries {
        if entry.IsDir() {
            continue
        }

        files = append(files, entry.Name())
    }

    for _, file := range files {
        if !strings.HasSuffix(file, ".vcxproj") {
            continue
        }
        if strings.Contains(file, "ALL_BUILD.vcxproj") {
            continue
        }
        if strings.Contains(file, "ZERO_CHECK.vcxproj") {
            continue
        }
        if strings.Contains(file, "Vendor.vcxproj") {
            continue
        }
        if strings.Contains(file, "WasmRuntime.vcxproj") {
            continue
        }

        ChangeIntermediateDirForVcxprojFile(Shared.GetCheckedAbsolutePath(fmt.Sprintf(
            "%s/%s", Shared.SolutionDirOut, file,
        )), false)
        continue
    }

    for _, file := range files {
        if !strings.HasSuffix(file, ".vcxproj.filters") {
            continue
        }
        if strings.Contains(file, "ALL_BUILD.vcxproj.filters") {
            continue
        }
        if strings.Contains(file, "ZERO_CHECK.vcxproj.filters") {
            continue
        }
        if strings.Contains(file, "Vendor.vcxproj.filters") {
            continue
        }
        if strings.Contains(file, "WasmRuntime.vcxproj.filters") {
            continue
        }

        ChangeIntermediateDirForVcxprojFile(Shared.GetCheckedAbsolutePath(fmt.Sprintf(
            "%s/%s", Shared.SolutionDirOut, file,
        )), true)
        continue
    }

    RetargetDirectories()

    return
}

func RetargetDirectories() {
    fmt.Println("Retargeting directories ...")

    var absfile = Shared.GetCheckedAbsolutePath(fmt.Sprintf(
        "%s/CMakeFiles/TargetDirectories.txt",
        Shared.SolutionDirOut,
    ))

    var file *os.File = Shared.OpenAbsoluteFile(absfile, false, os.O_RDWR)
    var lines []string
    var scanner *bufio.Scanner = bufio.NewScanner(file)

    var absTargetDir string = Shared.GetCheckedAbsolutePath(Shared.IntermediateDir)
    for scanner.Scan() {
        var line string = scanner.Text()
        if strings.Contains(line, "ALL_BUILD") {
            lines = append(lines, line)
            continue
        }
        if strings.Contains(line, "ZERO_CHECK") {
            lines = append(lines, line)
            continue
        }
        if strings.Contains(line, "Vendor") {
            lines = append(lines, line)
            continue
        }
        if strings.Contains(line, "WasmRuntime") {
            lines = append(lines, line)
            continue
        }

        if strings.Contains(line, ".dir") {
            var outLine = ""

            var lastPathSep int = strings.LastIndex(line, "/")
            if lastPathSep == -1 {
                panic("lastPathSep == -1")
            }

            outLine += absTargetDir + line[lastPathSep:]

            outLine = strings.ReplaceAll(outLine, ".dir", "")
            lines = append(lines, outLine)
            continue
        }

        lines = append(lines, line)

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

func ChangeIntermediateDirForVcxprojFile(absPath string, bFilters bool) {
    var fileName string = filepath.Base(absPath)
    fmt.Printf("Changing intermediate dir for module: %s.\n", fileName)

    var moduleName string
    if bFilters {
        moduleName = strings.TrimSuffix(fileName, ".vcxproj.filters")
    } else {
        moduleName = strings.TrimSuffix(fileName, ".vcxproj")
    }
    var module *Shared.Module = Shared.GApp.GetCheckedModuleByName(moduleName)
    var absPathToIntermediateDir string = Shared.GetAbsolutePathNoCheck(fmt.Sprintf(
        "%s/%s", Shared.IntermediateDir, module.GetRelativeModuleDir(),
    ))
    Shared.CheckAbsoluteDir(absPathToIntermediateDir)

    var file *os.File = Shared.OpenAbsoluteFile(absPath, false, os.O_RDWR)
    var lines []string
    var scanner *bufio.Scanner = bufio.NewScanner(file)

    var toReplace string = fmt.Sprintf("%s.dir", moduleName)
    for scanner.Scan() {
        var line string = scanner.Text()
        if !strings.Contains(line, toReplace) {
            lines = append(lines, line)
            continue
        }

        var toReplaceIndex int = strings.Index(line, toReplace)
        if toReplaceIndex == -1 {
            panic("toReplaceIndex == -1")
        }
        var toReplaceIndexEnd int = toReplaceIndex + len(toReplace)

        if line[toReplaceIndex-1] != '/' && line[toReplaceIndex-1] != '\\' {
            lines = append(lines, line[:toReplaceIndex]+absPathToIntermediateDir+line[toReplaceIndexEnd:])
            continue
        }

        var bOk bool = false
        for i := toReplaceIndexEnd - 1; i >= 0; i-- {
            if line[i] == '>' || line[i] == '"' {
                lines = append(lines, line[:i+1]+absPathToIntermediateDir+line[toReplaceIndexEnd:])
                bOk = true
                break
            }
        }
        if !bOk {
            panic(fmt.Sprintf("Found unexpected beginning of line for line: [%s].", line))
        }

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

func CopyPchSourceFilesToIntermediate() {
    fmt.Println("Copying pch source files to intermediate ...")

    for _, proj := range Shared.GApp.Projects {
        for _, mod := range proj.Modules {
            if mod.Pch.IsUse() {
                CopyPchSourceFilesToIntermediateForModule(&mod)
            }
            continue
        }

        continue
    }

    return
}

func CopyPchSourceFilesToIntermediateForModule(mod *Shared.Module) {
    fmt.Printf("Copying pch source files to intermediate for module: %s.\n", mod.GetUsableName())
    var absSourceDir string = Shared.GetCheckedAbsolutePath(fmt.Sprintf(
        "%s/CMakeFiles/%s.dir",
        Shared.SolutionDirOut, mod.GetUsableName(),
    ))
    if !Shared.DoesAbsoluteDirExist(absSourceDir) {
        panic(fmt.Sprintf("absSourceDir does not exist [%s].", absSourceDir))
    }

    var absTargetDir = Shared.GetCheckedAbsolutePath(fmt.Sprintf(
        "%s/%s",
        Shared.IntermediateDir, mod.GetRelativeModuleDir(),
    ))
    if !Shared.DoesAbsoluteDirExist(absTargetDir) {
        panic(fmt.Sprintf("absTargetDir does not exist [%s].", absTargetDir))
    }

    Shared.CopyOnlyChangedFiles(absSourceDir, absTargetDir, true)

    return
}

func FixVcsForRider() {
    if Shared.DoesRelativeFileExist(fmt.Sprintf("%s/.idea/vcs.xml", Shared.SolutionDirOut)) {
        return
    }

    fmt.Println("Fixing vcs for Rider ...")

    var absfile string = Shared.GetAbsolutePathNoCheck(fmt.Sprintf(
        "%s/.idea/.idea.Jafg/.idea/vcs.xml",
        Shared.SolutionDirOut,
    ))
    Shared.CheckAbsoluteFile(absfile)

    var file *os.File = Shared.OpenAbsoluteFile(absfile, true, os.O_RDWR)
    Shared.WriteToFile(file, `<?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  <component name="VcsDirectoryMappings">
    <mapping directory="$PROJECT_DIR$/../.." vcs="Git" />
  </component>
</project>
`)
    Shared.CloseFile(file)

    return
}
