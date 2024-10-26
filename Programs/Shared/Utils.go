// Copyright 2024 mzoesch. All rights reserved.

package Shared

func ContainsByPredicate[T any](ts []T, pred func(T) bool) bool {
    for _, t := range ts {
        if pred(t) {
            return true
        }
    }

    return false
}

func CountRunesInString(str string) int {
    return len([]rune(str))
}

func CountRuneInString(str string, r rune) int {
    var count int = 0
    for _, c := range str {
        if c == r {
            count++
        }
    }

    return count
}

func RemoveDuplicatesFromStringSlice(strSlice []string) []string {
    allKeys := make(map[string]bool)
    var list []string
    for _, item := range strSlice {
        if _, value := allKeys[item]; !value {
            allKeys[item] = true
            list = append(list, item)
        }
    }

    return list
}
