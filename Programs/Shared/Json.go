// Copyright mzoesch. All rights reserved.

package Shared

import (
    "encoding/json"
    "log"
)

func GetJsonMap(content string) map[string]interface{} {
    var jsonInterface interface{}
    var err error = json.Unmarshal([]byte(content), &jsonInterface)
    if err != nil {
        panic(err)
    }

    return jsonInterface.(map[string]interface{})
}

func IsJsonStringValid(data map[string]interface{}, key string) bool {
    _, ok := data[key].(string)
    return ok
}

func GetJsonString(data map[string]interface{}, key string) string {
    out, ok := data[key].(string)
    if !ok {
        log.Panicf("Json string [%s] not found.\n", key)
    }
    return out
}

func GetJsonArray(data map[string]interface{}, key string) []interface{} {
    out, ok := data[key].([]interface{})
    if !ok {
        log.Panicf("Json array [%s] not found.\n", key)
    }
    return out
}

func GetJsonStringArray(data map[string]interface{}, key string) []string {
    out, ok := data[key].([]interface{})
    if !ok {
        log.Panicf("Json array [%s] not found.\n", key)
    }

    var outStrings []string
    for _, item := range out {
        outStrings = append(outStrings, item.(string))
    }

    return outStrings
}

func GetJsonBool(data map[string]interface{}, key string) bool {
    out, ok := data[key].(bool)
    if !ok {
        log.Panicf("Json bool [%s] not found.\n", key)
    }
    return out
}
