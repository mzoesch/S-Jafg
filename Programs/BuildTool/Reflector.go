// Copyright mzoesch. All rights reserved.

package BuildTool

import (
	"Jafg/Core"
	"Jafg/Shared"
	"bytes"
	"fmt"
	"os"
	"path"
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

func RemoveDoubleColonGccShenanigansSuffix(name string) string {
    var out string = name
    var lastColon int = strings.LastIndex(out, "::")
    if lastColon != -1 {
        out = out[:lastColon]
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
            ExecuteJafgPragma(tokens, i, packetWrapper)
        } else if t.Type.IsDeclareClass() {
            AddJafgClassToPacket(tokens, i, TOKEN_DECLARE_CLASS, packetWrapper)
        } else if t.Type.IsDeclareWidget() {
            AddJafgClassToPacket(tokens, i, TOKEN_DECLARE_WIDGET, packetWrapper)
        } else if t.Type.IsDeclareWidgetWithFactory() {
            AddJafgClassToPacket(tokens, i, TOKEN_DECLARE_WIDGET_WITH_FACTORY, packetWrapper)
        } else if t.Type.IsGeneratedClassBody() {
            AddJafgClassGeneratedBodyToPacket(tokens, i, packetWrapper)
        } else if t.Type.IsClassField() {
            AddJafgClassFieldToPacket(tokens, i, packetWrapper)
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
func ExecuteJafgPragma(tokens []Token, idx int, packetWrapper *JPacketWrapper) {
    if tokens[idx].Type != TOKEN_PRAGMA {
        panic(fmt.Sprintf("Expected a pragma token at {%d}.", idx))
    }

    var t *Token = &tokens[idx]

    if !(t.Content == "\"IncludeAllModuleTests\"" || t.Content == "\"NextIsObjectBaseClass\"" ||
        t.Content == "\"MakeVirtualFilesystem\"" || t.Content == "\"MakeStaticClassContainer\"") {
        panic(fmt.Sprintf("Unknown pragma [%s].", t.Content))
    }

    if t.Content == "\"MakeVirtualFilesystem\"" {
        ExecuteJafgPragma_MakeVirtualFilesystem(tokens, idx, packetWrapper)
    }

    if t.Content == "\"MakeStaticClassContainer\"" {
        ExecuteJafgPragma_MakeStaticClassContainer(tokens, idx, packetWrapper)
    }

    return
}

func ExecuteJafgPragma_MakeVirtualFilesystem(tokens []Token, idx int, packetWrapper *JPacketWrapper) {
    var packet JPacket = JPacket{}

    var t *Token = &tokens[idx]
    if t.Type != TOKEN_PRAGMA {
        panic(fmt.Sprintf("Expected a class field token at {%d}.", idx))
    }

    packet.Callback = OnBuildJafgMakeVirtualFilesystem
    packet.Name = t.Content
    packet.Line = t.Line
    packetWrapper.Packets = append(packetWrapper.Packets, packet)

    return
}

func ExecuteJafgPragma_MakeStaticClassContainer(tokens []Token, idx int, packetWrapper *JPacketWrapper) {
    var packet JPacket = JPacket{}

    var t *Token = &tokens[idx]
    if t.Type != TOKEN_PRAGMA {
        panic(fmt.Sprintf("Expected a class field token at {%d}.", idx))
    }

    packet.Callback = OnBuildJafgMakeStaticClassContainer
    packet.Name = t.Content
    packet.Line = t.Line
    packetWrapper.Packets = append(packetWrapper.Packets, packet)

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

    idx++
    for {
        idx++
        if idx >= len(tokens) {
            break
        }
        if tokens[idx].Type != TOKEN_CLASS_FIELD {
            break
        }
        var arg string = ""
        if Shared.ContainsByPredicate(tokens[idx].Info, func(s string) bool { return s == "Config" }) {
            arg += "@C"
        }
        if Shared.ContainsByPredicate(tokens[idx].Info, func(s string) bool { return s == "DefaultOnly" }) {
            arg += "@D"
        }
        arg += tokens[idx].Content
        packet.Args = append(packet.Args, arg)
    }

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

func AddJafgClassFieldToPacket(tokens []Token, idx int, packetWrapper *JPacketWrapper) {
    var packet JPacket = JPacket{}

    var t *Token = &tokens[idx]
    if t.Type != TOKEN_CLASS_FIELD {
        panic(fmt.Sprintf("Expected a class field token at {%d}.", idx))
    }

    packet.Callback = OnBuildJafgClassField
    packet.Name = t.Content
    packet.Line = t.Line
    packet.Args = append(packet.Args, t.Info...)

    packetWrapper.Packets = append(packetWrapper.Packets, packet)

    return
}

func OnBuildJafgClassDeclarationGenericCommon__VA__ARGS(packet JPacket) string {
    if len(packet.Args) < 3 {
        return ""
    }

    var out string = ""

    for i := 2; i < len(packet.Args); i++ {
        var arg string = packet.Args[i]

        var bConfig bool = strings.Contains(arg, "@C")
        var bDefaultOnly bool = strings.Contains(arg, "@D")

        var memberName string = arg
        if bConfig {
            memberName = memberName[2:]
        }
        if bDefaultOnly {
            memberName = memberName[2:]
        }

        if bConfig && bDefaultOnly {
            out += fmt.Sprintf(`                                                                   \
Ref->GetMutableClassFieldsDangerous().Emplace(                                                     \
    /* Field Name   */ "%s",                                                                       \
    /* Field Setter */ LSetClassField::CreateMemberFunction(Ref, &_TObj::_SetField_%s),            \
    /* Field Getter */ LGetClassField::CreateMemberFunction(Ref, &_TObj::_GetField_%s),            \
    /* Field Malloc */ LCustomMallocClassField::CreateMemberFunction(Ref, &_TObj::_MallocField_%s) \
);                                                                                                 \
`,
                memberName, memberName, memberName, memberName,
            )
        } else if bConfig && !bDefaultOnly {
            out += fmt.Sprintf(`                                                        \
Ref->GetMutableClassFieldsDangerous().Emplace(                                          \
    /* Field Name   */ "%s",                                                            \
    /* Field Setter */ LSetClassField::CreateMemberFunction(Ref, &_TObj::_SetField_%s), \
    /* Field Getter */ LGetClassField::CreateMemberFunction(Ref, &_TObj::_GetField_%s), \
    /* Field Malloc */ nullptr                                                          \
);                                                                                      \
`,
                memberName, memberName, memberName,
            )
        } else if !bConfig && bDefaultOnly {
            out += fmt.Sprintf(`                                                                   \
Ref->GetMutableClassFieldsDangerous().Emplace(                                                     \
    /* Field Name   */ "%s",                                                                       \
    /* Field Setter */ nullptr,                                                                    \
    /* Field Getter */ nullptr,                                                                    \
    /* Field Malloc */ LCustomMallocClassField::CreateMemberFunction(Ref, &_TObj::_MallocField_%s) \
);                                                                                                 \
`,
                memberName, memberName,
            )
        }

        continue
    }

    return out
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
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [%s] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION */
#define %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION(...)              \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        /* My Class Name */          %s, /* ORIGIN VALUE: %s */                                \
        /* My Class Spaces */        %s, /* ORIGIN ARG VALUE: %s */                            \
        /* Super Class Name */       %s, /* ORIGIN VALUE: %s */                                \
        /* Line */                   %d,                                                       \
        /* Additional Class Flags */ __VA_ARGS__                                               \
    )
`,
        hFileId, packet.Line, packet.Name, hFileId, packet.Line, hFileId, packet.Line,

        RemoveAllNamespaces(packet.Name), packet.Name,
        RemoveDoubleColonGccShenanigansSuffix(packet.Args[1]), packet.Args[1],
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],
        packet.Line,
    ))

    bT.WriteString(fmt.Sprintf(`
#if PLATFORM_SUPPORTS_SHARED_LIBRARIES
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION( \
        /* My Class Name */          %s, /* ORIGIN VALUE: %s */                               \
        /* My Class Spaces */        %s, /* ORIGIN ARG VALUE: %s */                           \
        /* Line Of Declaration */    %d,                                                      \
        /* Super Class Name */       %s, /* ORIGIN VALUE: %s */                               \
        /* __VA_ARGS__ */            %s                                                       \
    )
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */
`,
        RemoveAllNamespaces(packet.Name), packet.Name,
        RemoveDoubleColonGccShenanigansSuffix(packet.Args[1]), packet.Args[1],
        packet.Line,
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],

        OnBuildJafgClassDeclarationGenericCommon__VA__ARGS(packet),
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
    #if!PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [%s] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION */
#define %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION(...)               \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        /* My Class Name */          %s, /* ORIGIN VALUE: %s */                                 \
        /* My Class Spaces */        %s, /* ORIGIN ARG VALUE: %s */                             \
        /* Super Class Name */       %s, /* ORIGIN VALUE: %s */                                 \
        /* Line */                   %d,                                                        \
        /* Additional Class Flags */ __VA_ARGS__                                                \
    )
`,
        hFileId, packet.Line, packet.Name, hFileId, packet.Line, hFileId, packet.Line,

        RemoveAllNamespaces(packet.Name), packet.Name,
        RemoveDoubleColonGccShenanigansSuffix(packet.Args[1]), packet.Args[1],
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],
        packet.Line,
    ))

    bT.WriteString(fmt.Sprintf(`
#if PLATFORM_SUPPORTS_SHARED_LIBRARIES
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION( \
        /* My Class Name */          %s, /* ORIGIN VALUE: %s */                               \
        /* My Class Spaces */        %s, /* ORIGIN ARG VALUE: %s */                           \
        /* Line Of Declaration */    %d,                                                      \
        /* Super Class Name */       %s, /* ORIGIN VALUE: %s */                               \
        /* __VA_ARGS__ */            %s                                                       \
    )
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */
`,
        RemoveAllNamespaces(packet.Name), packet.Name,
        RemoveDoubleColonGccShenanigansSuffix(packet.Args[1]), packet.Args[1],
        packet.Line,
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],

        OnBuildJafgClassDeclarationGenericCommon__VA__ARGS(packet),
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
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [%s] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION */
#define %s_%d_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION(TFactoryTy, ...)                \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        /* My Class Name */          %s, /* ORIGIN VALUE: %s */                                              \
        /* My Class Spaces */        %s, /* ORIGIN ARG VALUE: %s */                                          \
        /* Super Class Name */       %s, /* ORIGIN VALUE: %s */                                              \
        /* Line */                   %d,                                                                     \
        /* Factory Type */           TFactoryTy,                                                             \
        /* Additional Class Flags */ __VA_ARGS__                                                             \
    )
`,
        hFileId, packet.Line, packet.Name, hFileId, packet.Line, hFileId, packet.Line,

        RemoveAllNamespaces(packet.Name), packet.Name,
        RemoveDoubleColonGccShenanigansSuffix(packet.Args[1]), packet.Args[1],
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],
        packet.Line,
    ))

    bT.WriteString(fmt.Sprintf(`
#if PLATFORM_SUPPORTS_SHARED_LIBRARIES
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION( \
        /* My Class Name */          %s, /* ORIGIN VALUE: %s */                               \
        /* My Class Spaces */        %s, /* ORIGIN ARG VALUE: %s */                           \
        /* Line Of Declaration */    %d,                                                      \
        /* Super Class Name */       %s, /* ORIGIN VALUE: %s */                               \
        /* __VA_ARGS__ */            %s                                                       \
    )
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */
`,
        RemoveAllNamespaces(packet.Name), packet.Name,
        RemoveDoubleColonGccShenanigansSuffix(packet.Args[1]), packet.Args[1],
        packet.Line,
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],

        OnBuildJafgClassDeclarationGenericCommon__VA__ARGS(packet),
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
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [%s] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* %s_%d_MY_GENERATED_CLASS_BODY */
#define %s_%d_MY_GENERATED_CLASS_BODY(...)                            \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_IMPL(          \
        /* My Class Name */            %s, /* ORIGIN VALUE: %s */     \
        /* My Class Spaces */          %s, /* ORIGIN ARG VALUE: %s */ \
        /* Super Class Name */         %s, /* ORIGIN VALUE: %s */     \
        /* Construction Helper Line */ %d                             \
    )
`,
        hFileId, packet.Line, packet.Name, hFileId, packet.Line, hFileId, packet.Line,

        RemoveAllNamespaces(packet.Name), packet.Name,
        RemoveDoubleColonGccShenanigansSuffix(packet.Args[2]), packet.Args[2],
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
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [%s] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* %s_%d_MY_GENERATED_CLASS_BODY */
#define %s_%d_MY_GENERATED_CLASS_BODY(...)                            \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_BODY_IMPL(         \
        /* My Class Name */            %s, /* ORIGIN VALUE: %s */     \
        /* My Class Spaces */          %s, /* ORIGIN ARG VALUE: %s */ \
        /* Super Class Name */         %s, /* ORIGIN VALUE: %s */     \
        /* Construction Helper Line */ %d                             \
    )
