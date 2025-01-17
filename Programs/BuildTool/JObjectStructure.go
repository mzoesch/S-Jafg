// Copyright mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Core"
    "Jafg/Shared"
    "encoding/json"
    "fmt"
    "os"
    "strings"
)

type JPacketCallback func(hFileId string, bH *strings.Builder, bT *strings.Builder, packet JPacket)

type JPacket struct {
    Callback JPacketCallback
    Name     string   `json:"Name"`
    Line     int      `json:"Line"`
    Args     []string `json:"Args"`
}

// JPacketWrapper wraps j-packets.
type JPacketWrapper struct {
    Name    string    `json:"Name"`
    Packets []JPacket `json:"Packets"`
}

// JPacketContainer is the global singleton for all JPacketWrapper.
type JPacketContainer struct {
    Wrappers []JPacketWrapper `json:"Wrappers"`
}

// GJPacketContainer is the global singleton for JPacketContainer. Might be nil, so do not use without checking.
var GJPacketContainer *JPacketContainer = nil

func (jpc *JPacketContainer) PullCache() {
    fmt.Printf("Pulling cache from [%s] ...\n", Core.FilePath_ObjStructureCache)

    Shared.CheckRelativeFile(Core.FilePath_ObjStructureCache)
    var content string = Shared.ReadRelativeFile(Core.FilePath_ObjStructureCache)
    if content == "" {
        content = "{}"
    }

    decoder := json.NewDecoder(strings.NewReader(content))
    var err error = decoder.Decode(GJPacketContainer)
    if err != nil {
        panic(err)
    }

    return
}

func (jpc *JPacketContainer) PushCache() {
    fmt.Printf("Pushing cache to [%s] ...\n", Core.FilePath_ObjStructureCache)

    jsonData, err := json.MarshalIndent(GJPacketContainer, "", "    ")
    if err != nil {
        panic(err)
    }

    Shared.CheckRelativeFile(Core.FilePath_ObjStructureCache)
    file := Shared.OpenRelativeFile(Core.FilePath_ObjStructureCache, true, os.O_RDWR)
    _, err = file.WriteAt([]byte(jsonData), 0)
    if err != nil {
        panic(err)
    }

    return
}
