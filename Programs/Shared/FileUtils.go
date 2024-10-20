// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "fmt"
    "io"
    "os"
    "path/filepath"
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

// CheckRelativeFile checks if a file exists and if not tries to create it.
func CheckRelativeFile(relFilePath string) {
    CheckAbsoluteFile(fmt.Sprintf("%s/%s", GetAbsolutePathToEngineRootDir(), relFilePath))
    return
}

// CheckAbsoluteFile checks if a file exists and if not tries to create it.
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

// VerifyAbsoluteFileExistence verifies that a file exists, panicking if it doesn't
func VerifyAbsoluteFileExistence(absFile string) {
    if _, err := os.Stat(absFile); os.IsNotExist(err) {
        if err != nil {
            panic(fmt.Sprintf("Error while verifying [%s] existence: %s.", absFile, err))
        }
    }

    return
}

func DoesRelativeFileExist(relFile string) bool {
    return DoesAbsoluteFileExist(GetAbsolutePathNoCheck(relFile))
}

func DoesAbsoluteFileExist(absoluteFile string) bool {
    _, err := os.Stat(absoluteFile);
    if os.IsNotExist(err) {
        return false
    }
    if err != nil {
        panic(err)
    }

    return true
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

func OpenRelativeFile(relFile string, bTruncate bool, flag int) *os.File {
    return OpenAbsoluteFile(fmt.Sprintf("%s/%s", GetAbsolutePathToEngineRootDir(), relFile), bTruncate, flag)
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

func DeleteRelativeFileIfExists(relPath string) {
    DeleteAbsoluteFileIfExists(GetAbsolutePathNoCheck(relPath))
    return
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

    if len(content) != len(other) {
        if bPrintReasonToStdOut {
            fmt.Printf("Provided file and string are not equal [%s]. Lengths differ.\n", file.Name())
        }

        return false
    }

    var otherRunes []rune = []rune(other)

    var bEqual bool = true
    var lineCount int = 0

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

func RecursivelyGetAllFilesInRelativeDir(relativeDir string) []string {
    return RecursivelyGetAllFilesInAbsoluteDir(fmt.Sprintf("%s/%s", GetAbsolutePathToEngineRootDir(), relativeDir))
}

func RecursivelyGetAllFilesInAbsoluteDir(absDir string) []string {
    var files []string
    err := filepath.Walk(absDir, func(path string, info os.FileInfo, err error) error {
        if err != nil {
            panic(err)
        }

        if !info.IsDir() {
            files = append(files, path)
        }

        return nil
    })
    if err != nil {
        return nil
    }

    return files
}

func IsHeaderFile(file string) bool {
    return filepath.Ext(file) == ".h" || filepath.Ext(file) == ".hpp"
}

func GetFileNameFromHeaderPath(headerPath string) string {
    var base string = filepath.Base(headerPath)

    var dotIndex int = -1
    for i := len(base) - 1; i >= 0; i-- {
        if base[i] == '.' {
            dotIndex = i
            break
        }

        continue
    }

    if dotIndex == -1 {
        panic(fmt.Sprintf("Could not find dot in header path [%s].", headerPath))
    }

    return base[:dotIndex]
}

// GetRelativeGeneratedHeaderPath returns the relative path to the generated header file.
// Do not an extension to the filename.
func GetRelativeGeneratedHeaderPath(filename string) string {
    return fmt.Sprintf("%s/%s%s", GeneratedHeadersDir, filename, GeneratedHeadersExtension)
}
