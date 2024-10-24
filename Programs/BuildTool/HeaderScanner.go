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

var GCurrentHeaderState *CurrentHeaderState = nil

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

func ScanAllHeadersForObjectStructure(absolutePaths []string) {
    for _, path := range absolutePaths {
        if !Shared.IsHeaderFile(path) {
            panic(fmt.Sprintf("File [%s] is not a header file.", path))
        }

        var content string = Shared.ReadFileContentsFromAbsolutePath(path)
        ScanForObjectStructureOnFile(Shared.GetFileNameFromHeaderPath(path), &content)

        continue
    }

    return
}

// RecursivelyScanAndOperateOnHeaders scans all headers that are due for regeneration.
// Will regenerate generated headers if they are out of date.
func RecursivelyScanAndOperateOnHeaders(absolutePaths []string) {
    for _, path := range absolutePaths {
        if !Shared.IsHeaderFile(path) {
            panic(fmt.Sprintf("File [%s] is not a header file.", path))
        }

        var content string = Shared.ReadFileContentsFromAbsolutePath(path)
        OperateOnHeaderFileForGeneratedHeaders(Shared.GetFileNameFromHeaderPath(path), &content)

        continue
    }

    return
}

// ScanForObjectStructureOnFile scans a header file for object structure information and caches it.
func ScanForObjectStructureOnFile(fileName string, content *string) {
    var tokens []Token = TokenizeContent(content)

    var tokensToSkip int = 0
    for idx, token := range tokens {
        if token.Kind.IsInvalid() {
            panic("Invalid token kind.")
        }

        if tokensToSkip > 0 {
            tokensToSkip--
            continue
        }

        if token.Kind.IsPragma() {
            tokensToSkip = ExecutePragmaStatementForObjectStructure(token.PragmaStatement, idx, &tokens)
            continue
        }

        if token.Kind.IsJafgClass() {
            ExecuteJafgClassStatement(
                token.JafgClassName,
                token.JafgClassSuperName,
                CalculateCurrentNamespaces(&tokens, idx),
            )
            continue
        }

        if token.Kind.IsPushNamespace() || token.Kind.IsPopNamespace() {
            continue
        }

        panic("Unknown token kind.")
    }

    return
}

// OperateOnHeaderFileForGeneratedHeaders tokenizes the content of a header file and generates the .generated.h
//file if necessary. Will delete the old .generated.h file if it exists and no new one is needed.
func OperateOnHeaderFileForGeneratedHeaders(fileName string, content *string) {
    GCurrentHeaderState = new(CurrentHeaderState)
    GCurrentHeaderState.filename = fileName
    GCurrentHeaderState.builder = new(strings.Builder)

    var tokens []Token = TokenizeContent(content)

    if len(tokens) == 0 {
        ConditionallyWriteGeneratedHeaderFile()
        return
    }

    var tokensToSkip int = 0
    for idx, token := range tokens {
        if token.Kind.IsInvalid() {
            panic("Invalid token kind.")
        }

        if tokensToSkip > 0 {
            tokensToSkip--
            continue
        }

        if token.Kind.IsPragma() {
            tokensToSkip = ExecutePragmaStatementForGeneratedHeaders(token.PragmaStatement, idx, &tokens)
            continue
        }

        if token.Kind.IsJafgClass() {
            continue
        }

        if token.Kind.IsPushNamespace() || token.Kind.IsPopNamespace() {
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

func ExecutePragmaStatementForObjectStructure(pragmaStatement string, tokenIndex int, tokens *[]Token) int /* How many tokens to skip. */ {
    var out int = 0

    switch pragmaStatement {
    case "IncludeAllModuleTests":
        break
    case "MakeBuildFile":
        break
    case "NextIsObjectBaseClass":
        out = RegisterObjectBaseClass(tokenIndex, tokens)
    default:
        panic(fmt.Sprintf("Unknown pragma statement [%s].", pragmaStatement))
    }

    return out
}

func ExecutePragmaStatementForGeneratedHeaders(pragmaStatement string, tokenIndex int, tokens *[]Token) int /* How many tokens to skip. */ {
    var out int = 0

    switch pragmaStatement {
    case "IncludeAllModuleTests":
        IncludeAllModuleTests()
    case "MakeBuildFile":
        MakeBuildFile()
    case "NextIsObjectBaseClass":
        break
    default:
        panic(fmt.Sprintf("Unknown pragma statement [%s].", pragmaStatement))
    }

    return out
}

func ExecuteJafgClassStatement(jafgClassName string, jafgClassSuperName string, namespaces []string) {
    fmt.Printf("Found Jafg class [%s] with super class [%s].\n", jafgClassName, jafgClassSuperName)

    if GObjectStructure.AddNewObjectNode(jafgClassName, jafgClassSuperName, namespaces) {
        return
    }

    GObjectStructure.AddDeferredObjectNode(jafgClassName, jafgClassSuperName, namespaces)

    return
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

func CalculateCurrentNamespaces(tokens *[]Token, index int) []string {
    var out []string

    for idx := 0; idx < index; idx++ {
        var token Token = (*tokens)[idx]

        if token.Kind.IsPushNamespace() {
            out = append(out, token.NamespaceName)
            continue
        }

        if token.Kind.IsPopNamespace() {
            out = out[:len(out)-1]
        }

        break
    }

    return out
}

func RegisterObjectBaseClass(index int, tokens *[]Token) int /* How many tokens to skip. */ {
    if len(*tokens) <= index+1 {
        panic("Object base class is illformed.")
    }

    var objBaseClass Token = (*tokens)[index+1]

    fmt.Printf("Registering object base class with name %s ...\n", objBaseClass.JafgClassName)

    if objBaseClass.JafgClassSuperName != "" {
        panic(fmt.Sprintf("Object base class [%s] has a super class [%s]. This is not allowed.",
            objBaseClass.JafgClassName, objBaseClass.JafgClassSuperName))
    }

    GObjectStructure.AddNewObjectNode(
        objBaseClass.JafgClassName,
        objBaseClass.JafgClassSuperName,
        CalculateCurrentNamespaces(tokens, index),
    )

    return 1
}
