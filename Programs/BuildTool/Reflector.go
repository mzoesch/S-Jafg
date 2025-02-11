// Copyright mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Core"
    "Jafg/Shared"
    "fmt"
    "strconv"
    "strings"
)

func ConvertNameToCppValidDefine(name string) string {
    var out string = name
    out = strings.ReplaceAll(out, "~", "")
    out = strings.ReplaceAll(out, "/", "___")
    out = strings.ReplaceAll(out, ".", "__")
    out = strings.ReplaceAll(out, "-", "_")
    return out
}

func RemoveAllNamespaces(name string) string {
    var out string = name
    var lastColon int = strings.LastIndex(out, "::")
    if lastColon != -1 {
        out = out[lastColon+2:]
    }
    return out
}

func ConvertPacketWrapperPathToGhPath(relPath string) string {
    if strings.HasPrefix(relPath, "~") == false {
        panic("RelPath does not start with ~.")
    }
    relPath = relPath[1:]
    relSource := GBuildTargetInfo.GetRelativeSourceDir()
    if !strings.Contains(relPath, relSource) {
        panic(fmt.Sprintf("RelPath [%s] is not a subpath of [%s].", relPath, relSource))
    }

    var lastSlash int = strings.LastIndex(relPath, "/")
    if lastSlash == -1 {
        panic("RelPath does not contain a slash.")
    }

    if strings.HasSuffix(relPath, ".h") == false {
        panic("RelPath does not end with [.h].")
    }

    return GBuildTargetInfo.GetRelativeModuleGhDir() + "/" + relPath[lastSlash+1:len(relPath)-2] + Core.GhExtension
}

func ConvertPacketWrapperPathToGtPath(relPath string) string {
    if strings.HasPrefix(relPath, "~") == false {
        panic("RelPath does not start with ~.")
    }
    relPath = relPath[1:]
    relSource := GBuildTargetInfo.GetRelativeSourceDir()
    if !strings.Contains(relPath, relSource) {
        panic(fmt.Sprintf("RelPath [%s] is not a subpath of [%s].", relPath, relSource))
    }

    var lastSlash int = strings.LastIndex(relPath, "/")
    if lastSlash == -1 {
        panic("RelPath does not contain a slash.")
    }

    if strings.HasSuffix(relPath, ".h") == false {
        panic("RelPath does not end with [.h].")
    }

    return GBuildTargetInfo.GetRelativeModuleGtDir() + "/" + relPath[lastSlash+1:len(relPath)-2] + Core.GtExtension
}

func ReflectModule() {
    if GBuildTargetInfo == nil {
        panic("BuildTargetInfo not initialized.")
    }

    var SearchDir string = GBuildTargetInfo.GetRelativeSourceDir()
    var SearchDirInternal string = SearchDir + "/Internal"
    var SearchDirPublic string = SearchDir + "/Public"
    var filesAbs []string
    if Shared.DoesRelativeDirExist(SearchDirInternal) {
        var filesAbsInternal []string = Shared.GetAllFilesInRelativeDirRecursive(SearchDir + "/Internal")
        filesAbs = append(filesAbs, filesAbsInternal...)
    }
    if Shared.DoesRelativeDirExist(SearchDirPublic) {
        var filesAbsPublic []string = Shared.GetAllFilesInRelativeDirRecursive(SearchDir + "/Public")
        filesAbs = append(filesAbs, filesAbsPublic...)
    }

    var hFilesRel []string
    for _, fAbs := range filesAbs {
        if Shared.IsHeaderFile(fAbs) == false {
            continue
        }
        var fRel string = Shared.ToRelativePath(Shared.NormalizePath(fAbs))
        hFilesRel = append(hFilesRel, "~"+fRel)
        continue
    }

    var oldNum int = len(GJPacketContainer.Wrappers)

    // Remove deleted files from the cache for the current module only.
    var newWrappers []JPacketWrapper = make([]JPacketWrapper, 0)
    for idxW, _ := range GJPacketContainer.Wrappers {
        if strings.HasPrefix(GJPacketContainer.Wrappers[idxW].Name, GBuildTargetInfo.Module) == false {
            newWrappers = append(newWrappers, GJPacketContainer.Wrappers[idxW])
            continue
        }
        var bFound bool = false
        for idxF, _ := range hFilesRel {
            if GJPacketContainer.Wrappers[idxW].Name == hFilesRel[idxF] {
                bFound = true
                break
            }
        }
        if bFound {
            newWrappers = append(newWrappers, GJPacketContainer.Wrappers[idxW])
        }
    }
    GJPacketContainer.Wrappers = newWrappers
    if len(GJPacketContainer.Wrappers) != oldNum {
        fmt.Printf("Found %d cached wrappers of interest but %d are now considered relevant.\n", oldNum, len(GJPacketContainer.Wrappers))
    } else {
        fmt.Printf("Found %d cached wrappers of interest and all are still relevant.\n", len(GJPacketContainer.Wrappers))
    }

    var newReflectedFiles []JPacketWrapper
    for idx, _ := range hFilesRel {
        ReflectFile(hFilesRel[idx], &newReflectedFiles)
    }

    fmt.Printf("From %d files, %d were interesting based of reflection.\n", len(hFilesRel), len(newReflectedFiles))

    for _, packetWrapper := range newReflectedFiles {
        ConditionallyWritePacketToOut(packetWrapper)
    }

    return
}

