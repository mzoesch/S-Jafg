// Copyright mzoesch. All rights reserved.

package Core

type PchUsage int

const (
    PCH_ALLOW    PchUsage = iota
    PCH_PROHIBIT PchUsage = iota
)

func (pch PchUsage) IsAllowed() bool {
    return pch == PCH_ALLOW
}

func (pch PchUsage) IsProhibited() bool {
    return pch == PCH_PROHIBIT
}

func (pch PchUsage) ToString() string {
    switch pch {
    case PCH_ALLOW:
        return "Allow"
    case PCH_PROHIBIT:
        return "Prohibit"
    default:
        panic("Unknown PchUsage value.")
    }
}

func PchUsageFromString(str string) PchUsage {
    switch str {
    case "Allow", "ALLOW":
        return PCH_ALLOW
    case "Prohibit", "PROHIBIT":
        return PCH_PROHIBIT
    default:
        panic("Unknown PchUsage value.")
    }
}
