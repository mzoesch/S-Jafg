// Copyright 2024 mzoesch. All rights reserved.

package SolutionGenerator

import "slices"

func Launch(args []string) {
    if slices.Contains(args, "--Clean") {
        CleanSolution()
        return
    }

    if slices.Contains(args, "--Generate") {
        GenerateSolution()
        return
    }

    if slices.Contains(args, "--Generate-Lal-Unit-Tests") {
        GenerateLalUnitTestsSolution()
        return
    }

    if slices.Contains(args, "--PostLuaRun") {
        PostLuaRun()
        return
    }
}