func ReflectFile(fRel string, newReflectedFiles *[]JPacketWrapper) {
    if fRel == "~Engine/Engine/Source/Public/Engine/ObjectMacros.h" {
        // Very, very special file that declares a lot of stuff. But sadly confuses our tokenizer...
        // It would take simply too much time to implement special rules that are commonly accepted over all files
        // so that false tokenizing does not occur. But we do not really need that - just makes everything slower and
        // more complicated. So we just hardcode the ignore here.
        return
    }

    var bAddedNew bool = false
    var packetWrapper *JPacketWrapper = nil
    for idx, _ := range GJPacketContainer.Wrappers {
        if GJPacketContainer.Wrappers[idx].Name == fRel {
            packetWrapper = &GJPacketContainer.Wrappers[idx]
            break
        }
    }
    if packetWrapper == nil {
        bAddedNew = true
        packetWrapper = new(JPacketWrapper)
    }

    var tokens []Token = Tokenize(fRel, Shared.ReadRelativeFile(fRel[1:]))

    for i, t := range tokens {
        if t.Type.IsPragma() {
            ExecuteJafgPragma(tokens, i)
        } else if t.Type.IsDeclareClass() {
            AddJafgClassToPacket(tokens, i, TOKEN_DECLARE_CLASS, packetWrapper)
        } else if t.Type.IsDeclareWidget() {
            AddJafgClassToPacket(tokens, i, TOKEN_DECLARE_WIDGET, packetWrapper)
        } else if t.Type.IsDeclareWidgetWithFactory() {
            AddJafgClassToPacket(tokens, i, TOKEN_DECLARE_WIDGET_WITH_FACTORY, packetWrapper)
        } else if t.Type.IsGeneratedClassBody() {
            AddJafgClassGeneratedBodyToPacket(tokens, i, packetWrapper)
        }

        continue
    }

    if bAddedNew {
        if len(packetWrapper.Packets) > 0 {
            packetWrapper.Name = fRel
            *newReflectedFiles = append(*newReflectedFiles, *packetWrapper)
        }
    }

    return
}

// ExecuteJafgPragma executes a pragma token.
// Valid pragma tokens are:
//   "IncludeAllModuleTests"
//   "NextIsObjectBaseClass"
//   "MakeVirtualFilesystem"
//   "MakeStaticClassContainer"
func ExecuteJafgPragma(tokens []Token, idx int) {
    if tokens[idx].Type != TOKEN_PRAGMA {
        panic(fmt.Sprintf("Expected a pragma token at {%d}.", idx))
    }

    var t *Token = &tokens[idx]

    if !(t.Content == "\"IncludeAllModuleTests\"" || t.Content == "\"NextIsObjectBaseClass\"" ||
        t.Content == "\"MakeVirtualFilesystem\"" || t.Content == "\"MakeStaticClassContainer\"") {
        panic(fmt.Sprintf("Unknown pragma [%s].", t.Content))
    }

    // ...

    return
}