`,
        hFileId, packet.Line, packet.Name, hFileId, packet.Line, hFileId, packet.Line,

        RemoveAllNamespaces(packet.Name), packet.Name,
        RemoveDoubleColonGccShenanigansSuffix(packet.Args[2]), packet.Args[2],
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
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [%s] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* %s_%d_MY_GENERATED_CLASS_BODY */
#define %s_%d_MY_GENERATED_CLASS_BODY(...)                                 \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_BODY_IMPL( \
        /* My Class Name */            %s, /* ORIGIN VALUE: %s */          \
        /* My Class Spaces */          %s, /* ORIGIN ARG VALUE: %s */      \
        /* Super Class Name */         %s, /* ORIGIN VALUE: %s */          \
        /* Construction Helper Line */ %d                                  \
    )
`,
        hFileId, packet.Line, packet.Name, hFileId, packet.Line, hFileId, packet.Line,

        RemoveAllNamespaces(packet.Name), packet.Name,
        RemoveDoubleColonGccShenanigansSuffix(packet.Args[2]), packet.Args[2],
        RemoveAllNamespaces(packet.Args[0]), packet.Args[0],
        lineHelperConstruction,
    ))

    return
}

func OnBuildJafgClassField(hFileId string, bH *strings.Builder, bT *strings.Builder, packet JPacket) {
    if bH == nil {
        panic("Builder for header is nil.")
    }
    if bT == nil {
        panic("Builder for translation is nil.")
    }

    if len(packet.Args) == 0 {
        panic(fmt.Sprintf("No class fields provided: %s::%d.", packet.Name, packet.Line))
        return
    }

    bH.WriteString(fmt.Sprintf(`
#ifdef %s_%d_MY_GENERATED_CLASS_FIELD_DECLARATION
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [%s] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* %s_%d_MY_GENERATED_CLASS_FIELD_DECLARATION */
#define %s_%d_MY_GENERATED_CLASS_FIELD_DECLARATION(...) \
`,
        hFileId, packet.Line, packet.Name, hFileId, packet.Line, hFileId, packet.Line,
    ))

    if Shared.ContainsByPredicate(packet.Args, func(s string) bool { return s == "Config" }) {
        bH.WriteString(fmt.Sprintf(`                                        \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_FIELD_DECLARATION_Config( \
        /* My Class Member */ %s                                            \
    )                                                                       \
`,
            packet.Name,
        ))
    }

    if Shared.ContainsByPredicate(packet.Args, func(s string) bool { return s == "DefaultOnly" }) {
        bH.WriteString(fmt.Sprintf(` \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_FIELD_DECLARATION_DefaultOnly( \
        /* My Class Member */ %s                                            \
    )                                                                     \
`,
            packet.Name,
        ))
    }

    return
}

