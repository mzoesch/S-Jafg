// Copyright mzoesch. All rights reserved.

package Core

import (
    "fmt"
)

var _AbsoluteEngineRoot string = ""

func SetAbsoluteEngineRoot(path string) {
    _AbsoluteEngineRoot = path
}
func GetAbsoluteEngineRoot() string {
    if _AbsoluteEngineRoot == "" {
        panic("Engine root directory is not set.")
    }
    return _AbsoluteEngineRoot
}

var DirPath_Saved string = "Saved"

var FileName_LuaOut string = "__buildSolution.lua"
func MakeFilePath_LuaOut(sln *Solution) string /* relative */ {
    return fmt.Sprintf("%s/%s", sln.GetSavedRelativeDir(), FileName_LuaOut)
}

var FileName_CmakeOut string = "CMakeLists.txt"
var FileName_CmakePresets string = "CMakePresets.json"

var FilePath_ModuleCache string = fmt.Sprintf("%s/structure.jproject", DirPath_Saved)
var FilePath_ObjStructureCache string = fmt.Sprintf("%s/jobj.jobj", DirPath_Saved)

var GhExtension string = ".generated.h"
var GtExtension string = ".generated.cpp"
