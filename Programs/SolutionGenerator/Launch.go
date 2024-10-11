// Copyright 2024 mzoesch. All rights reserved.

package SolutionGenerator

import "slices"

func Launch(args []string) {
    if slices.Contains(args, "--Clean") {
        CleanSolution()
        return
    }

    GenerateSolution()

    return
}
