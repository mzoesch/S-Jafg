// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/BuildTool/Info"
    "Jafg/BuildTool/Private"
    "Jafg/Shared"
    "fmt"
    "os"
)

// GenerateBuildHeaderFile Generates the build header file that is unique to every project.
func GenerateBuildHeaderFile() {
    if Info.GProjectBuildInfo == nil {
        panic("GProjectBuildInfo is nil")
    }

    var TargetFile string = Shared.GeneratedHeadersDir + "/Build" + Shared.GeneratedHeaderExtension
    var file *os.File = Shared.OpenFile(TargetFile, true, os.O_WRONLY)

    var content string = fmt.Sprintf(`
#define PRIVATE_JAFG_BUILD_GENERATED_STR_HELPER(msg)        #msg
#define PRIVATE_JAFG_BUILD_GENERATED_STR(x)                 PRIVATE_JAFG_BUILD_GENERATED_STR_HELPER( x )

/*-----------------------------------------------------------------------------
    Call specs for all projects.
-----------------------------------------------------------------------------*/
%s

/*-----------------------------------------------------------------------------
    Assert call specs for all projects.
-----------------------------------------------------------------------------*/
%s

#undef PRIVATE_JAFG_BUILD_GENERATED_STR_HELPER
#undef PRIVATE_JAFG_BUILD_GENERATED_STR
`,
        Private.GetProjectCallSpecs(),
        Private.AssertProjectCallSpecs(),
    )

    Info.WriteHeaderContent(file, true, false, content)
    Shared.CloseFile(file)

    return
}
