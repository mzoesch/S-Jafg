// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "fmt"
    "os"
)

// GenerateWorkspaceWideBuildHeaderFile generates a header file that contains build information for the
// entire workspace.
func GenerateWorkspaceWideBuildHeaderFile() string {
    if GBuildInfo == nil {
        panic("BuildInfo not initialized.")
    }

    var relativeTargetFile string = fmt.Sprintf(
        "%s/Build%s",
        Shared.GeneratedHeadersDir,
        Shared.GeneratedHeadersExtension,
    )

    var content string = fmt.Sprintf(`
#define PRIVATE_JAFG_BUILD_GENERATED_STR_HELPER(msg)        #msg
#define PRIVATE_JAFG_BUILD_GENERATED_STR(x)                 PRIVATE_JAFG_BUILD_GENERATED_STR_HELPER( x )

/*-----------------------------------------------------------------------------
    Call specs for all modules.
-----------------------------------------------------------------------------*/
%s

/*-----------------------------------------------------------------------------
    Assert call specs for all modules.
-----------------------------------------------------------------------------*/
%s

#undef PRIVATE_JAFG_BUILD_GENERATED_STR_HELPER
#undef PRIVATE_JAFG_BUILD_GENERATED_STR
`,
        MakeModuleCallSpecs(),
        MakeModuleAssertCallSpecs(),
    )

    var file *os.File = Shared.OpenRelativeFile(relativeTargetFile, false, os.O_RDWR|os.O_CREATE)
    var finalContent string = MakeHeaderContentFinal(file, true, false, content)
    Shared.CloseFile(file)
    return finalContent
}

func ConditionallyWritePchSourceFileForModule(bUse bool, relPath string) {
    if bUse {
        Shared.CheckRelativeFile(relPath)

        fmt.Printf("Writing source afx for module at [%s]...\n", relPath)

        var content string = fmt.Sprintf(`
%s

#include "CoreAfx.h"
`,
            GeneratedTranslationFileStub,
        )

        var file *os.File = Shared.OpenRelativeFile(relPath, false, os.O_RDWR|os.O_CREATE)
        if Shared.IsFileAndStringEqual(file, content, true) {
            fmt.Printf("Source afx for module at [%s] is up-to-date. Skipping re-write.\n", relPath)
        } else {
            fmt.Printf("Source afx for module at [%s] is outdated. Re-writing.\n", relPath)
            Shared.TruncateFile(file)
            Shared.SeekFileBeginning(file)
            Shared.WriteToFile(file, content)
        }

        return
    }

    Shared.DeleteRelativeFileIfExists(relPath)

    return
}
