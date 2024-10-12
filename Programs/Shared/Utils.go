/*
 * Copyright 2024 mzoesch. All rights reserved.
 */

package Shared

import (
    "fmt"
    "os"
)

func GetRuneCountInString(s string, r rune) int {
    var count int = 0
    for _, c := range s {
        if c == r {
            count++
        }
    }

    return count
}

// CheckForDir checks if a directory exists, and if it doesn't, creates it
func CheckForDir(subDir string) {
    if _, err := os.Stat(fmt.Sprintf("%s/%s", GApp.GetEngineRootDir(), subDir)); os.IsNotExist(err) {
        err := os.MkdirAll(fmt.Sprintf("%s/%s", GApp.GetEngineRootDir(), subDir), os.ModePerm)
        if err != nil {
            panic(err)
        }
    }

    return
}

// TruncateFile truncates a file, or creates it if it doesn't exist
func TruncateFile(subDir string) {
    file, err := os.OpenFile(fmt.Sprintf("%s/%s", GApp.GetEngineRootDir(), subDir), os.O_TRUNC|os.O_CREATE, 0644)
    if err != nil {
        panic(err)
    }

    err = file.Close()
    if err != nil {
        panic(err)
    }

    return
}

func OpenFile(subDir string, bTruncate bool, flag int) *os.File {
    if bTruncate {
        TruncateFile(subDir)
    }

    file, err := os.OpenFile(fmt.Sprintf("%s/%s", GApp.GetEngineRootDir(), subDir), flag, 0644)
    if err != nil {
        panic(err)
    }

    return file
}

func WriteToFile(file *os.File, data string) {
    _, err := file.WriteString(data)
    if err != nil {
        panic(err)
    }

    return
}

func CloseFile(file *os.File) {
    err := file.Close()
    if err != nil {
        panic(err)
    }

    return
}
