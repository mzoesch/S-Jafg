// Copyright 2024 mzoesch. All rights reserved.

package Shared

type ConditionalInclude struct {
    Condition string
    Include   string
}

func (cond *ConditionalInclude) IsConditionTrue(targ *Target) bool {
    for _, def := range targ.AdditionalDefines {
        if def == cond.Condition {
            return true
        }

        continue
    }

    return false
}