func AddJafgClassToPacket(tokens []Token, idx int, tokenTy ETokenType, packetWrapper *JPacketWrapper) {
    var packet JPacket = JPacket{}

    var t *Token = &tokens[idx]
    if t.Type != tokenTy {
        panic(fmt.Sprintf("Expected any class declaration token at {%d}.", idx))
    }

    var tGeneratedBodyIdx int = FindNextToken(&tokens, idx, TOKEN_GENERATED_CLASS_BODY)
    if tGeneratedBodyIdx == -1 {
        panic(fmt.Sprintf("Expected a generated class body token after {%d}.", idx))
    }
    var tGeneratedBody Token = tokens[tGeneratedBodyIdx]
    if tGeneratedBody.Line < t.Line {
        panic(fmt.Sprintf("Expected a generated class body token after {%d}.", idx))
    }

    var namespaces Shared.Stack[string] = Shared.NewStack[string]()
    for _, t := range tokens {
        if t.Line > tGeneratedBody.Line {
            break
        }
        if t.Type == TOKEN_PUSHNS {
            namespaces.Push(t.Content)
        } else if t.Type == TOKEN_POPNS {
            namespaces.Pop()
        }
    }
    var namespaceStr string = ""
    for _, ns := range namespaces.Data {
        namespaceStr += ns + "::"
    }
    if namespaceStr == "" {
        namespaceStr = "::"
    }
    if strings.HasPrefix(namespaceStr, "::") == false {
        namespaceStr = "::" + namespaceStr
    }

    if tokenTy == TOKEN_DECLARE_CLASS {
        packet.Callback = OnBuildJafgClassDeclaration
    } else if tokenTy == TOKEN_DECLARE_WIDGET {
        packet.Callback = OnBuildJafgWidgetDeclaration
    } else if tokenTy == TOKEN_DECLARE_WIDGET_WITH_FACTORY {
        packet.Callback = OnBuildJafgWidgetWithFactoryDeclaration
    }
    packet.Name = t.Content
    packet.Line = t.Line
    packet.Args = append(packet.Args, t.Info[0])
    packet.Args = append(packet.Args, namespaceStr)

    packetWrapper.Packets = append(packetWrapper.Packets, packet)

    return
}

func AddJafgClassGeneratedBodyToPacket(tokens []Token, idx int, packetWrapper *JPacketWrapper) {
    var packet JPacket = JPacket{}

    var t *Token = &tokens[idx]
    if t.Type != TOKEN_GENERATED_CLASS_BODY {
        panic(fmt.Sprintf("Expected a generated class body token at {%d}.", idx))
    }

    var allowedTokens []ETokenType = []ETokenType{TOKEN_DECLARE_CLASS, TOKEN_DECLARE_WIDGET, TOKEN_DECLARE_WIDGET_WITH_FACTORY}
    tClassDeclIdx, tClassToken := FindMostPreviousToken(&tokens, idx, allowedTokens)
    if tClassDeclIdx == -1 {
        panic(fmt.Sprintf("Excected a class declaration token before {%d}.", idx))
    }

    var tClassDecl Token = tokens[tClassDeclIdx]
    if tClassDecl.Line > t.Line {
        panic(fmt.Sprintf("Excected a class declaration token before {%d}.", idx))
    }

    var namespaces Shared.Stack[string] = Shared.NewStack[string]()
    for _, tInner := range tokens {
        if tInner.Line > t.Line {
            break
        }
        if tInner.Type == TOKEN_PUSHNS {
            namespaces.Push(tInner.Content)
        } else if tInner.Type == TOKEN_POPNS {
            namespaces.Pop()
        }
    }
    var namespaceStr string = ""
    for _, ns := range namespaces.Data {
        namespaceStr += ns + "::"
    }
    if namespaceStr == "" {
        namespaceStr = "::"
    }
    if strings.HasPrefix(namespaceStr, "::") == false {
        namespaceStr = "::" + namespaceStr
    }

    if tClassToken == TOKEN_DECLARE_CLASS {
        packet.Callback = OnBuildJafgClassBody
    } else if tClassToken == TOKEN_DECLARE_WIDGET {
        packet.Callback = OnBuildJafgWidgetBody
    } else if tClassToken == TOKEN_DECLARE_WIDGET_WITH_FACTORY {
        packet.Callback = OnBuildJafgWidgetWithFactoryBody
    } else {
        panic("Unknown class token.")
    }
    packet.Name = tClassDecl.Content
    packet.Line = t.Line
    packet.Args = append(packet.Args, tClassDecl.Info[0])
    packet.Args = append(packet.Args, strconv.Itoa(tClassDecl.Line))
    packet.Args = append(packet.Args, namespaceStr)

    packetWrapper.Packets = append(packetWrapper.Packets, packet)

    return
}

