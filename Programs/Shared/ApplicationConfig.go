// Copyright 2024 mzoesch. All rights reserved.

package Shared

import "fmt"

type ApplicationConfig struct {
    /** Whether the build tool should copy the content files to the runtime output directory. */
    bDoCopyContent bool
    /** Whether the build tool should copy the shared project binaries to the runtime output directory. */
    bDoCopyBinaries bool
}

func (appConfig *ApplicationConfig) Initialize() {
    var configFileRel string = ConfigDir + "/Program.cfg"
    jsonMap := GetJsonMap(ReadFileContentsFromRelativePath(configFileRel))

    for k, v := range jsonMap {
        switch vv := v.(type) {
        case string:
            appConfig.InitializeWithStringField(k, vv)
        case bool:
            appConfig.InitializeWithBooleanField(k, vv)
        default:
            panic(fmt.Sprintf("Could not resolve type: %T.", vv))
        }
    }

    return
}

func (appConfig *ApplicationConfig) InitializeWithStringField(key string, value string) {
    switch key {
    default:
        panic(fmt.Sprintf("Could not resolve key: %s.", key))
    }

    return
}

func (appConfig *ApplicationConfig) InitializeWithBooleanField(key string, value bool) {
    switch key {
    case "bDoCopyContent":
        appConfig.bDoCopyContent = value
    case "bDoCopyBinaries":
        appConfig.bDoCopyBinaries = value
    default:
        panic(fmt.Sprintf("Could not resolve key: %s.", key))
    }

    return
}

func (appConfig *ApplicationConfig) ToString() string {
    return fmt.Sprintf("{bDoCopyContent:%t,bDoCopyBinaries:%t}", appConfig.bDoCopyContent, appConfig.bDoCopyBinaries)
}

func (appConfig *ApplicationConfig) DoCopySharedContent() bool {
    return appConfig.bDoCopyContent
}

func (appConfig *ApplicationConfig) DoCopySharedBinaries() bool {
    return appConfig.bDoCopyBinaries
}
