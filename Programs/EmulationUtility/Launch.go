// Copyright mzoesch. All rights reserved.

package EmulationUtility

import (
    "Jafg/BuildTool"
    "Jafg/Core"
    "Jafg/SolutionGenerator"
    "fmt"
    "strings"
)

//
// Launch is the entry point for the EmulationUtility
//
// Arguments - all arguments are mandatory:
//   - SLN=<str>                 [The solution to reindex]
//   - WHAT=<All>/<OnlyExisting> [Whether to reindex only existing files or reflect all files and regenerate
//                                the solution.]
//
func Launch(args []string) {
    var whatStr string = ""
    var slnStr string = ""
    for _, arg := range args {
        if strings.Contains(arg, "WHAT=") {
            whatStr = strings.Split(arg, "=")[1]
        } else if strings.Contains(arg, "SLN=") {
            slnStr = strings.Split(arg, "=")[1]
        }
    }
    if whatStr == "" || slnStr == "" {
        panic("Could not find all necessary build target information.")
    }

    if whatStr == "All" {
        err := SolutionGenerator.Launch([]string{fmt.Sprintf("GEN=%s", slnStr), "EmulateCompiler"})
        if err != nil {
            panic(err)
        }
    } else if whatStr == "OnlyExisting" {
        var sln *Core.Solution = Core.GApp.GetSolutionByNameChecked(slnStr)
        for idxT, _ := range sln.Targets {
            var tar *Core.Target = &sln.Targets[idxT]
            for idxM, _ := range tar.Modules {
                var mod *Core.Module = &tar.Modules[idxM]
                var args []string = []string{
                    "pre-build", fmt.Sprintf("SLN=%s", sln.Name), fmt.Sprintf("MODULE=%s", mod.GetUniqueName()),
                    fmt.Sprintf("KIND=%s", mod.Kind.ToLuaString()), "ARCH=NOT_SET",
                    fmt.Sprintf("TARGET=%s", tar.Name), "PLATFORM=NOT_SET",
                }
                fmt.Printf(
                    "Emulating compiler pre-build commands for module [%s/%s] with args [%v] ...\n",
                    sln.GetSavedRelativeDir(), mod.Name, args,
                )
                BuildTool.Launch(args)
                continue
            }
            continue
        }
    } else {
        panic("Invalid arguments.")
    }

    return
}
