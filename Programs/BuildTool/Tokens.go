// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "fmt"
    "strings"
)

type TokenKind int
const (
    TOKEN_INVALID TokenKind = iota
    TOKEN_PRAGMA TokenKind = iota
)

func (tk TokenKind) IsInvalid() bool {
    return tk == TOKEN_INVALID
}

func (tk TokenKind) IsPragma() bool {
    return tk == TOKEN_PRAGMA
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
    Kind        TokenKind
    Description string
}

func (t *Token) ToString() string {
    return fmt.Sprintf("{%s,%s}", t.Kind.ToString(), t.Description)
}

func TokenizeContent(content *string) []Token {
    var out []Token

    var words []string = SplitString(content, []string{" ", "\n", "\t", "\r"})

    /* Whether to skip the next word because of forward declarations. */
    var bSkipNextWord bool = false
    for index, word := range words {
        if bSkipNextWord {
            bSkipNextWord = false
            continue
        }

        if word == "#define" {
            bSkipNextWord = true
            continue
        }

        if strings.HasPrefix(word, "PRAGMA_FOR_JAFG_BUILD_TOOL") {
            MakeTokenForPragma(&out, GetNextValidString(&words, &index))
            continue
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

func GetNextValidString(words *[]string, index *int) string {
    var out string
    var bStarted bool = false
    for ; *index < len(*words); *index++ {
        for _, character := range (*words)[*index] {
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

func MakeTokenForPragma(tokens *[]Token, pragma string) {
    var token Token
    token.Kind = TOKEN_PRAGMA
    token.Description = pragma
    *tokens = append(*tokens, token)
    return
}
