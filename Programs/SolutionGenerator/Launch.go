// Copyright mzoesch. All rights reserved.

package SolutionGenerator

import (
    "Jafg/Core"
    "Jafg/Shared"
    "fmt"
    "slices"
    "strings"
)

// GbPredefineJafgMacros is a global flag to enable or disable the predefined Jafg macros.
// This is usually not a big deal with Visual Studio. But for whatever reason, Rider struggles a lot with the __LINE__
// C++ macro in the intellisense (which results in a heavily polluted intellisense error output). So enabling this
// is recommended (also for Visual Studio - there may be fewer errors in the intellisense but there still are some...).
var GbPredefineJafgMacros bool = false

//
// Launch generates the solution
//
// Valid arguments:
//   - GenerateAll [Generates all files]
//   - GEN=<str>   [Generates the solution with the given name]
//   - EmulateCompiler [Emulates all possible compiler commands. Useful for the first
//                      run of this program to generate all generated header and translation files]
//
func Launch(args []string) error {
    config := Shared.GetJsonMap(Shared.ReadRelativeFile(Core.FilePath_ConfigFile))
    if config["bPredefineJafgMacros"] == nil {
        panic("Could not find the 'bPredefineJafgMacros' key in the config file.")
    }

    GbPredefineJafgMacros = Shared.GetJsonBool(config, "bPredefineJafgMacros")
    fmt.Printf("CFG: <bPredefineJafgMacros>: %v.\n", GbPredefineJafgMacros)

    if slices.Contains(args, "GenerateAll") {
        fmt.Println("Generating all ...")
        err := GenerateAll(slices.Contains(args, "EmulateCompiler"))
        if err != nil {
            return err
        }
    } else if Shared.ContainsByPredicate(args, func(arg string) bool {
        return strings.Contains(arg, "GEN=")
    }) {
        var slnStr string = ""
        for _, arg := range args {
            if Shared.ContainsByPredicate([]string{"GEN="}, func(str string) bool {
                return strings.Contains(arg, str)
            }) {
                slnStr = strings.Split(arg, "=")[1]
                break
            }
        }
        if slnStr == "" {
            panic("Could not find the solution to generate.")
        }
        var sln *Core.Solution = Core.GApp.GetSolutionByNameChecked(slnStr)
        err := GenerateSpecificSolution(sln, slices.Contains(args, "EmulateCompiler"))
        if err != nil {
            return err
        }
    } else {
        panic(fmt.Sprintf("Invalid arguments. Args: %v", args))
    }

    return nil
}

func GenerateAll(bEmulate bool) error {
    fmt.Println("Generating solution ...")

    for idx, _ := range Core.GApp.Solutions {
        var sln *Core.Solution = &Core.GApp.Solutions[idx]
        err := GenerateSolutionFromPremake(sln, bEmulate)
        if err != nil {
            return err
        }
    }

    for idx, _ := range Core.GApp.Solutions {
        var sln *Core.Solution = &Core.GApp.Solutions[idx]
        err := MakeCmakeScripts(sln, bEmulate)
        if err != nil {
            return err
        }
    }

    fmt.Println("================================")
    fmt.Println("Finished generating solution.")
    fmt.Println("================================")

    return nil
}

func GenerateSpecificSolution(sln *Core.Solution, bEmulate bool) error {
    fmt.Println("Generating solution ...")

    err := GenerateSolutionFromPremake(sln, bEmulate)
    if err != nil {
        return err
    }

    err = MakeCmakeScripts(sln, bEmulate)
    if err != nil {
        return err
    }

    fmt.Println("================================")
    fmt.Println("Finished generating solution.")
    fmt.Println("================================")

    return nil
}
