// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "fmt"
    "os"
)

// GenerateWorkspaceWideBuildHeaderFile generates a header file that contains build information for the
// entire workspace. *But* only modifies the existing file if the generated build information is different
// from the existing build information. This is to avoid lengthy rebuilds as gcc, msvc or clang will use
// timestamps to determine if a file needs to be rebuilt. This sucks for us :(.
func GenerateWorkspaceWideBuildHeaderFile() {
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

   content = MakeHeaderContentFinal(file, true, false, content)

    if Shared.IsFileAndStringEqual(file, content, true) {
        fmt.Printf("Workspace-wide build header file [%s] is up-to-date. Skipping re-write.\n", relativeTargetFile)
    } else {
        fmt.Printf("Workspace-wide build header file [%s] is outdated. Writing new content.\n", relativeTargetFile)
        Shared.TruncateFile(file)
        Shared.SeekFileBeginning(file)
        Shared.WriteToFile(file, content)
    }

    Shared.CloseFile(file)

    return
}
