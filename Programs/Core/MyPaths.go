// Copyright mzoesch. All rights reserved.

package Core

import (
    "fmt"
)

var DirPath_Venv string = ".venv"
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

// GtDir is the dir for generated translation files
var GtDir string = "gt"
// CgtDir is the dir for conditional generated translation files
var CgtDir string = "cgt"
// GhDir is the dir for generated header files
var GhDir string = "gh"
// CgtDir is the dir for conditional generated header files
var CghDir string = "cgh"

var FilePath_PchSource string = "ModulePch" + GtExtension
var FilePath_PchHeader string = "ModulePch" + GhExtension
