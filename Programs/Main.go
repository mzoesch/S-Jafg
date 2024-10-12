// Copyright 2024 mzoesch. All rights reserved.

package main

import (
    "Jafg/Shared"
    "log"
    "os"
)

// Routes to subprogram
//
// Valid arguments are:
//     --SolutionGenerator              [Generates solution]
//       --Clean                        [Clean solution files (not necessary all of them)]
//       --Generate                     [Generates lua files to be used to generate the solution]
//       --PostLuaRun                   [Fixes errors in said generated solution files]
//     --BuildTool                      [Helps the compiler to compile the solution]
//       --PreBuild                     [Runs pre-build tasks (@see Programs/BuildTool/Launch)]
//       --PostBuild                    [Runs post-build tasks (@see Programs/BuildTool/Launch)]
func main() {
    var myApp *Shared.Application = new(Shared.Application)
    myApp.Initialize()

    var args []string = os.Args
    var errorLevel error = RouteToSubProgram(args)

    if errorLevel != nil {
        log.Fatalf("Application failed with error: [%s].", errorLevel.Error())
    }

    os.Exit(0)

    return
}
