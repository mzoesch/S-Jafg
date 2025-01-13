// Copyright mzoesch. All rights reserved.

package Core

type Target struct {
    Name     string
    Defines  []string
    Runtime  string
    Symbols  bool
    Optimize bool
    Modules  []Module
}