func OnBuildJafgMakeVirtualFilesystem(hFileId string, bH *strings.Builder, bT *strings.Builder, packet JPacket) {
    if bH == nil {
        panic("Builder for header is nil.")
    }
    if bT == nil {
        panic("Builder for translation is nil.")
    }

    bT.WriteString("#if WITH_VIRTUAL_FILESYSTEM\n")
    bT.WriteString("#include \"CoreAfx.h\"\n")
    bT.WriteString("#include \"System/VFilesystem.h\"\n")

    var absFiles []string = Shared.GetAllFilesInRelativeDirRecursive(Core.DirPath_Content)
    for _, file := range absFiles {
        if strings.Contains(file, ".gitignore") {
            continue
        }
        if strings.Contains(file, ".gitkeep") {
            continue
        }

        content, err := os.ReadFile(file);
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

        var relFile string = strings.ReplaceAll(file, "\\", "/")
        var contentIdx int = strings.Index(relFile, Core.DirPath_Content)
        relFile = relFile[contentIdx:]

        var formattedFileName string = strings.ReplaceAll(file, "\\", "/")
        formattedFileName = strings.ReplaceAll(formattedFileName, ":", "_")
        formattedFileName = strings.ReplaceAll(formattedFileName, "/", "__")
        formattedFileName = strings.ReplaceAll(formattedFileName, ".", "_")

        bH.WriteString(fmt.Sprintf(`
ENGINE_API extern const uint8* ___FILE_EXTERN___%s;
inline Jafg::Private::LVirtualFile __%s(Jafg::LPath("%s"), ___FILE_EXTERN___%s, %d);`,
            formattedFileName,
            formattedFileName, relFile, formattedFileName, indices,
        ))

        bT.WriteString(fmt.Sprintf(`
namespace
{
const uint8 ___%s[] = { %s };
} /* ~Namespace <Anonymous> */
ENGINE_API const uint8* ___FILE_EXTERN___%s = ___%s;`,
            formattedFileName, buffer.String(), formattedFileName, formattedFileName,
        ))

        continue
    }

    bT.WriteString("\n#endif /* WITH_VIRTUAL_FILESYSTEM */\n")

    return
}

