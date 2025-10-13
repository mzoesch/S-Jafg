// Copyright mzoesch. All rights reserved.

use crate::finder;

pub const SUPER_CLASS: usize = 0;

/// Word describes a collection of runes that are split with the C/C++ rules in mind.
/// The most minimal categorizable thing that has a valid syntax.
/// This means words are relevant not only for the preprocessor but also for the compiler.
struct Word
{
    pub content: String,
    pub line: u32,
}

#[derive(Debug)]
pub enum TokenType
{
    #[allow(dead_code)]
    Undefined,

    #[allow(dead_code)]
    NamespacePush,

    #[allow(dead_code)]
    NamespacePop,

    #[allow(dead_code)]
    Pragma,

    #[allow(dead_code)]
    ClassDeclaration,

    #[allow(dead_code)]
    WidgetDeclaration,

    #[allow(dead_code)]
    WidgetDeclarationWithFactory,

    #[allow(dead_code)]
    ClassBody,

    #[allow(dead_code)]
    ClassField,
}

impl TokenType
{
    #[allow(dead_code)]
    pub fn is_undefined(&self) -> bool
    {
        match self
        {
            TokenType::Undefined => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn is_namespace_push(&self) -> bool
    {
        match self
        {
            TokenType::NamespacePush => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn is_namespace_pop(&self) -> bool
    {
        match self
        {
            TokenType::NamespacePop => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn is_pragma(&self) -> bool
    {
        match self
        {
            TokenType::Pragma => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn is_class_declaration(&self) -> bool
    {
        match self
        {
            TokenType::ClassDeclaration => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn is_widget_declaration(&self) -> bool
    {
        match self
        {
            TokenType::WidgetDeclaration => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn is_widget_declaration_with_factory(&self) -> bool
    {
        match self
        {
            TokenType::WidgetDeclarationWithFactory => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn is_class_body(&self) -> bool
    {
        match self
        {
            TokenType::ClassBody => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn is_class_field(&self) -> bool
    {
        match self
        {
            TokenType::ClassField => true,
            _ => false,
        }
    }
}

impl PartialEq for TokenType
{
    fn eq(&self, other: &Self) -> bool
    {
        match (self, other)
        {
            (TokenType::Undefined, TokenType::Undefined) => true,
            (TokenType::NamespacePush, TokenType::NamespacePush) => true,
            (TokenType::NamespacePop, TokenType::NamespacePop) => true,
            (TokenType::Pragma, TokenType::Pragma) => true,
            (TokenType::ClassDeclaration, TokenType::ClassDeclaration) => true,
            (TokenType::WidgetDeclaration, TokenType::WidgetDeclaration) => true,
            (TokenType::WidgetDeclarationWithFactory, TokenType::WidgetDeclarationWithFactory) => true,
            (TokenType::ClassBody, TokenType::ClassBody) => true,
            (TokenType::ClassField, TokenType::ClassField) => true,
            _ => false,
        }
    }
}

pub struct Token
{
    pub ty: TokenType,
    pub line: u32,
    pub content: String,
    pub info: Vec<String>,
}

pub fn tokenize_file(file: &str) -> Vec<Token>
{
    let words = split_file(
        file,
        vec![
            ',', '{', '}', '(', ')', ';', '<', '>', '=', '+', '-', '*',
            '&', '|', '^', '!', '~', '?', '.', ',', '[', ']',
        ],
        vec![
            ' ', '\t', '\r', '\n', '\\',
        ]
    );

    let mut tokens: Vec<Token> = Vec::new();

    for idx in 0..words.len()
    {
        let w: &Word = &words[idx];
        let n: Option<&Word>;
        let nn: Option<&Word>;
        if idx + 1 < words.len()
        {
            n = Some(&words[idx + 1]);
        }
        else
        {
            n = None;
        }
        if idx + 2 < words.len()
        {
            nn = Some(&words[idx + 2]);
        }
        else
        {
            nn = None;
        }

        if w.content == "namespace"
        {
            let mut alias_namespace: bool = false;
            for w in words.iter().skip(idx + 1)
            {
                if w.content == "="
                {
                    alias_namespace = true;
                    break
                }
                if w.content == "{"
                {
                    break
                }
            }
            if alias_namespace == false
            {
                tokens.push(Token { ty: TokenType::NamespacePush, line: w.line, content: String::from(&n.unwrap().content), info: vec![] });
            }
        }
        else if w.content == "}"
        {
            let mut brackets: u32 = 0;
            let mut namespaces: Vec<u32> = vec![];

            for inner_idx in 0..idx+1
            {
                let inner_w: &Word = &words[inner_idx];
                let mut inner_pp: Option<&Word> = None; // Previous previous
                if inner_idx > 1
                {
                    inner_pp = Some(&words[inner_idx - 2]);
                }

                if inner_w.content == "{"
                {
                    brackets += 1;
                    if inner_pp.is_some() && inner_pp.unwrap().content == "namespace"
                    {
                        namespaces.push(brackets);
                    }
                }
                else if inner_w.content == "}"
                {
                    if idx == inner_idx
                    {
                        if namespaces.len() > 0 && namespaces[namespaces.len() - 1] == brackets
                        {
                            tokens.push(Token { ty: TokenType::NamespacePop, line: inner_w.line, content: "".to_string(), info: vec![] });
                        }
                        break
                    }
                    if namespaces.len() > 0
                    {
                        if namespaces[namespaces.len() - 1] == brackets
                        {
                            namespaces.pop();
                        }
                    }
                    brackets -= 1;
                }

                continue
            }
        }
        else if w.content == "PRAGMA_FOR_JAFG_BUILD_TOOL"
        {
            if nn.is_some()
            {
                tokens.push(Token { ty: TokenType::Pragma, line: w.line, content: String::from(&nn.unwrap().content), info: vec![] });
            }
            else
            {
                panic!("[{}]: Excepted one parameter for PRAGMA_FOR_JAFG_BUILD_TOOL but got zero.", file);
            }
        }
        else if w.content == "DECLARE_JAFG_CLASS"
        {
            let mut inner_idx: usize = idx + 1;
            for inner_w in words.iter().skip(idx + 1)
            {
                inner_idx += 1;
                if inner_w.content == "class"
                {
                    break
                }
            }
            if inner_idx >= words.len()
            {
                panic!("[{}]: Excepted class declaration after DECLARE_JAFG_CLASS.", file);
            }
            if words[inner_idx].content.ends_with("_API")
            {
                inner_idx += 1;
            }
            if inner_idx >= words.len()
            {
                panic!("[{}]: Excepted class declaration after DECLARE_JAFG_CLASS.", file);
            }
            let classname_idx: usize = inner_idx;

            if tokens.last().is_some() && tokens.last().unwrap().ty.is_pragma() && tokens.last().unwrap().content == "\"NextIsBaseCxxClass\""
            {
                tokens.push(Token { ty: TokenType::ClassDeclaration, line: w.line, content: String::from(&words[classname_idx].content), info: vec!["NextIsBaseCxxClass".to_string()] });
            }
            else
            {
                for inner_w in words.iter().skip(classname_idx)
                {
                    inner_idx += 1;
                    if inner_w.content == ":"
                    {
                        break
                    }
                }
                if inner_idx >= words.len()
                {
                    panic!("[{}]: Excepted j-object super class after class declaration.", file);
                }
                if words[inner_idx].content != "public"
                {
                    panic!("[{}]: Excepted public inheritance for j-object super class.", file);
                }
                inner_idx += 1;
                if inner_idx >= words.len()
                {
                    panic!("[{}]: Excepted j-object super class after class declaration.", file);
                }

                if words[classname_idx].content.starts_with('J')
                {
                    if remove_all_namespaces(&words[inner_idx].content).starts_with('J') == false
                    {
                        panic!("[{}]: Excepted j-object super class after class declaration.", file);
                    }
                }
                else if words[classname_idx].content.starts_with('A')
                {
                    if remove_all_namespaces(&words[inner_idx].content).starts_with('A') == false && words[classname_idx].content != "AActor"
                    {
                        panic!("[{}]: Excepted a-object super class after a-class declaration.", file);
                    }
                }
                else
                {
                    panic!("[{}]: Excepted j-object or a-object super class after j-class declaration.", file);
                }

                tokens.push(Token { ty: TokenType::ClassDeclaration, line: w.line, content: String::from(&words[classname_idx].content), info: vec![String::from(&words[inner_idx].content)] });
            }
        }
        else if w.content == "DECLARE_JAFG_WIDGET"
        {
            let mut inner_idx: usize = idx + 1;
            for inner_w in words.iter().skip(idx + 1)
            {
                inner_idx += 1;
                if inner_w.content == "class"
                {
                    break
                }
            }
            if inner_idx >= words.len()
            {
                panic!("[{}]: Excepted class declaration after DECLARE_JAFG_CLASS.", file);
            }
            if words[inner_idx].content.ends_with("_API")
            {
                inner_idx += 1;
            }
            if inner_idx >= words.len()
            {
                panic!("[{}]: Excepted class declaration after DECLARE_JAFG_CLASS.", file);
            }
            let classname_idx: usize = inner_idx;

            if tokens.last().is_some() && tokens.last().unwrap().ty.is_pragma() && tokens.last().unwrap().content == "\"NextIsBaseCxxClass\""
            {
                tokens.push(Token { ty: TokenType::ClassDeclaration, line: w.line, content: String::from(&words[classname_idx].content), info: vec!["NextIsBaseCxxClass".to_string()] });
            }
            else
            {
                for inner_w in words.iter().skip(classname_idx)
                {
                    inner_idx += 1;
                    if inner_w.content == ":"
                    {
                        break
                    }
                }
                if inner_idx >= words.len()
                {
                    panic!("[{}]: Excepted j-object super class after class declaration.", file);
                }
                if words[inner_idx].content != "public"
                {
                    panic!("[{}]: Excepted public inheritance for j-object super class.", file);
                }
                inner_idx += 1;
                if inner_idx >= words.len()
                {
                    panic!("[{}]: Excepted j-object super class after j-class declaration.", file);
                }

                if words[classname_idx].content.starts_with('W') == false
                {
                    panic!("[{}]: Excepted w-object class declaration.", file);
                }
                else
                {
                    if remove_all_namespaces(&words[inner_idx].content).starts_with('W') == false && words[classname_idx].content != "WNode"
                    {
                        panic!("[{}]: Excepted w-object super class after class declaration.", file);
                    }
                }

                tokens.push(Token { ty: TokenType::WidgetDeclaration, line: w.line, content: String::from(&words[classname_idx].content), info: vec![String::from(&words[inner_idx].content)] });
            }
        }
        else if w.content == "DECLARE_JAFG_WIDGET_WITH_FACTORY"
        {
            let mut inner_idx: usize = idx + 1;
            for inner_w in words.iter().skip(idx + 1)
            {
                inner_idx += 1;
                if inner_w.content == "class"
                {
                    break
                }
            }
            if inner_idx >= words.len()
            {
                panic!("[{}]: Excepted class declaration after DECLARE_JAFG_CLASS.", file);
            }
            if words[inner_idx].content.ends_with("_API")
            {
                inner_idx += 1;
            }
            if inner_idx >= words.len()
            {
                panic!("[{}]: Excepted class declaration after DECLARE_JAFG_CLASS.", file);
            }
            let classname_idx: usize = inner_idx;

            if tokens.last().is_some() && tokens.last().unwrap().ty.is_pragma() && tokens.last().unwrap().content == "\"NextIsBaseCxxClass\""
            {
                tokens.push(Token { ty: TokenType::ClassDeclaration, line: w.line, content: String::from(&words[classname_idx].content), info: vec!["NextIsBaseCxxClass".to_string()] });
            }
            else
            {
                for inner_w in words.iter().skip(classname_idx)
                {
                    inner_idx += 1;
                    if inner_w.content == ":"
                    {
                        break
                    }
                }
                if inner_idx >= words.len()
                {
                    panic!("[{}]: Excepted j-object super class after class declaration.", file);
                }
                if words[inner_idx].content != "public"
                {
                    panic!("[{}]: Excepted public inheritance for j-object super class.", file);
                }
                inner_idx += 1;
                if inner_idx >= words.len()
                {
                    panic!("[{}]: Excepted j-object super class after j-class declaration.", file);
                }

                if words[classname_idx].content.starts_with('W') == false
                {
                    panic!("[{}]: Excepted w-object class declaration.", file);
                }
                else
                {
                    if remove_all_namespaces(&words[inner_idx].content).starts_with('W') == false && words[classname_idx].content != "WNode"
                    {
                        panic!("[{}]: Excepted w-object super class after class declaration.", file);
                    }
                }

                tokens.push(Token { ty: TokenType::WidgetDeclarationWithFactory, line: w.line, content: String::from(&words[classname_idx].content), info: vec![String::from(&words[inner_idx].content)] });
            }
        }
        else if w.content == "CLASS_FIELD"
        {
            let mut args: Vec<String> = Vec::new();
            let mut inner_idx: usize = idx + 1;
            let mut arg_opens: u32 = 0;
            let mut inner_content: String = String::new();
            for inner_w in words.iter().skip(idx + 1)
            {
                inner_idx += 1;
                if inner_w.content == "("
                {
                    arg_opens += 1;
                }
                else if inner_w.content == ")"
                {
                    arg_opens -= 1;
                    if arg_opens == 0
                    {
                        break
                    }
                }
                else if inner_w.content == "," && arg_opens == 1
                {
                    if inner_content.is_empty() == false
                    {
                        args.push(inner_content.clone());
                        inner_content.clear();
                    }
                }
                else
                {
                    inner_content.push_str(&inner_w.content);
                }
            }
            if inner_content.is_empty() == false
            {
                args.push(inner_content.clone());
                inner_content.clear();
            }
            if arg_opens != 0
            {
                panic!("[{}]: Excepted ')' after CLASS_FIELD declaration.", file);
            }

            let mut type_opens: u32 = 0;
            loop
            {
                inner_idx += 1;
                if inner_idx >= words.len()
                {
                    panic!("[{}]: Excepted member variable after CLASS_FIELD declaration.", file);
                }
                if words[inner_idx].content == ";"
                {
                    panic!("[{}]: Excepted member variable after CLASS_FIELD declaration.", file);
                }
                if words[inner_idx].content == "(" || words[inner_idx].content == "<"
                {
                    type_opens += 1;
                }
                else if words[inner_idx].content == ")" || words[inner_idx].content == ">"
                {
                    type_opens -= 1;
                }
                else if type_opens == 0
                {
                    break
                }
            }
            if type_opens != 0
            {
                panic!("[{}]: Excepted ')' after CLASS_FIELD declaration.", file);
            }
            let member_idx: usize = inner_idx;
            if member_idx >= words.len()
            {
                panic!("[{}]: Excepted member variable after CLASS_FIELD declaration.", file);
            }

            tokens.push(Token { ty: TokenType::ClassField, line: w.line, content: String::from(&words[member_idx].content), info: args });
        }
        else if w.content == "GENERATED_CLASS_BODY"
        {
            tokens.push(Token { ty: TokenType::ClassBody, line: w.line, content: "".to_string(), info: vec![] });
        }

        continue
    }

    let mut push_ns: u32 = 0;
    let mut pop_ns: u32 = 0;
    for t in tokens.iter()
    {
        if t.ty.is_namespace_push()
        {
            push_ns += 1;
        }
        else if t.ty.is_namespace_pop()
        {
            pop_ns += 1;
        }
    }
    if push_ns != pop_ns
    {
        // Theoretically, this is not std C++, lol. You do not need to close namespaces or basically anything,
        // as you can just include another file that closes more things than it opens and that would be completely
        // valid C/C++ code ... But,... we don't do that here. If someone would wanna do that, they can fuck off.
        panic!("[{}]: Number of namespace pushes and pops do not match: {} != {}.", file, push_ns, pop_ns);
    }

    return tokens;
}

fn remove_all_namespaces(token: &str) -> String
{
    let i: Option<usize> = token.rfind("::");
    return if i.is_some()
    {
        token[i.unwrap()+2..].to_string()
    }
    else
    {
        token.to_string()
    }
}

/// Splits a C/C++ file by the given splits. Will not split if inside a define, string or comment.
/// Comments are ignored and not included in the output.
fn split_file(file: &str, splits: Vec<char>, splits_no_out: Vec<char>) -> Vec<Word>
{
    let mut words: Vec<Word> = Vec::new();

    let mut line: u32 = 1;
    let mut current_word: String = String::new();

    let mut in_define: bool = false;
    let mut in_string: bool = false;
    let mut in_comment: bool = false;
    let mut in_multiline_comment: bool = false;

    let mut last_char: char = '\0';
    let mut last_last_char: char = '\0';

    let content: String = finder::read_file(file);
    for c in content.chars()
    {
        if c == '\n'
        {
            line += 1;
        }

        if in_define
        {
            if c == '\n'
            {
                if last_char == '\\' || (last_char == '\r' && last_last_char == '\\')
                {
                    last_last_char = last_char;
                    last_char = c;
                    continue;
                }
                in_define = false;
            }
            last_last_char = last_char;
            last_char = c;
            continue;
        }

        if in_comment
        {
            if c == '\n'
            {
                in_comment = false;
            }
            last_last_char = last_char;
            last_char = c;
            continue;
        }

        if in_multiline_comment
        {
            if c == '/' && last_char == '*'
            {
                in_multiline_comment = false;
            }
            last_last_char = last_char;
            last_char = c;
            continue;
        }

        if c == '#'
        {
            if current_word.is_empty() == false
            {
                words.push(Word { content: current_word.clone(), line: line });
                current_word.clear();
            }
            in_define = true;
            last_last_char = last_char;
            last_char = c;
            continue;
        }

        if c == '"' && last_char != '\\'
        {
            if in_string
            {
                in_string = false;
                current_word.push(c);
                words.push(Word { content: current_word.clone(), line: line });
                current_word.clear();
            }
            else
            {
                if current_word.is_empty() == false
                {
                    words.push(Word { content: current_word.clone(), line: line });
                    current_word.clear();
                }

                in_string = true;
                current_word.push(c);
            }

            last_last_char = last_char;
            last_char = c;
            continue;
        }

        if in_string
        {
            current_word.push(c);
            last_last_char = last_char;
            last_char = c;
            continue;
        }

        if c == '/' && last_char == '/'
        {
            if current_word.is_empty() == false
            {
                current_word.pop();
            }
            if current_word.is_empty() == false
            {
                words.push(Word { content: current_word.clone(), line: line });
                current_word.clear();
            }
            in_comment = true;
            last_last_char = last_char;
            last_char = c;
            continue;
        }

        if c == '*' && last_char == '/'
        {
            if current_word.is_empty() == false
            {
                current_word.pop();
            }
            if current_word.is_empty() == false
            {
                words.push(Word { content: current_word.clone(), line: line });
                current_word.clear();
            }
            in_multiline_comment = true;
            last_last_char = last_char;
            last_char = c;
            continue;
        }

        if splits.contains(&c) || splits_no_out.contains(&c)
        {
            if current_word.is_empty() == false
            {
                words.push(Word { content: current_word.clone(), line: line });
                current_word.clear();
            }

            if splits.contains(&c)
            {
                words.push(Word { content: c.to_string(), line: line });
            }

            last_last_char = last_char;
            last_char = c;
            continue;
        }


        current_word.push(c);
        last_last_char = last_char;
        last_char = c;
        continue
    }

    return words;
}
