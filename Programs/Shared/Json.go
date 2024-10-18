// Copyright 2024 mzoesch. All rights reserved.

package Shared

import "encoding/json"

func GetJsonMap(content string) map[string]interface{} {
    var jsonInterface interface{}
    var err error = json.Unmarshal([]byte(content), &jsonInterface)
    if err != nil {
        panic(err)
    }

    return jsonInterface.(map[string]interface{})
}
