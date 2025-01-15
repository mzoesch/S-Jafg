// Copyright mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "fmt"
    "strings"
)

func ReflectModule() {
    if GBuildTargetInfo == nil {
        panic("BuildTargetInfo not initialized.")
    }

    var SearchDir string = GBuildTargetInfo.GetRelativeSourceDir()
    var filesAbs []string = Shared.GetAllFilesInRelativeDirRecursive(SearchDir)

    var hFilesRel []string
    for _, fAbs := range filesAbs {
        if Shared.IsHeaderFile(fAbs) == false {
            continue
        }
        var fRel string = Shared.ToRelativePath(Shared.NormalizePath(fAbs))
        hFilesRel = append(hFilesRel, "~"+fRel)
        continue
    }

    // Remove deleted files from the cache for the current module only.
    var newWrappers []JPacketWrapper = make([]JPacketWrapper, 0)
    for idxW, _ := range GJPacketContainer.Wrappers {
        if strings.HasPrefix(GJPacketContainer.Wrappers[idxW].Name, GBuildTargetInfo.Module) == false {
            newWrappers = append(newWrappers, GJPacketContainer.Wrappers[idxW])
            continue
        }
        var bFound bool = false
        for idxF, _ := range hFilesRel {
            if GJPacketContainer.Wrappers[idxW].Name == hFilesRel[idxF] {
                bFound = true
                break
            }
        }
        if bFound {
            newWrappers = append(newWrappers, GJPacketContainer.Wrappers[idxW])
        }
    }
    GJPacketContainer.Wrappers = newWrappers

    for idx, _ := range hFilesRel {
        ReflectFile(hFilesRel[idx])
    }

    return
}

func ReflectFile(fRel string) {
    fmt.Println("Reflecting file: " + fRel)

    var tokens []Token = Tokenize(fRel, Shared.ReadRelativeFile(fRel[1:]))

    for idx, _ := range tokens {
        fmt.Println(tokens[idx].ToString())
    }

    return
}
