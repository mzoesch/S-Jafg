// Copyright mzoesch. All rights reserved.

package main

import (
    "Jafg/BuildTool"
    "Jafg/EmulationUtility"
    "Jafg/SolutionGenerator"
    "errors"
    "fmt"
    "slices"
)

func RouteToSubProgram(args []string) error {
    fmt.Println("Routing to subprogram ...")
    if slices.Contains(args, "--DoNothing") {
        return nil
    } else if slices.Contains(args, "--SolutionGenerator") {
        fmt.Println("Routing to SolutionGenerator ...")
        err := SolutionGenerator.Launch(args)
        if err != nil {
            return err
        }
    } else if slices.Contains(args, "--BuildTool") {
        fmt.Println("Routing to BuildTool ...")
        BuildTool.Launch(args)
    } else if slices.Contains(args, "--EmulationUtility") {
        fmt.Println("Routing to EmulationUtility ...")
        EmulationUtility.Launch(args)
    } else {
        return errors.New(fmt.Sprintf("no subprogram specified. Args: %v", args))
    }

    return nil
}
