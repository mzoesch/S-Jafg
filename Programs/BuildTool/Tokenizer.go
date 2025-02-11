// Copyright mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "fmt"
    "slices"
    "strings"
)

type ETokenType int

const (
    TOKEN_UNKNOWN ETokenType = iota

    TOKEN_PREPROCESSOR_INCLUDE ETokenType = iota

    // TOKEN_PREPROCESSOR_IF describes either #if, #ifdef or #ifndef.
    TOKEN_PREPROCESSOR_IF ETokenType = iota
    // TOKEN_PREPROCESSOR_ELSE describes #else or #elif.
    TOKEN_PREPROCESSOR_ELSE ETokenType = iota
    // TOKEN_PREPROCESSOR_ENDIF describes #endif.
    TOKEN_PREPROCESSOR_ENDIF ETokenType = iota

    // TOKEN_PUSHNS describes a push of a namespace or multiple namespaces.
    TOKEN_PUSHNS ETokenType = iota
    // TOKEN_POPNS describes a pop of a namespace or multiple namespaces.
    TOKEN_POPNS ETokenType = iota

    // TOKEN_PRAGMA describes a pragma for jafg not for the preprocessor.
    TOKEN_PRAGMA                      ETokenType = iota
    TOKEN_DECLARE_CLASS               ETokenType = iota
    TOKEN_GENERATED_CLASS_BODY        ETokenType = iota
    TOKEN_DECLARE_WIDGET              ETokenType = iota
    TOKEN_DECLARE_WIDGET_WITH_FACTORY ETokenType = iota
    TOKEN_GENERATED_WIDGET_BODY       ETokenType = iota
    TOKEN_CLASS_FIELD                 ETokenType = iota
)

func (tk ETokenType) IsPreprocessor() bool {
    return tk == TOKEN_PREPROCESSOR_INCLUDE ||
        tk == TOKEN_PREPROCESSOR_IF ||
        tk == TOKEN_PREPROCESSOR_ELSE ||
        tk == TOKEN_PREPROCESSOR_ENDIF
}

func (tk ETokenType) IsPreprocessorInclude() bool {
    return tk == TOKEN_PREPROCESSOR_INCLUDE
}

func (tk ETokenType) IsPreprocessorIf() bool {
    return tk == TOKEN_PREPROCESSOR_IF
}

func (tk ETokenType) IsPreprocessorElse() bool {
    return tk == TOKEN_PREPROCESSOR_ELSE
}

func (tk ETokenType) IsPreprocessorEndif() bool {
    return tk == TOKEN_PREPROCESSOR_ENDIF
}

func (tk ETokenType) IsPushNs() bool {
    return tk == TOKEN_PUSHNS
}

func (tk ETokenType) IsPopNs() bool {
    return tk == TOKEN_POPNS
}

func (tk ETokenType) IsPragma() bool {
    return tk == TOKEN_PRAGMA
}

func (tk ETokenType) IsDeclareClass() bool {
    return tk == TOKEN_DECLARE_CLASS
}

func (tk ETokenType) IsGeneratedClassBody() bool {
    return tk == TOKEN_GENERATED_CLASS_BODY
}

func (tk ETokenType) IsDeclareWidget() bool {
    return tk == TOKEN_DECLARE_WIDGET
}

func (tk ETokenType) IsDeclareWidgetWithFactory() bool {
    return tk == TOKEN_DECLARE_WIDGET_WITH_FACTORY
}

func (tk ETokenType) IsGeneratedWidgetBody() bool {
    return tk == TOKEN_GENERATED_WIDGET_BODY
}

func (tk ETokenType) IsClassField() bool {
    return tk == TOKEN_CLASS_FIELD
}

