/*
 * Copyright 2024 mzoesch. All rights reserved.
 */

package Shared

func GetRuneCountInString(s string, r rune) int {
    var count int = 0
    for _, c := range s {
        if c == r {
            count++
        }
    }

    return count
}
