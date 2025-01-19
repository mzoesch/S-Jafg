// Copyright mzoesch. All rights reserved.

package Core

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
