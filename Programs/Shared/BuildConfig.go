// Copyright 2024 mzoesch. All rights reserved.

package Shared

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
    switch config {
    case "Shipping":
        return BCFG_SHIPPING
    case "Development":
        return BCFG_DEVELOPMENT
    case "Debug":
        return BCFG_DEBUG
    default:
        panic("Unknown build configuration")
    }
}
