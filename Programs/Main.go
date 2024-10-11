// Copyright 2024 mzoesch. All rights reserved.

package main

import (
    "Jafg/Shared"
    "log"
    "os"
)

func main() {
    var myApp *Shared.Application = new(Shared.Application)
    myApp.Initialize()

    var args []string = os.Args
    var errorLevel error = RouteToSubProgram(args)

    if errorLevel != nil {
        log.Fatalf("Application failed with error: [%s].", errorLevel.Error())
    }

    os.Exit(0)

    return
}
