// Copyright 2024 mzoesch. All rights reserved.

package main

import (
    "Jafg/BuildTool"
    "Jafg/SolutionGenerator"
    "errors"
    "slices"
)

func RouteToSubProgram(args []string) error {
    if slices.Contains(args, "--SolutionGenerator") {
        SolutionGenerator.Launch(args)
    } else if slices.Contains(args, "--BuildTool") {
        BuildTool.Launch(args)
    } else {
        return errors.New("no subprogram specified")
    }

    return nil
}