func (tk ETokenType) ToString() string {
    switch tk {
    case TOKEN_PREPROCESSOR_INCLUDE:
        return "Pp_In"
    case TOKEN_PREPROCESSOR_IF:
        return "PP_If"
    case TOKEN_PREPROCESSOR_ELSE:
        return "PP_Else"
    case TOKEN_PREPROCESSOR_ENDIF:
        return "Pp_Endif"
    case TOKEN_PUSHNS:
        return "PushNs"
    case TOKEN_POPNS:
        return "PopNs"
    case TOKEN_PRAGMA:
        return "Pragma"
    case TOKEN_DECLARE_CLASS:
        return "DeclareClass"
    case TOKEN_GENERATED_CLASS_BODY:
        return "GeneratedClassBody"
    case TOKEN_DECLARE_WIDGET:
        return "DeclareWidget"
    case TOKEN_DECLARE_WIDGET_WITH_FACTORY:
        return "DeclareWidgetWithFactory"
    case TOKEN_GENERATED_WIDGET_BODY:
        return "GeneratedWidgetBody"
    case TOKEN_CLASS_FIELD:
        return "ClassField"
    default:
        panic("Unknown token type.")
    }
}

// Token describes the most minimal categorizable thing that is relevant for reflection.
type Token struct {
    Type    ETokenType
    Content string
    Line    int
    Info    []string
}

func (t *Token) ToString() string {
    return fmt.Sprintf("{%s, %s, %d}", t.Type.ToString(), t.Content, t.Line)
}

func FindNextToken(tokens *[]Token, startIdx int, tokenType ETokenType) int {
    for idx, token := range *tokens {
        if idx <= startIdx {
            continue
        }
        if token.Type == tokenType {
            return idx
        }
    }
    return -1
}

func FindPreviousToken(tokens *[]Token, startIdx int, tokenType ETokenType) int {
    for idxSmall, _ := range *tokens {
        var idx int = (len(*tokens) - 1) - idxSmall
        if idx > startIdx {
            continue
        }
        var token *Token = &(*tokens)[idx]
        if token.Type == tokenType {
            return idx
        }
    }
    return -1
}

func FindMostPreviousToken(tokens *[]Token, startIdx int, allowedTokens []ETokenType) (int /* idx */, ETokenType /* ty */) {
    for idxSmall, _ := range *tokens {
        var idx int = (len(*tokens) - 1) - idxSmall
        if idx > startIdx {
            continue
        }
        var token *Token = &(*tokens)[idx]
        for _, allowed := range allowedTokens {
            if token.Type == allowed {
                return idx, token.Type
            }
            continue
        }
        continue
    }
    return -1, TOKEN_UNKNOWN
}

// Word describes a collection of runes that are split with the C/C++ rules in mind. The most minimal categorizable
// thing that has a valid syntax.
// This means words are relevant not only for the preprocessor but also for the compiler.
type Word struct {
    Content string
    Line    int
}

