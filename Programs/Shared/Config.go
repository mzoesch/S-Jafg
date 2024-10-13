// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "encoding/json"
    "fmt"
    "os"
)

type AppConfiguration struct {
    /** Whether the build tool should copy the content files to the runtime output directory */
    bDoCopyContent bool
    /** Whether the build tool should copy the shared project binaries to the runtime output directory */
    bDoCopyBinaries bool
}

func (config *AppConfiguration) DoCopyContent() bool {
    return config.bDoCopyContent
}

func (config *AppConfiguration) DoCopySharedProjectBinaries() bool {
    return config.bDoCopyBinaries
}

func (config *AppConfiguration) ToString() string {
    return fmt.Sprintf("AppConfiguration: { bDoCopyContent: %t, bDoCopyBinaries: %t }",
        config.bDoCopyContent, config.bDoCopyBinaries)
}

func (config *AppConfiguration) Initialize() {
    var configFileRel string = ConfigDir + "/Program.cfg"
    var configFileAbs, exists = GetAbsolutePath(configFileRel)
    if !exists {
        panic("Could not find the configuration file: " + configFileRel)
    }

    file, err := os.ReadFile(configFileAbs)
    if err != nil {
        panic(err)
    }

    var content string = string(file[:])
    var jsonInterface interface{}
    err = json.Unmarshal([]byte(content), &jsonInterface)
    if err != nil {
        panic(err)
    }
    var jsonMap map[string]interface{} = jsonInterface.(map[string]interface{})

    for k, v := range jsonMap {
        switch vv := v.(type) {
        case string:
            config.InitializeStringField(k, vv)
        case bool:
            config.InitializeBoolField(k, vv)
        default:
            panic(fmt.Sprintf("Unknown configuration field type: %T", vv))
        }
    }

    return
}

func (config *AppConfiguration) InitializeStringField(key string, value string) {
    switch key {
    default:
        panic(fmt.Sprintf("Unknown configuration field: %s", key))
    }

    return
}

func (config *AppConfiguration) InitializeBoolField(key string, value bool) {
    switch key {
    case "bDoCopyContent":
        config.bDoCopyContent = value
    case "bDoCopyBinaries":
        config.bDoCopyBinaries = value
    default:
        panic(fmt.Sprintf("Unknown configuration field: %s", key))
    }

    return
}
