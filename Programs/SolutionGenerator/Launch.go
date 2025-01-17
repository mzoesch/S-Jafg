// Copyright mzoesch. All rights reserved.

package SolutionGenerator

import (
    "Jafg/Core"
    "Jafg/Shared"
    "fmt"
    "slices"
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
//   - EmulateCompiler [Emulates all possible compiler commands. Useful for the first
//                      run of this program to generate all generated public and translation files]
//
func Launch(args []string) error {
    config := Shared.GetJsonMap(Shared.ReadRelativeFile(Core.FilePath_ConfigFile))
    if config["bPredefineJafgMacros"] == nil {
        panic("Could not find the 'bPredefineJafgMacros' key in the config file.")
    }

    GbPredefineJafgMacros = Shared.GetJsonBool(config, "bPredefineJafgMacros")
    fmt.Printf("CFG: <bPredefineJafgMacros>: %v.\n", GbPredefineJafgMacros)

    if slices.Contains(args, "GenerateAll") {
        fmt.Println("Generating all...")
        err := GenerateAll()
        if err != nil {
            return err
        }
    }

    if slices.Contains(args, "EmulateCompiler") {
        fmt.Println("Emulating compiler...")
        err := EmulateCompiler()
        if err != nil {
            return err
        }
    }

    return nil
}

func GenerateAll() error {
    fmt.Println("Generating solution...")

    for idx, _ := range Core.GApp.Solutions {
        var sln *Core.Solution = &Core.GApp.Solutions[idx]
        err := GenerateSolutionFromPremake(sln)
        if err != nil {
            return err
        }
    }

    err := MakeCmakeScripts()
    if err != nil {
        return err
    }

    fmt.Println("================================")
    fmt.Println("Finished generating solution.")
    fmt.Println("================================")

    return nil
}

func EmulateCompiler() error {
    fmt.Println("Emulate compiler commands...")
    fmt.Println("================================")
    fmt.Println("Finished emulating compiler commands.")
    fmt.Println("================================")
    return nil
}
