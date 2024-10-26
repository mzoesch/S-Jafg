// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "fmt"
)

func MakeRelativePathFromSourceToTarget(relativePathSource string, relativeTargetPath string) string {
    var sourceSlashCount int = Shared.CountRuneInString(relativePathSource, '/') + 1
    var out string = ""
    for i := 0; i < sourceSlashCount; i++ {
        out += "../"
    }

    return fmt.Sprintf("%s%s", out, relativeTargetPath)
}

func GetTranslationImportStub(justFilename string, bIncludeCoreAfx bool) string {
    var afxInclude string = ""
    if bIncludeCoreAfx {
        //afxInclude = fmt.Sprintf(
        //    "#include \"%s/CoreAfx.h\"",
        //    MakeRelativePathFromSourceToTarget(
        //        GBuildInfo.GetRelativeGeneratedTranslationDir(),
        //        Shared.GApp.GetCheckedModuleByName("Lal").GetRelativePublicSourceDir(),
        //    ),
        //)
        afxInclude = "#include \"CoreAfx.h\""
    }

    var headerInclude string = fmt.Sprintf(
        "#include \"%s\"",
            MakeRelativePathFromSourceToTarget(
                GBuildInfo.GetRelativeGeneratedTranslationDir(),
                Shared.GetRelativePathFromAbsolutePath(GCurrentHeaderState.absoluteFilePath),
            ),
        )

    var generatedHeaderInclude string = fmt.Sprintf(
        "#include \"%s/%s%s\"",
        MakeRelativePathFromSourceToTarget(
            GBuildInfo.GetRelativeGeneratedTranslationDir(),
            Shared.GeneratedHeadersDir,
        ),
        justFilename,
        Shared.GeneratedHeadersExtension,
    )

    return fmt.Sprintf(`
%s
%s

#ifndef PRIVATE_JAFG_%s_%s_GENERATED_HEADER
    #error "Header %s-file inside project %s is missing the preprocessing directive #include"
    #error "\"%s.generated.h\" at the end of all other includes."
#endif /* !PRIVATE_JAFG_%s_%s_GENERATED_HEADER */
#undef PRIVATE_JAFG_%s_%s_GENERATED_HEADER

/*
 * Even though tectonically this header file will be transitively passed to this translation unit, we include
 * it here just in case there is an include statement at the end of the target header file (or at least after
 * the #*.generated.h macros have been used, as otherwise it would not have been allowed).
 */
%s
`,
        afxInclude,
        headerInclude,

        GBuildInfo.ModuleName, justFilename,
        justFilename, GBuildInfo.ModuleName, justFilename,
        GBuildInfo.ModuleName, justFilename,
        GBuildInfo.ModuleName, justFilename,

        generatedHeaderInclude,
    )
}
