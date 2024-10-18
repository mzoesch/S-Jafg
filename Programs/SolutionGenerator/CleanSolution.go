// Copyright 2024 mzoesch. All rights reserved.

package SolutionGenerator

import (
    "Jafg/Shared"
    "fmt"
)

func CleanSolution() {
    Shared.DeleteAbsoluteFileIfExists(Shared.GetAbsolutePathToSolutionLuaFile())
    Shared.DeleteAbsoluteFileIfExists(Shared.GetAbsolutePathToEngineRootDir() + "/Jafg.sln")

    for _, proj := range Shared.GApp.Projects {
        var targetedGeneratedFile string = proj.GetRelativeProjectDir() + "/" + proj.Name + ".vcxproj"
        targetedGeneratedFile, bExists := Shared.GetAbsolutePath(targetedGeneratedFile)

        if bExists {
            Shared.DeleteAbsoluteFileIfExists(targetedGeneratedFile)
        }

        continue
    }

    fmt.Println("Finished cleaning solution.")

    return
}