func OnBuildJafgClassDeclaration(hFileId string, bH *strings.Builder, bT *strings.Builder, packet JPacket) {
    if bH == nil {
        panic("Builder for header is nil.")
    }
    if bT == nil {
        panic("Builder for translation is nil.")
    }

    bH.WriteString(fmt.Sprintf(`
#ifdef %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION
    #error "Generated packet [%s] included multiple times. Missing #pragma once or #ifndef guard?"
#endif /* %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION */
#define %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION(...)              \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        /* My Class Name */          %s, /* ORIGIN VALUE: %s */                                \
        /* My Class Spaces */        %s,                                                       \
        /* Super Class Name */       %s, /* ORIGIN VALUE: %s */                                \
        /* Line */                   %d,                                                       \
        /* Additional Class Flags */ __VA_ARGS__                                               \
    )
`,
        hFileId, packet.Line, packet.Name, hFileId, packet.Line, hFileId, packet.Line,

        RemoveAllNamespaces(packet.Name), packet.Name,
        packet.Args[1],
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],
        packet.Line,
    ))

    bT.WriteString(fmt.Sprintf(`
#if PLATFORM_SUPPORTS_SHARED_LIBRARIES
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION( \
        /* My Class Name */          %s, /* ORIGIN VALUE: %s */                               \
        /* My Class Spaces */        %s,                                                      \
        /* Line Of Declaration */    %d,                                                      \
        /* Super Class Name */       %s  /* ORIGIN VALUE: %s */                               \
    )
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */
`,
        RemoveAllNamespaces(packet.Name), packet.Name,
        packet.Args[1],
        packet.Line,
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],
    ))

    return
}

func OnBuildJafgWidgetDeclaration(hFileId string, bH *strings.Builder, bT *strings.Builder, packet JPacket) {
    if bH == nil {
        panic("Builder for header is nil.")
    }
    if bT == nil {
        panic("Builder for translation is nil.")
    }

    bH.WriteString(fmt.Sprintf(`
#ifdef %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION
    #error "Generated packet [%s] included multiple times. Missing #pragma once or #ifndef guard?"
#endif /* %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION */
#define %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION(...)               \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        /* My Class Name */          %s, /* ORIGIN VALUE: %s */                                 \
        /* My Class Spaces */        %s,                                                        \
        /* Super Class Name */       %s, /* ORIGIN VALUE: %s */                                 \
        /* Line */                   %d,                                                        \
        /* Additional Class Flags */ __VA_ARGS__                                                \
    )
`,
        hFileId, packet.Line, packet.Name, hFileId, packet.Line, hFileId, packet.Line,

        RemoveAllNamespaces(packet.Name), packet.Name,
        packet.Args[1],
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],
        packet.Line,
    ))

    bT.WriteString(fmt.Sprintf(`
#if PLATFORM_SUPPORTS_SHARED_LIBRARIES
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION( \
        /* My Class Name */          %s, /* ORIGIN VALUE: %s */                               \
        /* My Class Spaces */        %s,                                                      \
        /* Line Of Declaration */    %d,                                                      \
        /* Super Class Name */       %s  /* ORIGIN VALUE: %s */                               \
    )
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */
`,
        RemoveAllNamespaces(packet.Name), packet.Name,
        packet.Args[1],
        packet.Line,
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],
    ))

    return
}