func OnBuildJafgMakeStaticClassContainer(hFileId string, bH *strings.Builder, bT *strings.Builder, packet JPacket) {
    if bH == nil {
        panic("Builder for header is nil.")
    }
    if bT == nil {
        panic("Builder for translation is nil.")
    }

    bT.WriteString("#include \"CoreAfx.h\"\n\n")
    bT.WriteString("#if !PRIVATE_JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES\n")

    // All modules have to exist in the same solution. So we just iterate over the first one.
    for idx, _ := range GBuildTargetInfo.GetSlnPointerChecked().Targets[0].Modules {
        var module *Core.Module = &GBuildTargetInfo.GetSlnPointerChecked().Targets[0].Modules[idx]
        if module.GetFunctionalRelativeDir() == GBuildTargetInfo.GetModulePointerChecked().GetFunctionalRelativeDir() {
            continue
        }

        var generatedTranslationDir string = fmt.Sprintf("%s/%s/%s",
            GBuildTargetInfo.GetSlnPointerChecked().GetSavedRelativeDir(),
            module.GetFunctionalRelativeDir(),
            Core.GtDir,
        )
        var generteHeaderDir string = fmt.Sprintf("%s/%s/%s",
            GBuildTargetInfo.GetSlnPointerChecked().GetSavedRelativeDir(),
            module.GetFunctionalRelativeDir(),
            Core.GhDir,
        )

        var absHFilesReal []string = Shared.GetAllFilesInRelativeDirRecursive(generteHeaderDir)
        var absHFiles []string = make([]string, 0)
        for _, file := range absHFilesReal {
            var relFile string = strings.ReplaceAll(file, "\\", "/")
            var contentIdx int = strings.Index(relFile, Core.DirPath_Saved)
            relFile = relFile[contentIdx:]
            absHFiles = append(absHFiles, relFile)
        }
        var absFiles []string = Shared.GetAllFilesInRelativeDirRecursive(generatedTranslationDir)
        for _, file := range absFiles {
            if strings.Contains(file, ".gitignore") {
                continue
            }
            if strings.Contains(file, ".gitkeep") {
                continue
            }
            if strings.Contains(file, fmt.Sprintf("LaunchWasm%s", Core.GtExtension)) {
                continue
            }
            if strings.Contains(file, fmt.Sprintf("VFilesystem%s", Core.GtExtension)) {
                continue
            }

            var filename string = path.Base(file)
            filename = Shared.GetFileNameWithoutExtension(filename)
            potentialHeaderFile := fmt.Sprintf("%s/%s%s", generteHeaderDir, filename, ".h")
            for _, hFile := range absHFiles {
                if hFile != potentialHeaderFile {
                    continue
                }

                content, err := os.ReadFile(potentialHeaderFile)
                if err != nil {
                    panic(err)
                }

                bT.WriteString(fmt.Sprintf(`
#ifdef PLATFORM_SUPPORTS_SHARED_LIBRARIES
    #undef PLATFORM_SUPPORTS_SHARED_LIBRARIES
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */
#define PLATFORM_SUPPORTS_SHARED_LIBRARIES          1 /* Cheecky but works. */
#ifdef PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
    #undef PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
#endif /* PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#define PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS         1

/* --- */
%s
/* --- */

#ifdef PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
    #undef PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
#endif /* PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#define PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS         1 /* Could be overriden, we do not know. Just reset here for safety. */
`,
                    content,
                ))
                break
            }


            content, err := os.ReadFile(file)
            if err != nil {
                panic(err)
            }

            var relFile string = strings.ReplaceAll(file, "\\", "/")
            var contentIdx int = strings.Index(relFile, Core.DirPath_Saved)
            relFile = relFile[contentIdx:]
            var formattedFileName string = strings.ReplaceAll(file, "\\", "/")
            formattedFileName = strings.ReplaceAll(formattedFileName, ":", "_")
            formattedFileName = strings.ReplaceAll(formattedFileName, "/", "__")
            formattedFileName = strings.ReplaceAll(formattedFileName, ".", "_")

            bT.WriteString(fmt.Sprintf(`
#ifdef PLATFORM_SUPPORTS_SHARED_LIBRARIES
    #undef PLATFORM_SUPPORTS_SHARED_LIBRARIES
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */
#define PLATFORM_SUPPORTS_SHARED_LIBRARIES          1 /* Cheecky but works. */
#ifndef %s_%d_%s_SHARED_CONTAINER_DECLARATION
#define %s_%d_%s_SHARED_CONTAINER_DECLARATION
%s
#endif /* %s_%d_%s_SHARED_CONTAINER_DECLARATION */
`,
        hFileId, packet.Line, formattedFileName,
        hFileId, packet.Line, formattedFileName,
        content,
        hFileId, packet.Line, formattedFileName,
    ))



            continue
        }

        continue
    }

    bT.WriteString("\n#endif /* !PRIVATE_JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES */\n")

    return
}

