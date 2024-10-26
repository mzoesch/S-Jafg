// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "fmt"
    "os"
    "path/filepath"
    "strings"
)

type CurrentHeaderState struct {
    filename           string
    absoluteFilePath   string
    builder            *strings.Builder
    translationBuilder *strings.Builder
}

var GCurrentHeaderState *CurrentHeaderState = nil

func (chs *CurrentHeaderState) AppendString(str string) {
    if chs.builder.Len() == 0 {
        chs.builder.WriteString(GeneratedHeaderFileStub)
        chs.builder.WriteString(GetHeaderGuardForModule2(GBuildInfo.ModuleName, GCurrentHeaderState.filename))
        chs.builder.WriteString(HeaderGuardPostfixStub)
    }

    chs.builder.WriteString(str)
    return
}

func (chs *CurrentHeaderState) AppendStringNoSideEffects(str string) {
    chs.builder.WriteString(str)
    return
}

func (chs *CurrentHeaderState) AppendTranslationString(str string) {
    if chs.translationBuilder.Len() == 0 {
        chs.translationBuilder.WriteString(GeneratedTranslationFileStub)
        chs.translationBuilder.WriteString(GetTranslationImportStub(GCurrentHeaderState.filename, true))
        chs.translationBuilder.WriteString(TranslationStubPostFixStub)
    }

    chs.translationBuilder.WriteString(str)
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
        OperateOnHeaderFileForGeneratedHeaders(path, Shared.GetFileNameFromHeaderPath(path), &content)

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
// file if necessary. Will delete the old .generated.h file if it exists and no new one is needed.
func OperateOnHeaderFileForGeneratedHeaders(absoluteFilePath string, fileName string, content *string) {
    GCurrentHeaderState = new(CurrentHeaderState)
    GCurrentHeaderState.filename = fileName
    GCurrentHeaderState.absoluteFilePath = Shared.NormalizePath(absoluteFilePath)
    GCurrentHeaderState.builder = new(strings.Builder)
    GCurrentHeaderState.translationBuilder = new(strings.Builder)

    var tokens []Token = TokenizeContent(content)

    if len(tokens) == 0 {
        ConditionallyWriteGeneratedFiles()
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
            GenerateObjectBodyForClass(token)
            continue
        }

        if token.Kind.IsPushNamespace() || token.Kind.IsPopNamespace() {
            continue
        }

        panic("Unknown token kind.")
    }

    ConditionallyWriteGeneratedFiles()

    return
}

func ConditionallyWriteGeneratedFiles() {
    ConditionallyWriteGeneratedHeaderFile()
    ConditionallyWriteGeneratedTranslationFile()
    return
}

func ConditionallyWriteGeneratedHeaderFile() {
    var targetFile string = Shared.GetRelativeGeneratedHeaderPath(GCurrentHeaderState.filename)

    if GCurrentHeaderState.builder.Len() == 0 {
        Shared.DeleteRelativeFileIfExists(targetFile)
        return
    }

    Shared.CheckRelativeFile(targetFile)

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

func ConditionallyWriteGeneratedTranslationFile() {
    var targetFile string = Shared.GetRelativeGeneratedTranslationPath(
        GBuildInfo.GetRelativeModuleDir() + "/" + GCurrentHeaderState.filename,
    )

    if GCurrentHeaderState.translationBuilder.Len() == 0 {
        Shared.DeleteRelativeFileIfExists(targetFile)
        return
    }

    Shared.CheckRelativeDir(filepath.Dir(targetFile))
    Shared.CheckRelativeFile(targetFile)

    var contentToWrite string = GCurrentHeaderState.translationBuilder.String()

    var file *os.File = Shared.OpenRelativeFile(targetFile, false, os.O_RDWR|os.O_CREATE)
    if Shared.IsFileAndStringEqual(file, contentToWrite, true) {
        fmt.Printf("Generated translation file [%s] is up-to-date. Skipping re-write.\n", targetFile)
    } else {
        fmt.Printf("Generated translation file [%s] is outdated. Writing new content.\n", targetFile)
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

func GenerateObjectBodyForClass(token Token) {
    if token.JafgClassSuperName == "" {
        if GObjectStructure.Root == nil {
            panic(fmt.Sprintf("Object [%s] is the root object. But super was not found.", token.JafgClassName))
        }
        if GObjectStructure.Root.Name != token.JafgClassName {
            panic(fmt.Sprintf("Object [%s] is not the root object. But super was not found.", token.JafgClassName))
        }

        token.JafgClassSuperName = token.JafgClassName
        token.JafgClassSuperNamespaces = token.JafgClassNamespaces
    }

    var fullSpacedName string = ConcatNameWithNamespace(token.JafgClassName, token.JafgClassNamespaces)
    var concatedNamespaces string = ConcatNamespaces(token.JafgClassNamespaces)
    var fullSuperSpacedName string = ConcatNameWithNamespace(token.JafgClassSuperName, token.JafgClassSuperNamespaces)

    GCurrentHeaderState.AppendString(fmt.Sprintf(`
#ifdef GENERATED_CLASS_BODY
    #undef GENERATED_CLASS_BODY
#endif /* GENERATED_CLASS_BODY */

#define GENERATED_CLASS_BODY()                         \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_BODY_IMPL( \
        %s, /* MyClassName          */                 \
        %s, /* MyClassSpacedName    */                 \
        %s, /* SuperClassName       */                 \
        %s  /* SuperClassSpacedName */                 \
    )

#ifdef DECLARE_JAFG_CLASS
    #undef DECLARE_JAFG_CLASS
#endif /* DECLARE_JAFG_CLASS */

#define DECLARE_JAFG_CLASS() \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        %s, /* MyClassName          */                                                 \
        %s, /* MyClassSpacedName    */                                                 \
        %s, /* SuperClassName       */                                                 \
        %s  /* SuperClassSpacedName */                                                 \
    )

`,
        token.JafgClassName,
        fullSpacedName,
        token.JafgClassSuperName,
        fullSuperSpacedName,

        token.JafgClassName,
        fullSpacedName,
        token.JafgClassSuperName,
        fullSuperSpacedName,
    ))

    GCurrentHeaderState.AppendTranslationString(fmt.Sprintf(`
#ifndef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION
    #error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION not defined."
    #error "Missing #include \"Engine/ObjectBase.h\". This should be transitively included by the derived class [%s] inside"
    #error "project [%s]. Is super class [%s] not deriving from any subclass of ObjectBase? It must."
#endif /* PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION */

PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION(
    %s, /* MyClassName          */
    %s, /* MyClassSpacedName    */
    %s, /* MyClassNamespaces    */
    %s, /* SuperClassName       */
    %s  /* SuperClassSpacedName */
)

`,
        fullSpacedName, GBuildInfo.ModuleName, fullSuperSpacedName,

        token.JafgClassName,
        fullSpacedName,
        concatedNamespaces,
        token.JafgClassSuperName,
        fullSuperSpacedName,
    ))

    return
}
