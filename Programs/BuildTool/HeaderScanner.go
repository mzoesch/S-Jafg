// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "fmt"
    "os"
    "strings"
)

type CurrentHeaderState struct {
    filename string
    builder  *strings.Builder
}

func (chs *CurrentHeaderState) AppendString(str string) {
    if chs.builder.Len() == 0 {
        chs.builder = new(strings.Builder)

        chs.builder.WriteString(GeneratedHeaderFileStub)
        chs.builder.WriteString(GetHeaderGuardForModule2(GBuildInfo.ModuleName, GCurrentHeaderState.filename))
        chs.builder.WriteString(HeaderGuardPostfixStub)
    }

    chs.builder.WriteString(str)
    return
}

func (chs *CurrentHeaderState) AppendStringNoSideEffects(str string) {
    if chs.builder.Len() == 0 {
        chs.builder = new(strings.Builder)
    }

    chs.builder.WriteString(str)
    return
}

var GCurrentHeaderState *CurrentHeaderState = nil

func RecursivelyScanAndOperateOnHeaders(absolutePaths []string) {
    for _, path := range absolutePaths {
        if !Shared.IsHeaderFile(path) {
            panic(fmt.Sprintf("File [%s] is not a header file.", path))
        }

        var content string = Shared.ReadFileContentsFromAbsolutePath(path)
        OperateOnHeaderFile(Shared.GetFileNameFromHeaderPath(path), &content)

        continue
    }

    return
}

// OperateOnHeaderFile tokenizes the content of a header file and generates the .generated.h file if necessary.
// Will delete the old .generated.h file if it exists and no new one is needed.
func OperateOnHeaderFile(fileName string, content *string) {
    GCurrentHeaderState = new(CurrentHeaderState)
    GCurrentHeaderState.filename = fileName
    GCurrentHeaderState.builder = new(strings.Builder)

    var tokens []Token = TokenizeContent(content)

    if len(tokens) == 0 {
        ConditionallyWriteGeneratedHeaderFile()
        return
    }

    for _, token := range tokens {
        if token.Kind.IsInvalid() {
            panic("Invalid token kind.")
        }

        if token.Kind.IsPragma() {
            ExecutePragmaStatement(token.Description)
            continue
        }

        panic("Unknown token kind.")
    }

    ConditionallyWriteGeneratedHeaderFile()

    return
}

func ConditionallyWriteGeneratedHeaderFile() {
    var targetFile string = Shared.GetRelativeGeneratedHeaderPath(GCurrentHeaderState.filename)

    if GCurrentHeaderState.builder.Len() == 0 {
        Shared.DeleteRelativeFileIfExists(targetFile)
        return
    }

    var contentToWrite string = GCurrentHeaderState.builder.String()

    var file *os.File = Shared.OpenRelativeFile(targetFile, false, os.O_RDWR|os.O_CREATE)
    if Shared.IsFileAndStringEqual(file, contentToWrite, true) {
        fmt.Printf("Generated header file [%s] is up-to-date. Skipping re-write.\n", targetFile)
    } else {
        fmt.Printf("Generated header file [%s] is outdated. Writing new content.\n", targetFile)
        Shared.TruncateFile(file)
        Shared.SeekFileBeginning(file)
        Shared.WriteToFile(file, contentToWrite)
    }

    Shared.CloseFile(file)

    return
}

func ExecutePragmaStatement(pragmaStatement string) {
    switch pragmaStatement {
    case "IncludeAllModuleTests":
        IncludeAllModuleTests()
    case "MakeBuildFile":
        MakeBuildFile()
    default:
        panic(fmt.Sprintf("Unknown pragma statement [%s].", pragmaStatement))
    }
}

func IncludeAllModuleTests() {
    fmt.Println("Including all module tests ...")

    for _, mod := range Shared.GApp.GetAllModules() {
        if !mod.HasValidTestIncludeFile() {
            continue
        }

        GCurrentHeaderState.AppendString(fmt.Sprintf("#include \"Test/%s\"\n", mod.GetPredictedTestIncludeFileName()))

        continue
    }

    return
}

func MakeBuildFile() {
    fmt.Println("Making build file ...")

    var content string = GenerateWorkspaceWideBuildHeaderFile()
    GCurrentHeaderState.AppendStringNoSideEffects(content)

    return
}