func Tokenize(debugDisplayName string, content string) []Token {
    var out []Token

    var words []Word = SplitValidCppFile(debugDisplayName, content,
        []string{
            ",", "{", "}", "(", ")", ";", "<", ">", "=", "+", "-", "*",
            "&", "|", "^", "!", "~", "?", ".", ",", "[", "]",
        },
        []string{
            " ", "\t", "\r", "\n", "\\",
        },
    )

    for idx, _ := range words {
        var word string = words[idx].Content
        var line int = words[idx].Line
        var next string = ""
        var nextnext string = ""
        if len(words) > idx+1 {
            next = words[idx+1].Content
        } else {
            next = ""
        }
        if len(words) > idx+2 {
            nextnext = words[idx+2].Content
        } else {
            nextnext = ""
        }

        if strings.HasPrefix(word, "#") {
            if strings.Contains(word, "include") {
                out = append(out, Token{Type: TOKEN_PREPROCESSOR_INCLUDE, Content: next, Line: line})
            } else if Shared.ContainsByPredicate([]string{"if", "ifdef", "ifndef"}, func(s string) bool {
                return strings.Contains(word, s)
            }) {
                out = append(out, Token{Type: TOKEN_PREPROCESSOR_IF, Content: word, Line: line})
            } else if Shared.ContainsByPredicate([]string{"else", "elif"}, func(s string) bool {
                return strings.Contains(word, s)
            }) {
                out = append(out, Token{Type: TOKEN_PREPROCESSOR_ELSE, Content: word, Line: line})
            } else if strings.Contains(word, "endif") {
                out = append(out, Token{Type: TOKEN_PREPROCESSOR_ENDIF, Content: word, Line: line})
            }
        } else if word == "namespace" {
            out = append(out, Token{Type: TOKEN_PUSHNS, Content: next, Line: line})
        } else if word == "}" {
            var bracketOpen int = 0
            var namespaceOpen []int
            for idx2, word2 := range words {
                var word2prevprev string = ""
                if idx2 > 1 {
                    word2prevprev = words[idx2-2].Content
                }
                if idx2 > idx {
                    break
                }
                if word2.Content == "{" {
                    bracketOpen++
                    if word2prevprev == "namespace" {
                        namespaceOpen = append(namespaceOpen, bracketOpen)
                    }
                } else if word2.Content == "}" {
                    if idx2 == idx {
                        if len(namespaceOpen) > 0 && namespaceOpen[len(namespaceOpen)-1] == bracketOpen {
                            out = append(out, Token{Type: TOKEN_POPNS, Content: "", Line: line})
                        }
                        break
                    }
                    if len(namespaceOpen) > 0 {
                        if namespaceOpen[len(namespaceOpen)-1] == bracketOpen {
                            namespaceOpen = namespaceOpen[:len(namespaceOpen)-1]
                        }
                    }
                    bracketOpen--
                }
                continue
            }
        } else if word == "PRAGMA_FOR_JAFG_BUILD_TOOL" {
            if nextnext == "" {
                panic("Expected content after PRAGMA_FOR_JAFG_BUILD_TOOL.")
            }
            out = append(out, Token{Type: TOKEN_PRAGMA, Content: nextnext, Line: line})
        } else if word == "DECLARE_JAFG_CLASS" {
            if idx > 1 {
                if words[idx-1].Content == "#ifdef" {
                    continue
                }
            }

            var cursor int = idx

            for _, word2 := range words[idx:] {
                cursor++
                if word2.Content == "class" {
                    break
                }
            }
            if cursor >= len(words) {
                panic("Could not find class after DECLARE_JAFG_CLASS.")
            }
            if words[cursor].Content == "class" {
                cursor++
            }
            if cursor >= len(words) {
                panic("Could not find class name after DECLARE_JAFG_CLASS.")
            }
            if strings.HasSuffix(words[cursor].Content, "_API") {
                cursor++
            }
            if cursor >= len(words) {
                panic("Could not find class name after DECLARE_JAFG_CLASS.")
            }
            var classnameCursor int = cursor

            for _, word2 := range words[cursor:] {
                cursor++
                if word2.Content == ":" {
                    break
                }
            }
            if len(out) > 0 &&
                out[len(out)-1].Type == TOKEN_PRAGMA &&
                out[len(out)-1].Content == "\"NextIsObjectBaseClass\"" {
                out = append(out, Token{
                    Type:    TOKEN_DECLARE_CLASS,
                    Content: words[classnameCursor].Content,
                    Line:    line,
                    Info:    []string{"NextIsObjectBaseClass"},
                })
            } else {
                if cursor >= len(words) {
                    panic("Could not find super class after DECLARE_JAFG_CLASS.")
                }
                if words[cursor].Content == ":" {
                    cursor++
                }
                if cursor >= len(words) {
                    panic("Could not find super class name after DECLARE_JAFG_CLASS.")
                }
                if words[cursor].Content != "public" {
                    panic("Expected public keyword for super class DECLARE_JAFG_CLASS.")
                }
                cursor++
                if cursor >= len(words) {
                    panic("Could not find super class name after DECLARE_JAFG_CLASS.")
                }

                if strings.HasPrefix(words[classnameCursor].Content, "J") {
                    if strings.HasPrefix(RemoveAllNamespaces(words[cursor].Content), "J") == false {
                        panic(fmt.Sprintf("Class [%s] does not inherit from a j-object. Super class: [%s].",
                            words[classnameCursor].Content, words[cursor].Content))
                    }
                } else if strings.HasPrefix(words[classnameCursor].Content, "A") {
                    if strings.HasPrefix(RemoveAllNamespaces(words[cursor].Content), "A") == false &&
                        words[classnameCursor].Content != "AActor" {
                        panic(fmt.Sprintf("Class [%s] does not inherit from an aactor. Super class: [%s].",
                            words[classnameCursor].Content, words[cursor].Content))
                    }
                } else {
                    panic(fmt.Sprintf("Expected 'J' or 'A' as a prefix for a j-object. Faulty class: %s",
                        words[classnameCursor].Content))
                }

                out = append(out, Token{
                    Type:    TOKEN_DECLARE_CLASS,
                    Content: words[classnameCursor].Content,
                    Line:    line,
                    Info:    []string{words[cursor].Content},
                })
            }
        } else if word == "DECLARE_JAFG_WIDGET" {
            if idx > 1 {
                if words[idx-1].Content == "#ifdef" {
                    continue
                }
            }

            var cursor int = idx
            for _, word2 := range words[idx:] {
                cursor++
                if word2.Content == "class" {
                    break
                }
            }
            if cursor >= len(words) {
                panic("Could not find class after DECLARE_JAFG_WIDGET.")
            }
            if words[cursor].Content == "class" {
                cursor++
            }
            if cursor >= len(words) {
                panic("Could not find class name after DECLARE_JAFG_WIDGET.")
            }
            if strings.HasSuffix(words[cursor].Content, "_API") {
                cursor++
            }
            if cursor >= len(words) {
                panic("Could not find class name after DECLARE_JAFG_WIDGET.")
            }
            var classnameCursor int = cursor

            for _, word2 := range words[cursor:] {
                cursor++
                if word2.Content == ":" {
                    break
                }
            }
            if cursor >= len(words) {
                panic("Could not find super class after DECLARE_JAFG_WIDGET.")
            }
            if words[cursor].Content == ":" {
                cursor++
            }
            if cursor >= len(words) {
                panic("Could not find super class name after DECLARE_JAFG_WIDGET.")
            }
            if words[cursor].Content != "public" {
                panic("Expected public keyword for super class DECLARE_JAFG_WIDGET.")
            }
            cursor++
            if cursor >= len(words) {
                panic("Could not find super class name after DECLARE_JAFG_WIDGET.")
            }

            if strings.HasPrefix(words[classnameCursor].Content, "W") == false {
                panic(fmt.Sprintf("Expected 'W' as a prefix for a widget. Faulty class: [%s]",
                    words[classnameCursor].Content))
            }
            if strings.HasPrefix(RemoveAllNamespaces(words[cursor].Content), "W") == false {
                if words[cursor].Content != "WWidgetNode" {
                    panic(fmt.Sprintf("Class [%s] does not inherit from a widget. Super class: [%s].",
                        words[classnameCursor].Content, words[cursor].Content))
                }
            }

            out = append(out, Token{
                Type:    TOKEN_DECLARE_WIDGET,
                Content: words[classnameCursor].Content,
                Line:    line,
                Info:    []string{words[cursor].Content},
            })
        } else if word == "DECLARE_JAFG_WIDGET_WITH_FACTORY" {
            if idx > 1 {
                if words[idx-1].Content == "#ifdef" {
                    continue
                }
            }

            var cursor int = idx
            for _, word2 := range words[idx:] {
                cursor++
                if word2.Content == "class" {
                    break
                }
            }
            if cursor >= len(words) {
                panic("Could not find class after DECLARE_JAFG_WIDGET_WITH_FACTORY.")
            }
            if words[cursor].Content == "class" {
                cursor++
            }
            if cursor >= len(words) {
                panic("Could not find class name after DECLARE_JAFG_WIDGET_WITH_FACTORY.")
            }
            if strings.HasSuffix(words[cursor].Content, "_API") {
                cursor++
            }
            if cursor >= len(words) {
                panic("Could not find class name after DECLARE_JAFG_WIDGET_WITH_FACTORY.")
            }
            var classnameCursor int = cursor

            for _, word2 := range words[cursor:] {
                cursor++
                if word2.Content == ":" {
                    break
                }
            }
            if cursor >= len(words) {
                panic("Could not find super class after DECLARE_JAFG_WIDGET_WITH_FACTORY.")
            }
            if words[cursor].Content == ":" {
                cursor++
            }
            if cursor >= len(words) {
                panic("Could not find super class name after DECLARE_JAFG_WIDGET_WITH_FACTORY.")
            }
            if words[cursor].Content != "public" {
                panic("Expected public keyword for super class DECLARE_JAFG_WIDGET_WITH_FACTORY.")
            }
            cursor++
            if cursor >= len(words) {
                panic("Could not find super class name after DECLARE_JAFG_WIDGET_WITH_FACTORY.")
            }

            if strings.HasPrefix(words[classnameCursor].Content, "W") == false {
                panic(fmt.Sprintf("Expected 'W' as a prefix for a widget. Faulty class: [%s]",
                    words[classnameCursor].Content))
            }
            if strings.HasPrefix(words[cursor].Content, "W") == false {
                if words[classnameCursor].Content != "WWidgetNode" {
                    panic(fmt.Sprintf("Class [%s] does not inherit from a widget. Super class: [%s].",
                        words[classnameCursor].Content, words[cursor].Content))
                }
            }

            out = append(out, Token{
                Type:    TOKEN_DECLARE_WIDGET_WITH_FACTORY,
                Content: words[classnameCursor].Content,
                Line:    line,
                Info:    []string{words[cursor].Content},
            })
        } else if word == "CLASS_FIELD" {
            if idx > 1 {
                if words[idx-1].Content == "#ifdef" {
                    continue
                }
            }

            var preCursor int = idx + 1

            var cursor int = idx
            for _, word2 := range words[idx:] {
                cursor++
                if word2.Content == ")" {
                    break
                }
            }
            if cursor >= len(words) {
                panic("Expected member variable after CLASS_FIELD.")
            }
            if words[cursor].Content == ")" {
                cursor++
            }
            if cursor >= len(words) {
                panic("Expected member variable after CLASS_FIELD.")
            }

            var typeOpenings int = 0
            for {
                cursor++
                if cursor >= len(words) {
                    panic("Expected member variable after CLASS_FIELD.")
                }
                if words[cursor].Content == ";" {
                    panic("Expected member variable after CLASS_FIELD.")
                }
                if words[cursor].Content == "(" || words[cursor].Content == "<" {
                    typeOpenings++
                    continue
                }
                if words[cursor].Content == ")" || words[cursor].Content == ">" {
                    typeOpenings--
                    continue
                }

                if typeOpenings == 0 {
                    break
                }

                continue
            }

            var args []string
            var argOpenings int = 0
            for _, word2 := range words[preCursor:] {
                if word2.Content == "(" {
                    argOpenings++
                    continue
                }
                if word2.Content == ")" {
                    argOpenings--
                    if argOpenings == 0 {
                        break
                    }
                    continue
                }
                if word2.Content == "," && argOpenings == 1 {
                    continue
                }
                args = append(args, word2.Content)
            }
            if argOpenings != 0 {
                panic("Expected ')' after CLASS_FILED.")
            }

            out = append(out, Token{
                Type:    TOKEN_CLASS_FIELD,
                Content: words[cursor].Content,
                Line:    line,
                Info:    args,
            })
        } else if word == "GENERATED_CLASS_BODY" {
            out = append(out, Token{Type: TOKEN_GENERATED_CLASS_BODY, Content: "", Line: line})
        } else if word == "GENERATED_WIDGET_BODY" {
            out = append(out, Token{Type: TOKEN_GENERATED_CLASS_BODY, Content: "", Line: line})
        }
    }

    var countPushNs int = 0
    var countPopNs int = 0
    for _, token := range out {
        if token.Type.IsPushNs() {
            countPushNs++
        } else if token.Type.IsPopNs() {
            countPopNs++
        }
    }
    if countPushNs != countPopNs {
        // Theoretically, this is not std C++, lol. You do not need to close namespaces or basically anything,
        // as you can just include another file that closes more things than it opens and that would be completely
        // valid C/C++ code ... But,... we don't do that here. If someone would wanna do that, they can fuck off.
        panic(fmt.Sprintf("ERROR in file [%s]: PushNs and PopNs count mismatch: %d != %d.",
            debugDisplayName, countPushNs, countPopNs))
    }

    return out
}

