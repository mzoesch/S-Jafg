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

    return
}

func FixAfxForModule(mod *Shared.Module) {
    var afxFile string = Shared.GetCheckedAbsolutePath(fmt.Sprintf(
        "%s/%s.vcxproj",
        mod.GetRelativeModuleDir(),
        mod.GetUsableName(),
    ))

    FixAfxForFile(afxFile)

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
