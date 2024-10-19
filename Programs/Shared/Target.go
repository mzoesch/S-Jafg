// Copyright 2024 mzoesch. All rights reserved.

package Shared

import "fmt"

type Target struct {
    Suffix            string
    Parent            *Module
    AdditionalDefines []string
    CustomEntryPoint  string
}

func (targ *Target) LoadTarget() {
    if len(targ.Suffix) == 0 {
        panic("Target suffix is empty.")
    }
    if targ.Parent == nil {
        panic("Target parent is nil.")
    }

    var jsonMap map[string]interface{} = GetJsonMap(ReadFileContentsFromRelativePath(targ.Parent.GetRelativeModuleFilePath()))
    for k, v := range jsonMap {
        switch vv := v.(type) {
        case string:
            targ.DeserializeStringField(k, vv)
        case []interface{}:
            targ.DeserializeArrayField(k, vv)
        case map[string]interface{}:
            if k == "ConditionalPublicDependencies" {
                continue
            }
            panic(fmt.Sprintf("Could not resolve type: %T.", vv))
        default:
            panic(fmt.Sprintf("Could not resolve type: %T. Inside target: %s.", vv, targ.Parent.GetUsableName()))
        }
    }

    return
}

func (targ *Target) DeserializeStringField(key string, value string) {
    switch key {
    case "FriendlyName", "Pch", "Kind":
        /* Already handled in module loader. */
    default:
        panic(fmt.Sprintf("Unknown string field [%s] in target [%s].", key, targ.GetSuffix()))
    }

    return
}

func (targ *Target) DeserializeArrayField(key string, value []interface{}) {
    switch key {
    case "Targets":
        for _, v := range value {
            var targetObj map[string]interface{} = v.(map[string]interface{})
            for kObj, vObj := range targetObj {
                switch vvObj := vObj.(type) {
                case string:
                    if kObj == "Suffix" {
                        if vvObj != targ.Suffix {
                            continue
                        }
                        for kkObj, vvObj := range targetObj {
                            switch vvvObj := vvObj.(type) {
                            case string:
                                if kkObj == "CustomEntryPoint" {
                                    targ.CustomEntryPoint = vvvObj
                                    continue
                                }
                                if kkObj == "Suffix" {
                                    continue
                                }
                                panic(fmt.Sprintf("Unknown string field [%s] in target [%s].", kkObj, targ.GetSuffix()))
                            case []interface{}:
                                if kkObj == "AdditionalDefines" {
                                    targ.AddAdditionalDefines(vvvObj)
                                    continue
                                }
                                panic(fmt.Sprintf("Unknown array field [%s] in target [%s].", kkObj, targ.GetSuffix()))
                            default:
                                panic(fmt.Sprintf("Could not resolve type: %T.", vvObj))
                            }
                        }
                    }
                default:
                    continue
                }

                continue
            }

            continue
        }
    case "PublicDependencies":
        /* Already handled in module loader. */
    default:
        panic(fmt.Sprintf("Unknown array field [%s] in target [%s].", key, targ.GetSuffix()))
    }

    return
}

func (targ *Target) AddAdditionalDefines(defines []interface{}) {
    for _, v := range defines {
        targ.AdditionalDefines = append(targ.AdditionalDefines, v.(string))
        continue
    }

    return
}

func (targ *Target) PrettyPrint(indent int) {
    fmt.Println(fmt.Sprintf(
        "%s| Target [%s]",
        Indent(indent), targ.GetSuffix(),
    ))

    return
}

func (targ *Target) GetSuffix() string {
    return targ.Suffix
}

func (targ *Target) HasCustomEntryPoint() bool {
    return len(targ.CustomEntryPoint) != 0
}