func ConditionallyWritePacketToOut(packetWrapper JPacketWrapper) {
    if packetWrapper.Name == "" {
        panic("PacketWrapper.Name is empty.")
    }

    var uniqueFileId string = ConvertNameToCppValidDefine(packetWrapper.Name)
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
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated header [%s] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
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

#ifdef PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION
    #undef PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION
#endif /* PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION */
#define PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION               1

#include "%s"

#ifdef PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION
    #undef PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION
#endif /* PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION */


/*-----------------------------------------------------------------------------
    BEGIN Generated translation content.
-----------------------------------------------------------------------------*/

`,
        packetWrapper.Name[1:],
    ))

    for _, packet := range packetWrapper.Packets {
        if packet.Callback == nil {
            panic(fmt.Sprintf("Callback from [%s] is invalid.", packet.Name))
        }
        packet.Callback(hFileId, &outH, &outT, packet)
    }

    // May happen if we do not do this:
    // [warning]: backslash-newline at end of file
    outH.WriteString("\n\n")
    outT.WriteString("\n\n")

    Shared.OpenAndWriteToRelativeFileIfDifferent(relTargetGh, outH.String(), true)
    Shared.OpenAndWriteToRelativeFileIfDifferent(relTargetGt, outT.String(), true)

    return
}
