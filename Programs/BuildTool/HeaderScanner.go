// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "bytes"
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

func (chs *CurrentHeaderState) AppendTranslationStringNoGuard(str string) {
    if chs.translationBuilder.Len() == 0 {
        chs.translationBuilder.WriteString(GeneratedTranslationFileStub)
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
        ScanForObjectStructureOnFile(path, Shared.GetFileNameFromHeaderPath(path), &content)

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
func ScanForObjectStructureOnFile(absPath string, fileName string, content *string) {
    GCurrentHeaderState = new(CurrentHeaderState)
    GCurrentHeaderState.filename = fileName
    GCurrentHeaderState.absoluteFilePath = Shared.NormalizePath(absPath)

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
    case "MakeVirtualFilesystem":
        break
    case "MakeStaticClassContainer":
        break
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
    case "MakeVirtualFilesystem":
        if strings.Contains(strings.ToLower(GBuildInfo.Platform), "wasm") {
            MakeVirtualFilesystem()
        }
    case "MakeStaticClassContainer":
        if strings.Contains(strings.ToLower(GBuildInfo.Platform), "wasm") {
            MakeStaticClassContainerForNonSharePlatforms()
        }
    default:
        panic(fmt.Sprintf("Unknown pragma statement [%s].", pragmaStatement))
    }

    return out
}

func ExecuteJafgClassStatement(jafgClassName string, jafgClassSuperName string, namespaces []string) {
    fmt.Printf("Found Jafg class [%s] with super class [%s].\n", jafgClassName, jafgClassSuperName)

    var filenameRelToPublicDir string;
    var pubIdx int = strings.Index(GCurrentHeaderState.absoluteFilePath, "/Public")
    if pubIdx == -1 {
        var privIdx int = strings.Index(GCurrentHeaderState.absoluteFilePath, "/Private")
        if privIdx == -1 {
            panic(fmt.Sprintf("File [%s] is not in a Public or Private directory.", GCurrentHeaderState.absoluteFilePath))
        }
        var filenameRelToPrivateDir string = GCurrentHeaderState.absoluteFilePath[privIdx+9:]
        filenameRelToPublicDir = fmt.Sprintf("../Private/%s", filenameRelToPrivateDir)
    } else {
        filenameRelToPublicDir = GCurrentHeaderState.absoluteFilePath[pubIdx+8:]
    }

    if GObjectStructure.AddNewObjectNode(jafgClassName, jafgClassSuperName, namespaces, filenameRelToPublicDir) {
        return
    }

    GObjectStructure.AddDeferredObjectNode(jafgClassName, jafgClassSuperName, namespaces, filenameRelToPublicDir)

    return
}

func IncludeAllModuleTests() {
    fmt.Println("Including all module tests ...")

    for _, mod := range Shared.GApp.GetAllModules() {
        if !mod.HasValidTestIncludeFile() {
            continue
        }

        GCurrentHeaderState.AppendString(
            fmt.Sprintf("#include \"../../%s/Source/Test/%s\"\n",
            mod.GetRelativeModuleDir(),
            mod.GetPredictedTestIncludeFileName()),
        )

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

    var filenameRelToPublicDir string;
    var pubIdx int = strings.Index(GCurrentHeaderState.absoluteFilePath, "/Public")
    if pubIdx == -1 {
        var privIdx int = strings.Index(GCurrentHeaderState.absoluteFilePath, "/Private")
        if privIdx == -1 {
            panic(fmt.Sprintf("File [%s] is not in a Public or Private directory.", GCurrentHeaderState.absoluteFilePath))
        }
        var filenameRelToPrivateDir string = GCurrentHeaderState.absoluteFilePath[privIdx+9:]
        filenameRelToPublicDir = fmt.Sprintf("../Private/%s", filenameRelToPrivateDir)
    } else {
        filenameRelToPublicDir = GCurrentHeaderState.absoluteFilePath[pubIdx+8:]
    }

    GObjectStructure.AddNewObjectNode(
        objBaseClass.JafgClassName,
        objBaseClass.JafgClassSuperName,
        CalculateCurrentNamespaces(tokens, index),
        filenameRelToPublicDir,
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

#define DECLARE_JAFG_CLASS(...) \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        %s, /* MyClassName          */                                                 \
        %s, /* MyClassSpacedName    */                                                 \
        %s, /* SuperClassName       */                                                 \
        %s, /* SuperClassSpacedName */                                                 \
        ##__VA_ARGS__ /* ClassFlags */                                                 \
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

#if PLATFORM_SUPPORTS_SHARED_LIBRARIES
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION(
        %s, /* MyClassName          */
        %s, /* MyClassSpacedName    */
        %s, /* MyClassNamespaces    */
        %s, /* SuperClassName       */
        %s  /* SuperClassSpacedName */
    )
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */

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

func MakeVirtualFilesystem() {
    fmt.Println("Making virtual filesystem ...")

    var absFiles []string = Shared.RecursivelyGetAllFilesInRelativeDir(Shared.ContentDir)

    GCurrentHeaderState.translationBuilder.WriteString(GeneratedTranslationFileStub)
    GCurrentHeaderState.translationBuilder.WriteString(TranslationStubPostFixStub)

    for _, file := range absFiles {
        if strings.Contains(file, ".gitignore") {
            continue
        }
        if strings.Contains(file, ".gitkeep") {
            continue
        }

        WriteEmbeddedFile(file)

        continue
    }

    GCurrentHeaderState.AppendString("\n")
    GCurrentHeaderState.AppendTranslationString("\n")

    return
}

func WriteEmbeddedFile(absFile string) {
    content, err := os.ReadFile(absFile);
    if err != nil {
        panic(err)
    }

    var indices int = 0;
    var buffer bytes.Buffer = bytes.Buffer{}
    for i, b := range content {
        indices++
        buffer.WriteString(fmt.Sprintf("0x%02X", b))
        if i < len(content)-1 {
            buffer.WriteString(", ")
        }

       continue
    }

    var relFile string = strings.ReplaceAll(absFile, "\\", "/")
    var contentIdx int = strings.Index(relFile, Shared.ContentDir)
    relFile = relFile[contentIdx:]

    var formattedFileName string = strings.ReplaceAll(absFile, "\\", "/")
    formattedFileName = strings.ReplaceAll(formattedFileName, ":", "_")
    formattedFileName = strings.ReplaceAll(formattedFileName, "/", "__")
    formattedFileName = strings.ReplaceAll(formattedFileName, ".", "_")

    GCurrentHeaderState.AppendString(fmt.Sprintf(`
ENGINE_API extern const uint8* ___FILE_EXTERN___%s;
inline Jafg::Private::LVirtualFile __%s(Jafg::LPath("%s"), ___FILE_EXTERN___%s, %d);`,
        formattedFileName,
        formattedFileName, relFile, formattedFileName, indices,
    ))

    GCurrentHeaderState.AppendTranslationString(fmt.Sprintf(`
namespace
{
const uint8 ___%s[] = { %s };
} /* ~Namespace <Anonymous> */
ENGINE_API const uint8* ___FILE_EXTERN___%s = ___%s;`,
        formattedFileName, buffer.String(), formattedFileName, formattedFileName,
    ))

    return
}

// MakeStaticClassContainerForNonSharePlatforms generates an unifier off all translation unit registrations that
// were automatically generated by the build tool. When building for non-shared platforms (static only), there is
// no way of implementing a shared vector for all translation unit registrations. This is a workaround to make
// sure that all translation unit registrations are included in the final binary.
func MakeStaticClassContainerForNonSharePlatforms() {
    fmt.Println("Making static class container ...")

    if GObjectStructure.Root == nil {
        panic("Root object not found.")
    }

    if !strings.Contains(strings.ToLower(GBuildInfo.Platform), "wasm") {
        return
    }

    GCurrentHeaderState.AppendTranslationStringNoGuard(`
#include "CoreAfx.h"
#include "Engine/ObjectBase.h"

/*-----------------------------------------------------------------------------
    BEGIN Generated translation content.
-----------------------------------------------------------------------------*/

`,
    )

    GCurrentHeaderState.AppendTranslationString("#if !PLATFORM_SUPPORTS_SHARED_LIBRARIES\n\n")
    MakeStaticClassContainerForNonSharePlatformsImpl(GObjectStructure.Root, GObjectStructure.Root)
    GCurrentHeaderState.AppendTranslationString("\n#endif /* !PLATFORM_SUPPORTS_SHARED_LIBRARIES */\n")

    return
}

func MakeStaticClassContainerForNonSharePlatformsImpl(Parent *ObjectNode, Object *ObjectNode) {
    if Parent == nil {
        panic("Parent is nil.")
    }
    if Object == nil {
        return
    }

    var fullSpacedName string = Object.GetComparableName()
    var concatedNamespaces string = ConcatNamespaces(Object.Namespaces)
    var superName string = Parent.Name
    var fullSuperSpacedName string = Parent.GetComparableName()

    GCurrentHeaderState.AppendTranslationString(fmt.Sprintf(`
/*-----------------------------------------------------------------------------
    %s
-----------------------------------------------------------------------------*/
#include "%s"
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
        fullSpacedName,
        Object.RelHeader,
        fullSpacedName, "UNKNOWN", fullSuperSpacedName,

        Object.Name,
        fullSpacedName,
        concatedNamespaces,
        superName,
        fullSuperSpacedName,
    ))

    for idx, _ := range Object.Children {
        var child *ObjectNode = &Object.Children[idx]
        MakeStaticClassContainerForNonSharePlatformsImpl(Object, child)
    }

    return
}
