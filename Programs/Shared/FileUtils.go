// Copyright mzoesch. All rights reserved.

package Shared

import (
    "Jafg/Core"
    "fmt"
    "io"
    "os"
    "path/filepath"
)

// CheckRelativeDir checks if a directory exists and if not tries to create it.
func CheckRelativeDir(relDir string) {
    if _, err := os.Stat(fmt.Sprintf("%s/%s", Core.GetAbsoluteEngineRoot(), relDir)); os.IsNotExist(err) {
        err := os.MkdirAll(fmt.Sprintf("%s/%s", Core.GetAbsoluteEngineRoot(), relDir), os.ModePerm)
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
    CheckAbsoluteFile(fmt.Sprintf("%s/%s", Core.GetAbsoluteEngineRoot(), relFilePath))
    return
}

// CheckAbsoluteFile checks if a file exists and if not tries to create it.
func CheckAbsoluteFile(absFilePath string) {
    if _, err := os.Stat(absFilePath); os.IsNotExist(err) {
        file, err := os.Create(absFilePath)
        if err != nil {
            CheckAbsoluteDir(filepath.Dir(absFilePath))
            CheckAbsoluteFile(absFilePath)
            return
        }

        err = file.Close()
        if err != nil {
            panic(err)
        }
    }

    return
}

func DoesRelativeFileExist(relFile string) bool {
    return DoesAbsoluteFileExist(ToAbsolutePath(relFile))
}
func DoesAbsoluteFileExist(absoluteFile string) bool {
    _, err := os.Stat(absoluteFile)
    if os.IsNotExist(err) {
        return false
    }
    if err != nil {
        panic(err)
    }

    return true
}

// VerifyRelativeFileExistence verifies that a file exists, panicking if it doesn't.
func VerifyRelativeFileExistence(relFile string) {
    VerifyAbsoluteFileExistence(ToAbsolutePath(relFile))
    return
}

// VerifyAbsoluteFileExistence verifies that a file exists, panicking if it doesn't.
func VerifyAbsoluteFileExistence(absFile string) {
    if _, err := os.Stat(absFile); os.IsNotExist(err) {
        if err != nil {
            panic(fmt.Sprintf("Error while verifying [%s] existence: %s.", absFile, err))
        }
    }

    return
}

func DoesRelativeDirExist(relDir string) bool {
    return DoesAbsoluteDirExist(ToAbsolutePath(relDir))
}
func DoesAbsoluteDirExist(absDir string) bool {
    _, err := os.Stat(absDir)
    if os.IsNotExist(err) {
        return false
    }
    if err != nil {
        panic(err)
    }

    return true
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

func TruncateRelativeFile(relFile string) {
    TruncateAbsoluteFile(ToAbsolutePath(relFile))
    return
}
func TruncateAbsoluteFile(absFile string) {
    file, err := os.OpenFile(absFile, os.O_TRUNC|os.O_RDWR, 0644)
    if err != nil {
        panic(err)
    }

    err = file.Close()
    if err != nil {
        panic(err)
    }

    return
}

func OpenRelativeFile(relFile string, bTruncate bool, flag int) *os.File {
    return OpenAbsoluteFile(ToAbsolutePath(relFile), bTruncate, flag)
}
func OpenAbsoluteFile(absFile string, bTruncate bool, flag int) *os.File {
    if bTruncate {
        TruncateAbsoluteFile(absFile)
    }

    file, err := os.OpenFile(absFile, flag, 0644)
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

func GetContentFromFile(file *os.File) string {
    if file == nil {
        panic("File is nil.")
    }

    data, err := io.ReadAll(file)
    if err != nil {
        panic(err)
    }

    return string(data)
}

func WriteToFile(file *os.File, content string) {
    _, err := file.WriteString(content)
    if err != nil {
        panic(err)
    }

    return
}

func ReadRelativeFile(relPath string) string {
    return ReadAbsoluteFile(ToAbsolutePath(relPath))
}
func ReadAbsoluteFile(absPath string) string {
    file := OpenAbsoluteFile(absPath, false, os.O_RDONLY)
    defer CloseFile(file)
    return GetContentFromFile(file)
}

func DeleteRelativeFile(relPath string) {
    DeleteAbsoluteFile(ToAbsolutePath(relPath))
    return
}
func DeleteAbsoluteFile(absPath string) {
    if _, err := os.Stat(absPath); os.IsNotExist(err) {
        return
    }

    err := os.Remove(absPath)
    if err != nil {
        panic(err)
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

func GetAllFilesInRelativeDirRecursive(relDir string) []string {
    return GetAllFilesInAbsoluteDirRecursive(ToAbsolutePath(relDir))
}
func GetAllFilesInAbsoluteDirRecursive(absDir string) []string {
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
    return filepath.Ext(file) == ".h" ||
        filepath.Ext(file) == ".hpp"  ||
        filepath.Ext(file) == ".hh"   ||
        filepath.Ext(file) == ".inl"  ||
        filepath.Ext(file) == ".fwd"
}
func IsTranslationFile(file string ) bool {
    return filepath.Ext(file) == ".cpp" ||
        filepath.Ext(file) == ".c"
}
func IsCTranslationFile(file string) bool {
    return filepath.Ext(file) == ".c"
}
func IsCppTranslationFile(file string) bool {
    return filepath.Ext(file) == ".cpp"
}

// GetFileNameWithoutExtension returns from "MyFile.generated.h" -> "MyFile"
func GetFileNameWithoutExtension(file string) string {
    var base string = filepath.Base(file)

    var dotIndex int = -1
    for i := len(base) - 1; i >= 0; i-- {
        if base[i] == '.' {
            dotIndex = i
            break
        }

        continue
    }

    if dotIndex == -1 {
        panic(fmt.Sprintf("File [%s] has no extension.", file))
    }

    return base[:dotIndex]
}
