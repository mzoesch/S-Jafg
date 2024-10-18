// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "fmt"
    "io"
    "os"
)

// CheckRelativeDir checks if a directory exists and if not tries to create it.
func CheckRelativeDir(relDir string) {
    if _, err := os.Stat(fmt.Sprintf("%s/%s", GetAbsolutePathToEngineRootDir(), relDir)); os.IsNotExist(err) {
        err := os.MkdirAll(fmt.Sprintf("%s/%s", GetAbsolutePathToEngineRootDir(), relDir), os.ModePerm)
        if err != nil {
            panic(err)
        }
    }

    return
}

// CheckAbsoluteDir checks if a directory exists and if not tries to create it.
func CheckAbsoluteDir(absDir string) {
    if _, err := os.Stat(absDir); os.IsNotExist(err) {
        err := os.MkdirAll(absDir, os.ModePerm)
        if err != nil {
            panic(err)
        }
    }

    return
}

func CheckRelativeFile(relFilePath string) {
    CheckAbsoluteFile(fmt.Sprintf("%s/%s", GetAbsolutePathToEngineRootDir(), relFilePath))
    return
}

func CheckAbsoluteFile(absFilePath string) {
    if _, err := os.Stat(absFilePath); os.IsNotExist(err) {
        file, err := os.Create(absFilePath)
        if err != nil {
            panic(err)
        }

        err = file.Close()
        if err != nil {
            panic(err)
        }
    }

    return
}

func VerifyAbsoluteFileExistence(absFile string) {
    if _, err := os.Stat(absFile); os.IsNotExist(err) {
        if err != nil {
            panic(fmt.Sprintf("Error while verifying [%s] existence: %s.", absFile, err))
        }
    }

    return
}

// TruncateRelativeFile truncates a file, or creates it if it doesn't exist
func TruncateRelativeFile(relFile string) {
    TruncateAbsoluteFile(fmt.Sprintf("%s/%s", GetAbsolutePathToEngineRootDir(), relFile))
    return
}

func TruncateAbsoluteFile(absFile string) {
    file, err := os.OpenFile(absFile, os.O_TRUNC|os.O_CREATE, 0644)
    if err != nil {
        panic("Failed wth file: " + absFile + " " + err.Error())
    }

    err = file.Close()
    if err != nil {
        panic(err)
    }

    return
}

func TruncateFile(file *os.File) {
    err := file.Truncate(0)
    if err != nil {
        panic(err)
    }

    return
}

func SeekFileBeginning(file *os.File) {
    _, err := file.Seek(0, 0)
    if err != nil {
        panic(err)
    }

    return
}

func OpenRelativeFile(relDir string, bTruncate bool, flag int) *os.File {
    return OpenAbsoluteFile(fmt.Sprintf("%s/%s", GetAbsolutePathToEngineRootDir(), relDir), bTruncate, flag)
}

func OpenAbsoluteFile(absDir string, bTruncate bool, flag int) *os.File {
    if bTruncate {
        TruncateAbsoluteFile(absDir)
    }

    file, err := os.OpenFile(absDir, flag, 0644)
    if err != nil {
        panic(err)
    }

    return file
}

func CloseFile(file *os.File) {
    err := file.Close()
    if err != nil {
        panic(err)
    }

    return
}

func WriteToFile(file *os.File, data string) {
    _, err := file.WriteString(data)
    if err != nil {
        panic(err)
    }

    return
}

func GetContentsFromOpenFile(file *os.File) string {
    if file == nil {
        panic("GetContentsFromOpenFile called with nil file.")
    }

    data, err := io.ReadAll(file)
    if err != nil {
        panic(err)
    }

    return string(data)
}

func ReadFileContentsFromRelativePath(relPath string) string {
    return ReadFileContentsFromAbsolutePath(fmt.Sprintf("%s/%s", GetAbsolutePathToEngineRootDir(), relPath))
}

func ReadFileContentsFromAbsolutePath(absPath string) string {
    file := OpenAbsoluteFile(absPath, false, os.O_RDONLY)
    defer CloseFile(file)
    return GetContentsFromOpenFile(file)
}

func DeleteAbsoluteFileIfExists(absPath string) {
    if _, err := os.Stat(absPath); !os.IsNotExist(err) {
        err := os.Remove(absPath)
        if err != nil {
            panic(err)
        }
    }

    return
}

func IsFileAndStringEqual(file *os.File, other string, bPrintReasonToStdOut bool) bool {
    byteContent, err := io.ReadAll(file)
    if err != nil {
       panic(err)
    }
    var content string = string(byteContent)

    var otherRunes []rune = []rune(other)

    var lineCount int = 0
    var bEqual bool = true
    for i, c := range content {
        if c == '\n' {
            lineCount++
        }

        if c != otherRunes[i] {
            bEqual = false
            break
        }

       if i >= len(otherRunes) {
          bEqual = false
          break
       }

       continue
    }

    if bEqual {
        return true
    }

    if bPrintReasonToStdOut {
        fmt.Printf("Provided file and string are not equal [%s]. Failed at line %d.\n", file.Name(), lineCount+1)
    }

    return false
}
