// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "fmt"
    "slices"
    "strings"
)

type TokenKind int

const (
    TOKEN_INVALID    TokenKind = iota
    TOKEN_PRAGMA     TokenKind = iota
    TOKEN_JAFG_CLASS TokenKind = iota
)

func (tk TokenKind) IsInvalid() bool {
    return tk == TOKEN_INVALID
}

func (tk TokenKind) IsPragma() bool {
    return tk == TOKEN_PRAGMA
}

func (tk TokenKind) IsJafgClass() bool {
    return tk == TOKEN_JAFG_CLASS
}

func (tk TokenKind) ToString() string {
    switch tk {
    case TOKEN_INVALID:
        return "INVALID"
    case TOKEN_PRAGMA:
        return "PRAGMA"
    default:
        panic("Unknown token kind")
    }
}

type Token struct {
    Kind TokenKind

    PragmaStatement string

    JafgClassName      string
    JafgClassSuperName string
}

func (t *Token) ToString() string {
    return fmt.Sprintf("{%s,%s}", t.Kind.ToString(), t.PragmaStatement)
}

func TokenizeContent(content *string) []Token {
    var out []Token

    var words []string = SplitValidCppFile(content, []string{" ", ",", "\n", "\t", "\r"})

    /* Whether to skip the next word because of forward declarations. */
    var bSkipNextWord bool = false
    for index, word := range words {
        if bSkipNextWord {
            bSkipNextWord = false
            continue
        }

        /* Always ignore pragmas for the C/C++ preprocessor. */
        if strings.HasPrefix(word, "#") {
            bSkipNextWord = true
            continue
        }

        if strings.HasPrefix(word, "PRAGMA_FOR_JAFG_BUILD_TOOL") {
            MakeTokenForPragma(&out, GetCheckedNextValidString(&words, &index))
            continue
        }

        if strings.HasPrefix(word, "DECLARE_JAFG_CLASS") {
            MakeTokenForJafgClass(&out, index, &words)
        }

        continue
    }

    return out
}

// SplitString splits a string by the given splits. Will not split if inside a string.
func SplitString(content *string, splits []string) []string {
    var out []string

    var bInString bool = false
    var currentString string

    for _, character := range *content {
        if character == '"' {
            if bInString {
                bInString = false
                currentString += string(character)
                continue
            }

            currentString += string(character)
            bInString = true
            continue
        }

        if bInString {
            currentString += string(character)
            continue
        }

        var bSkip bool = false
        for _, split := range splits {
            if string(character) == split {
                bSkip = true

                if currentString != "" {
                    out = append(out, currentString)
                    currentString = ""
                    break
                }

                break
            }

            continue
        }

        if bSkip {
            continue
        }

        currentString += string(character)

        continue
    }

    return out
}

// SplitValidCppFile splits a C/C++ file by the given splits. Will not split if inside a string or a comment.
func SplitValidCppFile(content *string, splits []string) []string {
    var out []string
    var currentString string

    var eolTokens []string = []string{"\n", "\r"}

    var bInString bool = false
    var bInComment bool = false
    var bInMultilineComment bool = false

    var lastCharacter int32 = 0
    for _, character := range *content {
        if bInComment {
            if slices.Contains(eolTokens, string(character)) {
                bInComment = false
            }
            lastCharacter = character
            continue
        }
        if bInMultilineComment {
            if string(character) == "/" && lastCharacter == '*' {
                bInMultilineComment = false
            }
            lastCharacter = character
            continue
        }

        if character == '"' && lastCharacter != '\\' {
            if bInString {
                bInString = false
                currentString += string(character)
                out = append(out, currentString)
                currentString = ""
                lastCharacter = character
                continue
            }

            currentString += string(character)
            bInString = true
            continue
        }
        if bInString {
            currentString += string(character)
            lastCharacter = character
            continue
        }

        if character == '/' && lastCharacter == '/' {
            currentString = currentString[:len(currentString)-1]
            bInComment = true
            lastCharacter = character
            continue
        }
        if character == '*' && lastCharacter == '/' {
            currentString = currentString[:len(currentString)-1]
            bInMultilineComment = true
            lastCharacter = character
            continue
        }

        if slices.Contains(splits, string(character)) {
            if currentString != "" {
                out = append(out, currentString)
                currentString = ""
            }

            lastCharacter = character
            continue
        }

        currentString += string(character)
        lastCharacter = character

        continue
    }

    return out
}

// GetCheckedNextValidString gets the next valid string from words starting at index.
// A string is valid if it is enclosed in double quotes (Single quotes represent a character in C/C++, therefore,
// they are not valid.). Backslashes can be used to escape double quotes.
func GetCheckedNextValidString(words *[]string, index *int) string {
    var out string
    var bStarted bool = false
    var bEscapedLastWord bool = false
    for ; *index < len(*words); *index++ {
        for _, character := range (*words)[*index] {
            if bEscapedLastWord {
                bEscapedLastWord = false
                out += string(character)
            }

            if character == '\\' {
                bEscapedLastWord = true
                out += string(character)
                continue
            }

            if character == '"' {
                if bStarted {
                    return out
                }

                bStarted = true

                continue
            }

            if bStarted {
                out += string(character)
            }

            continue
        }

        continue
    }

    panic("No valid string found.")

    return ""
}

// FindNextCheckedIndex finds a word in words and returns its index.
// If start is nil, the search will begin at zero.
func FindNextCheckedIndex(words *[]string, start *int, targetString string) (int, bool) {
    if start == nil {
        start = new(int)
        *start = 0
    }

    for ; *start < len(*words); *start++ {
        if (*words)[*start] == targetString {
            return *start, true
        }

        continue
    }

    return -1, false
}

func MakeTokenForPragma(tokens *[]Token, pragma string) {
    var token Token
    token.Kind = TOKEN_PRAGMA
    token.PragmaStatement = pragma
    *tokens = append(*tokens, token)
    return
}

func MakeTokenForJafgClass(tokens *[]Token, index int, words *[]string) {
    if index+1 >= len(*words) {
        panic("Unexpected end of file.")
    }

    var token Token
    token.Kind = TOKEN_JAFG_CLASS

    predictedClassNameLocation, bFound := FindNextCheckedIndex(words, &index, "class")
    if !bFound {
        panic("Expected class keyword.")
    }
    predictedClassNameLocation++
    if strings.HasSuffix((*words)[predictedClassNameLocation], "_API") {
        predictedClassNameLocation++
    }
    token.JafgClassName = (*words)[predictedClassNameLocation]

    predictedSuperClassNameLocation, bFound := FindNextCheckedIndex(words, &index, "public")
    if !bFound {
        if len(*tokens) > 0 {
            if (*tokens)[len(*tokens)-1].Kind.IsPragma() {
                lastToken := (*tokens)[len(*tokens)-1]
                if lastToken.PragmaStatement != "NextIsObjectBaseClass" {
                    panic("Expected public keyword to derive from super class.")
                }
            } else {
                panic("Expected public keyword to derive from super class.")
            }
        } else {
            panic("Expected public keyword to derive from super class.")
        }
    } else {
        predictedSuperClassNameLocation++
        var superClassName string = (*words)[predictedSuperClassNameLocation]
        var superClassNameParts []string = strings.Split(superClassName, "::")
        token.JafgClassSuperName = superClassNameParts[len(superClassNameParts)-1]
    }

    *tokens = append(*tokens, token)

    return
}
