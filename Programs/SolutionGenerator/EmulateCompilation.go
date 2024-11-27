// Copyright 2024 mzoesch. All rights reserved.

package SolutionGenerator

import (
    "Jafg/BuildTool"
    "Jafg/Shared"
    "fmt"
)

func EmulateCompilationAll() {
    fmt.Printf("Emulating compilation of all [%d] modules ...\n", len(Shared.GApp.GetAllModules()))

    var modules []*Shared.Module = Shared.GApp.GetAllModules()
    var idx int = 0

    /*
     * Has to be done first, as it defines the root of the object hierarchy.
     */
    fmt.Printf("Emulating compilation of module [%s] (%d/%d) ...\n", "Engine", idx+1, len(modules))
    EmulateCompilation(Shared.GApp.GetCheckedModuleByName("Engine"))
    idx++

    for _, module := range modules {
        if module.GetUsableName() == "Engine" {
            continue
        }

        // TODO If we have modules that depend on each other, we have to sort them in a way that
        //      the dependencies are compiled first. Currently this will only work if they are in alphabetical order.
        //      Then we can also remove the hard-coded "Engine" module above.

        fmt.Printf("Emulating compilation of module [%s] (%d/%d) ...\n", module.GetUsableName(), idx+1, len(modules))
        EmulateCompilation(module)
        idx++

        continue
    }

    return
}

func EmulateCompilation(module *Shared.Module) {
    var args []string = []string{
        "--pre-build", "--BUILD_CONFIG=Debug-Client", "--PLATFORM=NOT_SET",
        fmt.Sprintf("--MOD_NAME=%s", module.GetUsableName()),
        fmt.Sprintf("--CFG_KIND=%s", module.GetKind().ToLuaString(nil)),
        "--CFG_SYSTEM=NOT_SET", "--CFG_ARCHITECTURE=NOT_SET",
    }

    fmt.Printf("Emulating compilation with args: %v\n", args)
    BuildTool.Launch(args)

    return
}