func OnBuildJafgWidgetWithFactoryDeclaration(hFileId string, bH *strings.Builder, bT *strings.Builder, packet JPacket) {
    if bH == nil {
        panic("Builder for header is nil.")
    }
    if bT == nil {
        panic("Builder for translation is nil.")
    }

    bH.WriteString(fmt.Sprintf(`
#ifdef %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION
    #error "Generated packet [%s] included multiple times. Missing #pragma once or #ifndef guard?"
#endif /* %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION */
#define %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION(TFactoryTy, ...)                \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        /* My Class Name */          %s, /* ORIGIN VALUE: %s */                                              \
        /* My Class Spaces */        %s,                                                                     \
        /* Super Class Name */       %s, /* ORIGIN VALUE: %s */                                              \
        /* Line */                   %d,                                                                     \
        /* Factory Type */           TFactoryTy,                                                             \
        /* Additional Class Flags */ __VA_ARGS__                                                             \
    )
`,
        hFileId, packet.Line, packet.Name, hFileId, packet.Line, hFileId, packet.Line,

        RemoveAllNamespaces(packet.Name), packet.Name,
        packet.Args[1],
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],
        packet.Line,
    ))

    bT.WriteString(fmt.Sprintf(`
#if PLATFORM_SUPPORTS_SHARED_LIBRARIES
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION( \
        /* My Class Name */          %s, /* ORIGIN VALUE: %s */                               \
        /* My Class Spaces */        %s,                                                      \
        /* Line Of Declaration */    %d,                                                      \
        /* Super Class Name */       %s  /* ORIGIN VALUE: %s */                               \
    )
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */
`,
        RemoveAllNamespaces(packet.Name), packet.Name,
        packet.Args[1],
        packet.Line,
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],
    ))

    return
}

func OnBuildJafgClassBody(hFileId string, bH *strings.Builder, bT *strings.Builder, packet JPacket) {
    if bH == nil {
        panic("Builder for header is nil.")
    }
    if bT == nil {
        panic("Builder for translation is nil.")
    }

    lineHelperConstruction, err := strconv.Atoi(packet.Args[1])
    if err != nil {
        panic(err)
    }

    bH.WriteString(fmt.Sprintf(`
#ifdef %s_%d_MY_GENERATED_CLASS_BODY
    #error "Generated packet [%s] included multiple times. Missing #pragma once or #ifndef guard?"
#endif /* %s_%d_MY_GENERATED_CLASS_BODY */
#define %s_%d_MY_GENERATED_CLASS_BODY(...)                        \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_IMPL(      \
        /* My Class Name */            %s, /* ORIGIN VALUE: %s */ \
        /* My Class Spaces */          %s,                        \
        /* Super Class Name */         %s, /* ORIGIN VALUE: %s */ \
        /* Construction Helper Line */ %d                         \
    )
`,
        hFileId, packet.Line, packet.Name, hFileId, packet.Line, hFileId, packet.Line,

        RemoveAllNamespaces(packet.Name), packet.Name,
        packet.Args[2],
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],
        lineHelperConstruction,
    ))

    return
}

func OnBuildJafgWidgetBody(hFileId string, bH *strings.Builder, bT *strings.Builder, packet JPacket) {
    if bH == nil {
        panic("Builder for header is nil.")
    }
    if bT == nil {
        panic("Builder for translation is nil.")
    }

    lineHelperConstruction, err := strconv.Atoi(packet.Args[1])
    if err != nil {
        panic(err)
    }

    bH.WriteString(fmt.Sprintf(`
#ifdef %s_%d_MY_GENERATED_CLASS_BODY
    #error "Generated packet [%s] included multiple times. Missing #pragma once or #ifndef guard?"
#endif /* %s_%d_MY_GENERATED_CLASS_BODY */
#define %s_%d_MY_GENERATED_CLASS_BODY(...)                        \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_BODY_IMPL(      \
        /* My Class Name */            %s, /* ORIGIN VALUE: %s */ \
        /* My Class Spaces */          %s,                        \
        /* Super Class Name */         %s, /* ORIGIN VALUE: %s */ \
        /* Construction Helper Line */ %d                         \
    )
`,
        hFileId, packet.Line, packet.Name, hFileId, packet.Line, hFileId, packet.Line,

        RemoveAllNamespaces(packet.Name), packet.Name,
        packet.Args[2],
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],
        lineHelperConstruction,
    ))

    return
}

