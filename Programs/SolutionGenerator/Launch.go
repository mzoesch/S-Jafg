// Copyright mzoesch. All rights reserved.

package SolutionGenerator

import (
    "Jafg/BuildTool"
    "Jafg/Core"
    "fmt"
    "slices"
)

//
// Launch generates the solution
//
// Valid arguments:
//   - GenerateAll [Generates all files]
//   - EmulateCompiler [Emulates all possible compiler commands. Useful for the first
//                      run of this program to generate all generated public and translation files]
//
func Launch(args []string) error {
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
        err1 := BuildTool.GenerateSolutionFromPremake(sln)
        if err1 != nil {
            return err1
        }
    }

    err2 := BuildTool.MakeCmakeScripts()
    if err2 != nil {
        return err2
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
