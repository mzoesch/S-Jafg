// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "encoding/json"
    "fmt"
    "io"
    "os"
    "strings"
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

func CheckForDirAbsolute(absPath string) {
    if _, err := os.Stat(absPath); os.IsNotExist(err) {
        err := os.MkdirAll(absPath, os.ModePerm)
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

func GetFileContents(file *os.File) string {
    if file == nil {
        panic("GetFileContents called with nil file")
    }

    data, err := io.ReadAll(file)
    if err != nil {
        panic(err)
    }
    return string(data)
}

func ReadFileContents(absFilePath string) string {
    file, err := os.ReadFile(absFilePath)
    if err != nil {
        panic(err)
    }

    return string(file)
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

func GetJsonMap(content string) map[string]interface{} {
    var jsonInterface interface{}
    var err error = json.Unmarshal([]byte(content), &jsonInterface)
    if err != nil {
        panic(err)
    }

    return jsonInterface.(map[string]interface{})
}

func RelPathToAbsPath(relPath string) string {
    return fmt.Sprintf("%s/%s", GApp.GetEngineRootDir(), relPath)
}

func Indent(indent int) string {
    return strings.Repeat(" ", indent)
}

func WriteWithIndent(builder *strings.Builder, indent int, content string) {
    builder.WriteString(Indent(indent))
    builder.WriteString(content)
}