func OnBuildJafgWidgetWithFactoryBody(hFileId string, bH *strings.Builder, bT *strings.Builder, packet JPacket) {
    if bH == nil {
        panic("Builder for header is nil.")
    }
    if bT == nil {
        panic("Builder for translation is nil.")
    }

    lineHelperConstruction, err := strconv.Atoi(packet.Args[1])
    if err != nil {
        panic(err)
    }

    bH.WriteString(fmt.Sprintf(`
#ifdef %s_%d_MY_GENERATED_CLASS_BODY
    #error "Generated packet [%s] included multiple times. Missing #pragma once or #ifndef guard?"
#endif /* %s_%d_MY_GENERATED_CLASS_BODY */
#define %s_%d_MY_GENERATED_CLASS_BODY(...)                        \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_BODY_IMPL(      \
        /* My Class Name */            %s, /* ORIGIN VALUE: %s */ \
        /* My Class Spaces */          %s,                        \
        /* Super Class Name */         %s, /* ORIGIN VALUE: %s */ \
        /* Construction Helper Line */ %d                         \
    )
`,
        hFileId, packet.Line, packet.Name, hFileId, packet.Line, hFileId, packet.Line,

        RemoveAllNamespaces(packet.Name), packet.Name,
        packet.Args[2],
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],
        lineHelperConstruction,
    ))

    return
}

func ConditionallyWritePacketToOut(packetWrapper JPacketWrapper) {
    if packetWrapper.Name == "" {
        panic("PacketWrapper.Name is empty.")
    }

    var uniqueFileId string = ConvertNameToCppValidDefine(packetWrapper.Name)
    GBuildTargetInfo.GetRelativeSourceDir()
    var relTargetGh string = ConvertPacketWrapperPathToGhPath(packetWrapper.Name)
    var relTargetGt string = ConvertPacketWrapperPathToGtPath(packetWrapper.Name)

    var outH strings.Builder
    var outT strings.Builder

    var hFileId string = "FILE_ID_" + uniqueFileId

    outH.WriteString(fmt.Sprintf(`
// Copyright mzoesch. All rights reserved.


/*-----------------------------------------------------------------------------
   This header file was generated by the Jafg build tool.
   Do not modify it manually.
-----------------------------------------------------------------------------*/

#ifdef PRIVATE_JAFG_GENERATED_HEADER_%s
    #error "Generated header [%s] included multiple times. Missing #pragma once or #ifndef guard?"
#endif /* PRIVATE_JAFG_GENERATED_HEADER_%s */
#define PRIVATE_JAFG_GENERATED_HEADER_%s

#ifdef JAFG_PRIVATE_FILE_ID
    #undef JAFG_PRIVATE_FILE_ID
#endif /* JAFG_PRIVATE_FILE_ID */
#define JAFG_PRIVATE_FILE_ID FILE_ID_%s


/*-----------------------------------------------------------------------------
   BEGIN Generated header content.
-----------------------------------------------------------------------------*/

`,
        uniqueFileId, relTargetGh, uniqueFileId, uniqueFileId,
        uniqueFileId,
    ))

    outT.WriteString(fmt.Sprintf(`
// Copyright mzoesch. All rights reserved.

/*-----------------------------------------------------------------------------
    This translation file was generated by the Jafg build tool.
    Do not modify it manually.
-----------------------------------------------------------------------------*/

#include "%s/%s/%s"

/*-----------------------------------------------------------------------------
    BEGIN Generated translation content.
-----------------------------------------------------------------------------*/

`,
        GBuildTargetInfo.GetSlnPointerChecked().GetChdirUpRelToBuildFile(),
        GBuildTargetInfo.GetModulePointerChecked().GetChdirUpRelToBuildFile(),
        packetWrapper.Name[1:],
    ))

    for _, packet := range packetWrapper.Packets {
        if packet.Callback == nil {
            panic(fmt.Sprintf("Callback from [%s] is invalid.", packet.Name))
        }
        packet.Callback(hFileId, &outH, &outT, packet)
    }

    Shared.OpenAndWriteToRelativeFileIfDifferent(relTargetGh, outH.String(), true)
    Shared.OpenAndWriteToRelativeFileIfDifferent(relTargetGt, outT.String(), true)

    return
}
