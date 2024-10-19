// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "fmt"
    "strings"
)

type BuildConfig int

const (
    BCFG_SHIPPING    BuildConfig = iota
    BCFG_DEVELOPMENT BuildConfig = iota
    BCFG_DEBUG       BuildConfig = iota
)

func (config BuildConfig) ToString() string {
    switch config {
    case BCFG_SHIPPING:
        return "Shipping"
    case BCFG_DEVELOPMENT:
        return "Development"
    case BCFG_DEBUG:
        return "Debug"
    default:
        panic("Unknown build configuration")
    }
}

func (config BuildConfig) IsShipping() bool {
    return config == BCFG_SHIPPING
}

func (config BuildConfig) IsDevelopment() bool {
    return config == BCFG_DEVELOPMENT
}

func (config BuildConfig) IsDebug() bool {
    return config == BCFG_DEBUG
}

func IsValidBuildConfig(config string) bool {
    switch config {
    case "Shipping":
        return true
    case "Development":
        return true
    case "Debug":
        return true
    default:
        return false
    }
}

func GetBuildConfigFromString(config string) BuildConfig {
    if strings.Contains(config, BCFG_DEBUG.ToString()) {
        return BCFG_DEBUG
    }

    if strings.Contains(config, BCFG_DEVELOPMENT.ToString()) {
        return BCFG_DEVELOPMENT
    }

    if strings.Contains(config, BCFG_SHIPPING.ToString()) {
        return BCFG_SHIPPING
    }

    panic(fmt.Sprintf("Unknown build configuration [%s].", config))

    return BCFG_DEBUG
}