// SplitValidCppFile splits a C/C++ file by the given splits. Will not split if inside a define, string or comment.
// Comments are ignored and not included in the output.
func SplitValidCppFile(debugDisplayName string, content string, splits []string, splitsNoOut []string) []Word {
    var out []Word

    var line int = 1
    var current string = ""

    var bInDefine bool = false
    var bInString bool = false
    var bInComment bool = false
    var bInLineComment bool = false

    var lastChar int32 = 0
    var lastLastChar int32 = 0
    var idx int = -1
    for _, char := range content {
        idx++

        if char == '\n' {
            line++
        }

        if bInDefine {
            if char == '\n' {
                if lastChar == '\\' || (lastChar == '\r' && lastLastChar == '\\') {
                    lastLastChar = lastChar
                    lastChar = char
                    continue
                }
                bInDefine = false
            }
            lastLastChar = lastChar
            lastChar = char
            continue
        }
        if bInComment {
            if slices.Contains([]string{"\n", "\r"}, string(char)) {
                bInComment = false
            }
            lastLastChar = lastChar
            lastChar = char
            continue
        }
        if bInLineComment {
            if string(char) == "/" && lastChar == '*' {
                bInLineComment = false
            }
            lastLastChar = lastChar
            lastChar = char
            continue
        }

        if char == '#' {
            if current != "" {
                out = append(out, Word{Content: current, Line: line})
                current = ""
            }
            bInDefine = true
            lastLastChar = lastChar
            lastChar = char
            continue
        }
        if char == '"' && lastChar != '\\' {
            if bInString {
                bInString = false
                current += string(char)
                out = append(out, Word{Content: current, Line: line})
                current = ""
                lastLastChar = lastChar
                lastChar = char
                continue
            }

            current += string(char)
            bInString = true
            lastLastChar = lastChar
            lastChar = char
            continue
        }
        if bInString {
            current += string(char)
            lastLastChar = lastChar
            lastChar = char
            continue
        }

        if char == '/' && lastChar == '/' {
            if current != "" {
                current = current[:len(current)-1]
            }
            if current != "" {
                out = append(out, Word{Content: current, Line: line})
                current = ""
            }
            bInComment = true
            lastLastChar = lastChar
            lastChar = char
            continue
        }
        if char == '*' && lastChar == '/' {
            if current != "" {
                current = current[:len(current)-1]
            }
            if current != "" {
                out = append(out, Word{Content: current, Line: line})
                current = ""
            }
            bInLineComment = true
            lastLastChar = lastChar
            lastChar = char
            continue
        }

        if slices.Contains(splits, string(char)) || slices.Contains(splitsNoOut, string(char)) {
            if current != "" {
                out = append(out, Word{Content: current, Line: line})
                current = ""
            }

            if slices.Contains(splits, string(char)) {
                out = append(out, Word{Content: string(char), Line: line})
            }

            lastLastChar = lastChar
            lastChar = char
            continue
        }

        current += string(char)
        lastLastChar = lastChar
        lastChar = char
        continue
    }

    return out
}
