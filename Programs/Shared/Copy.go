// Copyright 2024 mzoesch. All rights reserved.

package Shared

import (
    "fmt"
    "io"
    "os"
    "strings"
)

// CopyDirectory copies a directory from source to destination. Paths must be relative.
func CopyDirectory(source string, dest string, bOverwrite bool) {
    absSource, bExists := GetAbsolutePath(source)
    if !bExists {
        panic(fmt.Sprintf("Source directory %s does not exist", absSource))
    }
    absDest, bExists := GetAbsolutePath(dest)
    if !bExists {
        CheckForDir(dest)
        absDest, bExists = GetAbsolutePath(dest)
        if !bExists {
            panic(fmt.Sprintf("Enclosing block should never be reached."))
        }
    }

    CopyDirectoryAbs(absSource, absDest, bOverwrite)

    return
}

// CopyDirectoryByExts copies a directory from source to destination. Paths must be relative.
func CopyDirectoryByExts(source string, dest string, bOverwrite bool, exts []string, bRecursively bool) {
    absSource, bExists := GetAbsolutePath(source)
    if !bExists {
        panic(fmt.Sprintf("Source directory %s does not exist", absSource))
    }
    absDest, bExists := GetAbsolutePath(dest)
    if !bExists {
        CheckForDir(dest)
        absDest, bExists = GetAbsolutePath(dest)
        if !bExists {
            panic(fmt.Sprintf("Enclosing block should never be reached."))
        }
    }

    CopyDirectoryAbsByExtsAbs(absSource, absDest, bOverwrite, exts, bRecursively)

    return
}

func CopyDirectoryAbs(absSource string, absDest string, bOverwrite bool) {
    if _, err := os.Stat(absSource); os.IsNotExist(err) {
        panic(fmt.Sprintf("Source directory %s does not exist", absSource))
    }
    CheckForDirAbsolute(absDest)
    if _, err := os.Stat(absDest); os.IsExist(err) {
        panic("Enclosing block should never be reached.")
    }

    fmt.Println(fmt.Sprintf("Copying directory %s to %s ...", absSource, absDest))

    entries, err := os.ReadDir(absSource)
    if err != nil {
        panic(err)
    }

    for _, entry := range entries {
        srcFile := fmt.Sprintf("%s/%s", absSource, entry.Name())
        destFile := fmt.Sprintf("%s/%s", absDest, entry.Name())

        fileInfo, err := os.Stat(srcFile)
        if err != nil {
            panic(fmt.Sprintf("Error with file %s: [%s].", srcFile, err))
        }

        if fileInfo.IsDir() {
            CopyDirectoryAbs(srcFile, destFile, bOverwrite)
            continue
        }

        if _, err := os.Stat(destFile, ); os.IsExist(err) {
            if bOverwrite {
                err := os.Remove(destFile)
                if err != nil {
                    panic(fmt.Sprintf("Error removing file %s: [%s].", destFile, err))
                }
            } else {
                panic(fmt.Sprintf("File %s already exists in destination directory %s", srcFile, destFile))
            }
        }

        CopyFile(srcFile, destFile)

        fmt.Println(fmt.Sprintf("    Copied %s to %s.", srcFile, destFile))

        continue
    }

    return
}

func CopyDirectoryAbsByExtsAbs(absSource string, absDest string, bOverwrite bool, exts []string, bRecursively bool) {
    if _, err := os.Stat(absSource); os.IsNotExist(err) {
        panic(fmt.Sprintf("Source directory %s does not exist", absSource))
    }
    CheckForDirAbsolute(absDest)
    if _, err := os.Stat(absDest); os.IsExist(err) {
        panic("Enclosing block should never be reached.")
    }

    fmt.Println(fmt.Sprintf("Conditionally copying directory %s to %s ...", absSource, absDest))

    entries, err := os.ReadDir(absSource)
    if err != nil {
        panic(err)
    }

    for _, entry := range entries {
        srcFile := fmt.Sprintf("%s/%s", absSource, entry.Name())
        destFile := fmt.Sprintf("%s/%s", absDest, entry.Name())

        fileInfo, err := os.Stat(srcFile)
        if err != nil {
            panic(fmt.Sprintf("Error with file %s: [%s].", srcFile, err))
        }

        if fileInfo.IsDir() {
            if bRecursively {
                CopyDirectoryAbsByExtsAbs(srcFile, destFile, bOverwrite, exts, bRecursively)
            }
            continue
        }

        bCopy := false
        for _, ext := range exts {
            if strings.HasSuffix(fileInfo.Name(), ext) {
                bCopy = true
                break
            }

            continue
        }
        if !bCopy {
            continue
        }

        if _, err := os.Stat(destFile, ); os.IsExist(err) {
            if bOverwrite {
                err := os.Remove(destFile)
                if err != nil {
                    panic(fmt.Sprintf("Error removing file %s: [%s].", destFile, err))
                }
            } else {
                panic(fmt.Sprintf("File %s already exists in destination directory %s", srcFile, destFile))
            }
        }

        CopyFile(srcFile, destFile)

        fmt.Println(fmt.Sprintf("    Copied %s to %s.", srcFile, destFile))

        continue
    }

    return
}

// CopyFile copies a file from source to destination. Dest file must not exist.
func CopyFile(absSource string, absDest string) {
    if _, err := os.Stat(absSource); os.IsNotExist(err) {
        panic(fmt.Sprintf("Source file %s does not exist", absSource))
    }

    if _, err := os.Stat(absDest); os.IsExist(err) {
        panic(fmt.Sprintf("Destination file %s already exists", absDest))
    }

    out, err := os.Create(absDest)
    if err != nil {
        panic(err)
    }

    defer func(out *os.File) {
        err := out.Close()
        if err != nil {
            panic(err)
        }
    }(out)

    in, err := os.Open(absSource)
    if err != nil {
        panic(err)
    }

    defer func(in *os.File) {
        err := in.Close()
        if err != nil {
            panic(err)
        }
    }(in)

    _, err = io.Copy(out, in)
    if err != nil {
        panic(err)
    }

    return
}
