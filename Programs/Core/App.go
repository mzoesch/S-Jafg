// Copyright mzoesch. All rights reserved.

package Core

import "fmt"

// App is holding the structure for the main application.
// An application consists of modules and targets.
// Modules can reference other modules as dependencies.
// Public dependencies are transitive and public circular dependencies are forbidden.
// @see GApp
type App struct {
    Solutions []Solution
}

// GApp is the single global instance of the Application struct.
var GApp App = App{}

func (a App) GetSolutionByName(name string) *Solution {
    for idx, _ := range a.Solutions {
        if a.Solutions[idx].Name == name {
            return &a.Solutions[idx]
        }
    }
    return nil
}

func (a App) GetSolutionByNameChecked(name string) *Solution {
    var sln *Solution = a.GetSolutionByName(name)
    if sln == nil {
        panic(fmt.Sprintf("Could not find the solution [%s] by name.", name))
    }
    return sln
}
