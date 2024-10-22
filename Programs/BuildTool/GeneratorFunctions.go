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
