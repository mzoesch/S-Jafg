// Copyright mzoesch. All rights reserved.

use crate::Cli;
use crate::paths;
use crate::finder;
use crate::tokenizer;
use walkdir::WalkDir;

use tokenizer::{Token, TokenType, SUPER_CLASS};

/// A unit of multiple packets.
pub struct JPacketUnit
{
    pub name: String,
    pub packets: Vec<JPacket>,
}

impl JPacketUnit
{
    pub fn new(name: String) -> JPacketUnit
    {
        JPacketUnit
        {
            name: name,
            packets: Vec::new(),
        }
    }
}

/// A packet for a call of the source.
pub struct JPacket
{
    pub name: String,
    pub line: u32,
    pub args: Vec<String>,
    pub callback: Box<dyn Fn(
        /* h_file_id */ &String,
        /* h_builder */ &mut String,
        /* t_builder */ &mut String,
        /* self */ &JPacket,
    )>,
}

pub(crate) fn reflect_module(args: &Cli)
{
    let dir_public: String = paths::get_relative_source_dir_public(args);

    /* Relative to the engine root directory. */
    let mut files: Vec<String> = Vec::new();

    if finder::exists_dir(&dir_public)
    {
        for entry in WalkDir::new(dir_public).into_iter().filter_map(|e| e.ok())
        {
            if entry.path().is_file() && entry.path().extension().is_some() && entry.path().extension().unwrap().to_str().unwrap() == "h"
            {
                files.push(paths::to_posix_path(entry.path().to_str().unwrap()));
            }
        }
    }

    let mut units: Vec<JPacketUnit> = Vec::new();
    let mut removed_units: Vec<String> = Vec::new();

    for f in files
    {
        match reflect_file(args, &f)
        {
            Some(x) => units.push(x),
            None => removed_units.push(f),
        }
    }

    let mut writes: i32 = 0;
    let mut deletes: i32 = 0;

    for unit in units
    {
        writes += write_packet(args, unit);
    }
    for unit in removed_units
    {
        deletes += delete_packet(args, unit);
    }

    if writes > 0 || deletes > 0
    {
        if args.verbose
        {
            print!("Launching pre-build for [{}] ... ", args.module);
            if writes > 0
            {
                print!("w[{}] ", writes);
            }
            if deletes > 0
            {
                print!("d[{}]", deletes);
            }
            println!();
        }
    }

    return;
}

#[allow(dead_code)]
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

#[allow(dead_code)]
fn get_all_namespaces(token: &str) -> String
{
    let i: Option<usize> = token.rfind("::");
    return if i.is_some()
    {
        token[..i.unwrap()].to_string()
    }
    else
    {
        "".to_string()
    }
}

#[allow(dead_code)]
fn remove_suffixed_scoped_operator_because_gcc_does_gcc_unjustifiable_shenanigans(spaced_identifier: &str) -> String
{
    /*
     * Why gcc why?? This literally costed me days of fucking refactoring the whole fucking reflection system.
     * @see ~Jafg/Core/CoreMacros.h: JAFG_JOIN_SCOPE_RESOLUTION_OUTER_XXX
     */
    if spaced_identifier.ends_with("::")
    {
        return spaced_identifier[..spaced_identifier.len()-2].to_string();
    }
    return spaced_identifier.to_string();
}

#[allow(dead_code)]
fn find_next_token(tokens: &Vec<Token>, skip: usize, ty: &TokenType) -> Option<usize>
{
    for i in skip..tokens.len()
    {
        if tokens[i].ty == *ty
        {
            return Some(i);
        }
    }

    return None;
}

#[allow(dead_code)]
fn find_next_token_checked<'a>(tokens: &'a Vec<Token>, skip: usize, ty: &TokenType) -> &'a Token
{
    return match find_next_token(tokens, skip, &ty)
    {
        Some(x) => &tokens[x],
        None => panic!("Expected [{:?}].", &ty),
    }
}

#[allow(dead_code)]
fn find_prev_token(tokens: &Vec<Token>, skip: usize, ty: &TokenType) -> Option<usize>
{
    for i in (0..skip).rev()
    {
        if tokens[i].ty == *ty
        {
            return Some(i);
        }
    }

    return None;
}

#[allow(dead_code)]
fn find_prev_token_checked<'a>(tokens: &'a Vec<Token>, skip: usize, ty: &TokenType) -> &'a Token
{
    return match find_prev_token(tokens, skip, &ty)
    {
        Some(x) => &tokens[x],
        None => panic!("Expected [{:?}].", &ty),
    }
}

#[allow(dead_code)]
fn find_prev_token_by_list(tokens: &Vec<Token>, skip: usize, ty: &Vec<TokenType>) -> Option<usize>
{
    for i in (0..skip).rev()
    {
        if ty.contains(&tokens[i].ty)
        {
            return Some(i);
        }
    }

    return None;
}

#[allow(dead_code)]
fn find_prev_token_by_list_checked<'a>(tokens: &'a Vec<Token>, skip: usize, ty: &Vec<TokenType>) -> &'a Token
{
    return match find_prev_token_by_list(tokens, skip, ty)
    {
        Some(x) => &tokens[x],
        None => panic!("Expected [{:?}].", &ty),
    }
}

#[allow(dead_code)]
fn get_generated_file_stub(file: &str) -> Option<String>
{
    return match std::path::Path::new(file).file_name()
    {
        Some(x) => match x.to_str()
        {
            Some(y) =>
            {
                let splits: Vec<&str> = y.split(".").collect();
                Some(splits.iter().take(splits.len() - 1).map(|x| *x).collect::<Vec<&str>>().join("."))
            }
            None => None,
        }
        None => None,
    }
}

#[allow(dead_code)]
fn get_generated_file_stub_checked(file: &str) -> String
{
    return match get_generated_file_stub(file)
    {
        Some(x) => x,
        None => panic!("Could not get the generated file stub from [{:?}].", file),
    }
}

fn reflect_file(args: &Cli, file: &str) -> Option<JPacketUnit>
{
    if     file == "Engine/include/Engine/CxxClassMacros.h"
        || file == "Engine/include/Engine/CxxRecordMacros.h"
    {
        // Very, very special file that declares a lot of stuff. But sadly confuses our tokenizer...
        // It would take simply too much time to implement special rules that are commonly accepted over all files
        // so that false tokenizing does not occur. But we do not really need that - just makes everything slower and
        // more complicated. So we just hardcode the ignore here.
        return None;
    }

    let mut unit: JPacketUnit = JPacketUnit::new(file.to_string());
    let tokens: Vec<Token> = tokenizer::tokenize_file(file);

    for idx in 0..tokens.len()
    {
        let t: &Token = &tokens[idx];
        if t.ty.is_pragma()
        {
            match add_pragma(args, file, &tokens, idx, t)
            {
                Some(x) => unit.packets.push(x),
                None => (),
            }
        }
        else if t.ty.is_class_declaration() || t.ty.is_world_obj_declaration() || t.ty.is_widget_declaration() || t.ty.is_widget_declaration_with_factory()
        {
            match add_class(file, &tokens, idx, t)
            {
                Some(x) => unit.packets.push(x),
                None => (),
            }
        }
        else if t.ty.is_class_field()
        {
            match add_class_field(file, &tokens, idx, t)
            {
                Some(x) => unit.packets.push(x),
                None => (),
            }
        }
        else if t.ty.is_class_body()
        {
            match add_class_body(file, &tokens, idx, t)
            {
                Some(x) => unit.packets.push(x),
                None => (),
            }
        }

        continue
    }

    return match unit.packets.len()
    {
        0 => None,
        _ => Some(unit),
    }
}

fn write_packet(args: &Cli, unit: JPacketUnit) -> i32
{
    let mut changes: i32 = 0;

    let stub: String = get_generated_file_stub_checked(&unit.name);
    let gh_file: String = format!("{}/{}.generated.h", paths::construct_relative_gh_path(args), stub);
    let gt_file: String = format!("{}/{}.generated.cpp", paths::construct_relative_gt_path(args), stub);
    finder::ensure_file(&gh_file);
    finder::ensure_file(&gt_file);

    let mut h_builder: String = String::new();
    let mut t_builder: String = String::new();

    let mut h_file_id: String = gh_file
        .replace("~", "")
        .replace("/", "___")
        .replace(".", "__")
        .replace("-", "_");

    h_builder.push_str(&format!(r##"
// Copyright mzoesch. All rights reserved.

/*-----------------------------------------------------------------------------
   This header file was generated by the Jafg build tool.
   Do not modify it manually.
-----------------------------------------------------------------------------*/

#ifdef PRIVATE_JAFG_GENERATED_HEADER_{h_file_id}
    #error "Generated header [{gh_file}] included multiple times. Missing #pragma once or #ifndef guard?"
#endif /* PRIVATE_JAFG_GENERATED_HEADER_{h_file_id} */
#define PRIVATE_JAFG_GENERATED_HEADER_{h_file_id}

#ifdef JAFG_PRIVATE_FILE_ID
    #undef JAFG_PRIVATE_FILE_ID
#endif /* JAFG_PRIVATE_FILE_ID */
#define JAFG_PRIVATE_FILE_ID            FILE_ID_{h_file_id}

/*-----------------------------------------------------------------------------
   BEGIN Generated header content.
-----------------------------------------------------------------------------*/

"##,
    ));

    let mut include_path = unit.name;
    include_path = match include_path.find("/include/")
    {
        Some(i) => include_path[i+9..].to_string(),
        None => panic!("Could not find the include path for [{}].", include_path),
    };

    t_builder.push_str(&format!(r##"
// Copyright mzoesch. All rights reserved.

/*-----------------------------------------------------------------------------
    This translation file was generated by the Jafg build tool.
    Do not modify it manually.
-----------------------------------------------------------------------------*/

#ifdef PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION
    #undef PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION
#endif /* PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION */
#define PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION               1

#include "{}"

#ifdef PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION
    #undef PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION
#endif /* PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION */


/*-----------------------------------------------------------------------------
    BEGIN Generated translation content.
-----------------------------------------------------------------------------*/

"##,
        include_path
    ));

    h_file_id = format!("FILE_ID_{}", h_file_id);

    for packet in unit.packets.iter()
    {
        (*packet.callback)(&h_file_id, &mut h_builder, &mut t_builder, &packet);
    }

    /*
     * May happen if we do not do this:
     * [warning]: backslash-newline at the end of file
     */
    h_builder.push_str("\n\n");
    t_builder.push_str("\n\n");

    if finder::write_to_file_if_different(&gh_file, false, &h_builder)
    {
        changes += 1;
    }
    if finder::write_to_file_if_different(&gt_file, false, &t_builder)
    {
        changes += 1;
    }

    return changes;
}

fn delete_packet(args: &Cli, unit: String) -> i32
{
    let stub: String = get_generated_file_stub_checked(&unit);
    let gh_file: String = format!("{}/{}.generated.h", paths::construct_relative_gh_path(args), stub);
    let gt_file: String = format!("{}/{}.generated.cpp", paths::construct_relative_gt_path(args), stub);

    let mut count: i32 = 0;

    if finder::delete_file(&gh_file)
    {
        count += 1;
    }
    if finder::delete_file(&gt_file)
    {
        count += 1;
    }

    return count;
}

fn add_pragma(_args: &Cli, file: &str, tokens: &Vec<Token>, i: usize, t: &Token) -> Option<JPacket>
{
    assert_eq!(t.ty.is_pragma(), true);
    assert_eq!(tokens[i].ty.is_pragma(), true);

    if t.content == "\"NextIsBaseJxxClass\""
    {
        return None;
    }

    panic!("[{}:{}]: Unknown pragma: [{}].", file, tokens[i].line, t.content);
}

#[allow(non_snake_case)]
fn on_add_class__VA_ARGS__(packet: &JPacket) -> String
{
    let mut out: String = String::new();

    for arg in packet.args.iter().skip(1)
    {
        let config: bool = arg.contains("@C");
        let default_only: bool = arg.contains("@D");
        let member: String = arg.replace("@C", "").replace("@D", "");

        if default_only
        {
            panic!("Default only fields are no longer supported on: [{}::{}].", packet.name, member);
        }

        if config && default_only
        {
            out.push_str(&format!(r##"                                                                   \
Ref->GetMutableClassFieldsDangerous().emplace_back(                                                      \
    /* Field Name   */ "{member}",                                                                       \
    /* Field Setter */ LSetClassField::CreateMemberFunction(Ref, &_TObj::_SetField_{member}),            \
    /* Field Getter */ LGetClassField::CreateMemberFunction(Ref, &_TObj::_GetField_{member}),            \
    /* Field Malloc */ LCustomMallocClassField::CreateMemberFunction(Ref, &_TObj::_MallocField_{member}) \
);                                                                                                       \
"##));
        }
        else if config && !default_only
        {
            out.push_str(&format!(r##"                                                        \
.reflexive_emplace_back(::Jafg::LJxxClassField{{                                           \
    /* Field Name   */ "{member}",                                                            \
    /* Field Setter */ PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION_SET(_JAFG_OHGCRCHD_TObj, {member}),\
    /* Field Getter */ PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION_GET(_JAFG_OHGCRCHD_TObj,{member}) \
}})                                                                                            \
"##));
        }
        else if !config && default_only
        {
            panic!("Only config files are currently supported on: [{}::{}].", packet.name, member);
            out.push_str(&format!(r##"                                                                   \
Ref->GetMutableClassFieldsDangerous().emplace_back(                                                      \
    /* Field Name   */ "{member}",                                                                       \
    /* Field Setter */ nullptr,                                                                          \
    /* Field Getter */ nullptr,                                                                          \
    /* Field Malloc */ LCustomMallocClassField::CreateMemberFunction(Ref, &_TObj::_MallocField_{member}) \
);                                                                                                       \
"##));
        }
    }

    return out;
}

fn add_class(file: &str, tokens: &Vec<Token>, i: usize, t: &Token) -> Option<JPacket>
{
    assert_eq!(t.ty.is_class_declaration() || t.ty.is_world_obj_declaration() || t.ty.is_widget_declaration() || t.ty.is_widget_declaration_with_factory(), true);
    assert_eq!(tokens[i].ty.is_class_declaration() || tokens[i].ty.is_world_obj_declaration() || tokens[i].ty.is_widget_declaration() || tokens[i].ty.is_widget_declaration_with_factory(), true);

    // Just a more user-friendly error message - if the body was missing is kinda hard to make
    // sense of the C++ compilation error message.
    find_next_token_checked(tokens, i + 1, &TokenType::ClassBody);

    let mut namespaces: Vec<String> = Vec::new();
    for token in tokens.iter().take(i)
    {
        if token.ty.is_namespace_push()
        {
            namespaces.push(token.content.clone());
        }
        else if token.ty.is_namespace_pop()
        {
            namespaces.pop();
        }
    }
    let mut namespaces_str: String = namespaces.join("::");
    if namespaces_str.len() > 0
    {
        namespaces_str = format!("::{}", namespaces_str);
    }

    let mut additional_args: Vec<String> = t.info.clone();
    for field in tokens.iter().skip(i + 2)
    {
        if field.ty != TokenType::ClassField
        {
            break;
        }
        let mut arg: String = String::new();
        if field.info.contains(&"Config".to_string())
        {
            arg.push_str("@C");
        }
        // if field.info.contains(&"DefaultOnly".to_string())
        // {
        //     arg.push_str("@D");
        // }
        arg.push_str(&field.content);
        additional_args.push(arg);
        continue
    }

    return Some(JPacket
    {
        name: format!("{}::{}", namespaces_str, t.content),
        line: t.line,
        args: additional_args,
        callback: match t.ty
        {
            TokenType::ClassDeclaration => Box::new(|h_file_id, h_builder, t_builder, self_packet|
            {
                h_builder.push_str(&format!(r##"
#ifdef {}_{}_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [{}] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* {}_{}_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION */
#define {}_{}_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION(...)              \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        /* My Class Name */          {}, /* ORIGIN VALUE: {} */                                \
        /* My Class Spaces */        {}, /* ORIGIN ARG VALUE: {} */                            \
        /* Super Class Name */       {}, /* ORIGIN VALUE: {} */                                \
        /* Line */                   {},                                                       \
        /* Additional Class Flags */ __VA_ARGS__                                               \
    )
"##,
                    h_file_id, self_packet.line,
                    self_packet.name,
                    h_file_id, self_packet.line,
                    h_file_id, self_packet.line,
                    remove_all_namespaces(&self_packet.name), self_packet.name,
                    get_all_namespaces(&self_packet.name), self_packet.name,
                    remove_all_namespaces(&self_packet.args[0]), self_packet.args[0],
                    self_packet.line,
                ));

                t_builder.push_str(&format!(r##"
PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION( \
    /* My Class Name */          {}, /* ORIGIN VALUE: {} */                               \
    /* My Class Spaces */        {}, /* ORIGIN ARG VALUE: {} */                           \
    /* Line Of Declaration */    {},                                                      \
    /* Super Class Name */       {},                                                      \
    /* __VA_ARGS__ */            {}                                                       \
)
"##,
                                            remove_all_namespaces(&self_packet.name), self_packet.name,
                                            get_all_namespaces(&self_packet.name), self_packet.name,
                                            self_packet.line,
                                            self_packet.args[SUPER_CLASS],
                                            on_add_class__VA_ARGS__(&self_packet),
                ));

                return
            }),
            TokenType::WorldObjectDeclaration => Box::new(|h_file_id, h_builder, t_builder, self_packet|
            {
                h_builder.push_str(&format!(r##"
#ifdef {}_{}_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [{}] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* {}_{}_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION */
#define {}_{}_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION(...)              \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        /* My Class Name */          {}, /* ORIGIN VALUE: {} */                                \
        /* My Class Spaces */        {}, /* ORIGIN ARG VALUE: {} */                            \
        /* Super Class Name */       {}, /* ORIGIN VALUE: {} */                                \
        /* Line */                   {},                                                       \
        /* Additional Class Flags */ __VA_ARGS__                                               \
    )
"##,
                                            h_file_id, self_packet.line,
                                            self_packet.name,
                                            h_file_id, self_packet.line,
                                            h_file_id, self_packet.line,
                                            remove_all_namespaces(&self_packet.name), self_packet.name,
                                            get_all_namespaces(&self_packet.name), self_packet.name,
                                            remove_all_namespaces(&self_packet.args[0]), self_packet.args[0],
                                            self_packet.line,
                ));

                t_builder.push_str(&format!(r##"
PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION( \
    /* My Class Name */          {}, /* ORIGIN VALUE: {} */                               \
    /* My Class Spaces */        {}, /* ORIGIN ARG VALUE: {} */                           \
    /* Line Of Declaration */    {},                                                      \
    /* Super Class Name */       {},                                                      \
    /* __VA_ARGS__ */            {}                                                       \
)
"##,
                                            remove_all_namespaces(&self_packet.name), self_packet.name,
                                            get_all_namespaces(&self_packet.name), self_packet.name,
                                            self_packet.line,
                                            self_packet.args[SUPER_CLASS],
                                            on_add_class__VA_ARGS__(&self_packet),
                ));

                return
            }),
            TokenType::WidgetDeclaration => Box::new(|h_file_id, h_builder, t_builder, self_packet|
            {
                h_builder.push_str(&format!(r##"
#ifdef {}_{}_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION
    #if!PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [{}] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* {}_{}_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION */
#define {}_{}_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION(...)               \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        /* My Class Name */          {}, /* ORIGIN VALUE: {} */                                 \
        /* My Class Spaces */        {}, /* ORIGIN ARG VALUE: {} */                             \
        /* Super Class Name */       {}, /* ORIGIN VALUE: {} */                                 \
        /* Line */                   {},                                                        \
        /* Additional Class Flags */ __VA_ARGS__                                                \
    )
"##,
                    h_file_id, self_packet.line,
                    self_packet.name,
                    h_file_id, self_packet.line,
                    h_file_id, self_packet.line,
                    remove_all_namespaces(&self_packet.name), self_packet.name,
                    get_all_namespaces(&self_packet.name), self_packet.name,
                    remove_all_namespaces(&self_packet.args[0]), self_packet.args[0],
                    self_packet.line,
                ));

                t_builder.push_str(&format!(r##"
PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION( \
    /* My Class Name */          {}, /* ORIGIN VALUE: {} */                               \
    /* My Class Spaces */        {}, /* ORIGIN ARG VALUE: {} */                           \
    /* Line Of Declaration */    {},                                                      \
    /* Super Class Name */       {},                                                      \
    /* __VA_ARGS__ */            {}                                                       \
)
"##,
                                            remove_all_namespaces(&self_packet.name), self_packet.name,
                                            get_all_namespaces(&self_packet.name), self_packet.name,
                                            self_packet.line,
                                            &self_packet.args[SUPER_CLASS],
                                            on_add_class__VA_ARGS__(&self_packet),
                ));

                return
            }),
            TokenType::WidgetDeclarationWithFactory => Box::new(|h_file_id, h_builder, t_builder, self_packet|
            {
                h_builder.push_str(&format!(r##"
#ifdef {}_{}_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [{}] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* {}_{}_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION */
#define {}_{}_MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION(TFactoryTy, ...)                \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        /* My Class Name */          {}, /* ORIGIN VALUE: {} */                                              \
        /* My Class Spaces */        {}, /* ORIGIN ARG VALUE: {} */                                          \
        /* Super Class Name */       {}, /* ORIGIN VALUE: {} */                                              \
        /* Line */                   {},                                                                     \
        /* Factory Type */           TFactoryTy,                                                             \
        /* Additional Class Flags */ __VA_ARGS__                                                             \
    )
"##,
                    h_file_id, self_packet.line,
                    self_packet.name,
                    h_file_id, self_packet.line,
                    h_file_id, self_packet.line,
                    remove_all_namespaces(&self_packet.name), self_packet.name,
                    get_all_namespaces(&self_packet.name), self_packet.name,
                    remove_all_namespaces(&self_packet.args[0]), self_packet.args[0],
                    self_packet.line,
                ));

                t_builder.push_str(&format!(r##"
PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION( \
    /* My Class Name */          {}, /* ORIGIN VALUE: {} */                               \
    /* My Class Spaces */        {}, /* ORIGIN ARG VALUE: {} */                           \
    /* Line Of Declaration */    {},                                                      \
    /* Super Class Name */       {},                                                      \
    /* __VA_ARGS__ */            {}                                                       \
)
"##,
                                            remove_all_namespaces(&self_packet.name), self_packet.name,
                                            get_all_namespaces(&self_packet.name), self_packet.name,
                                            self_packet.line,
                                            self_packet.args[SUPER_CLASS],
                                            on_add_class__VA_ARGS__(&self_packet),
                ));

                return
            }),
            _ => panic!("[{}:{}]: Unknown class type [{:?}].", file, t.line, &t.ty),
        }
    });
}

fn add_class_field(_file: &str, tokens: &Vec<Token>, i: usize, t: &Token) -> Option<JPacket>
{
    assert_eq!(t.ty.is_class_field(), true);
    assert_eq!(tokens[i].ty.is_class_field(), true);

    return Some(JPacket
    {
        name: t.content.clone(),
        line: t.line,
        args: t.info.clone(),
        callback: Box::new(|h_file_id, h_builder, _t_builder, self_packet|
            {
                h_builder.push_str(&format!(r##"
#ifdef {}_{}_MY_GENERATED_CLASS_FIELD_DECLARATION
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [{}] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* {}_{}_MY_GENERATED_CLASS_FIELD_DECLARATION */
#define {}_{}_MY_GENERATED_CLASS_FIELD_DECLARATION(...)         \
"##,
                    h_file_id, self_packet.line,
                    self_packet.name,
                    h_file_id, self_packet.line,
                    h_file_id, self_packet.line,
                ));

                let mut analyzed_count: usize = 0;

                if self_packet.args.contains(&"Config".to_string())
                {
                    h_builder.push_str(&format!(r##"                        \
PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_FIELD_DECLARATION_Config(     \
        /* My Class Member */ {}                                            \
    )                                                                       \
"##,
                        self_packet.name,
                    ));

                    analyzed_count += 1;
                }

//                 if self_packet.args.contains(&"DefaultOnly".to_string())
//                 {
//                     h_builder.push_str(&format!(r##"                         \
// PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_FIELD_DECLARATION_DefaultOnly( \
//         /* My Class Member */ {}                                             \
//     )                                                                        \
// "##,
//                         self_packet.name,
//                     ));
//                 }

                if analyzed_count != self_packet.args.len()
                {
                    panic!("[{}]: Could not analyze all class field args on [{}]. Analyzed [{}/{}] args: [{:?}].",
                        h_file_id, self_packet.name,
                        analyzed_count, self_packet.args.len(), self_packet.args
                        );
                }

                return;
            }
        ),
    });
}

fn add_class_body(file: &str, tokens: &Vec<Token>, i: usize, t: &Token) -> Option<JPacket>
{
    assert_eq!(t.ty.is_class_body(), true);
    assert_eq!(tokens[i].ty.is_class_body(), true);

    let class_decl: &Token = find_prev_token_by_list_checked(tokens, i, &vec![TokenType::ClassDeclaration, TokenType::WorldObjectDeclaration, TokenType::WidgetDeclaration, TokenType::WidgetDeclarationWithFactory]);

    return Some(JPacket
    {
        name: class_decl.content.to_string(),
        line: t.line,
        args: vec![class_decl.info[SUPER_CLASS].clone(), class_decl.line.to_string(), tokens[i].content.clone()],
        callback: match class_decl.ty
        {
            TokenType::ClassDeclaration => Box::new(|h_file_id, h_builder, _t_builder, self_packet|
            {
                h_builder.push_str(&format!(r##"
#ifdef {}_{}_MY_GENERATED_CLASS_BODY
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [{}] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* {}_{}_MY_GENERATED_CLASS_BODY */
#define {}_{}_MY_GENERATED_CLASS_BODY(...)                            \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_IMPL(          \
        /* My Class Name */            {}, /* ORIGIN VALUE: {} */     \
        /* My Class Spaces */          {}, /* ORIGIN ARG VALUE: {} */ \
        /* Super Class Name */         {},                            \
        /* Construction Helper Line */ {},                            \
        __VA_ARGS__ /* API */                                         \
    )
"##,
                    h_file_id, self_packet.line,
                    self_packet.name,
                    h_file_id, self_packet.line,
                    h_file_id, self_packet.line,
                    remove_all_namespaces(&self_packet.name), self_packet.name,
                    get_all_namespaces(&self_packet.name), self_packet.name,
                    self_packet.args[SUPER_CLASS],
                    self_packet.args[1],
                ));
            }),
            TokenType::WorldObjectDeclaration => Box::new(|h_file_id, h_builder, _t_builder, self_packet|
            {
                    h_builder.push_str(&format!(r##"
#ifdef {}_{}_MY_GENERATED_CLASS_BODY
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [{}] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* {}_{}_MY_GENERATED_CLASS_BODY */
#define {}_{}_MY_GENERATED_CLASS_BODY(...)                            \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WORLD_BODY_IMPL(          \
        /* My Class Name */            {}, /* ORIGIN VALUE: {} */     \
        /* My Class Spaces */          {}, /* ORIGIN ARG VALUE: {} */ \
        /* Super Class Name */         {},                            \
        /* Construction Helper Line */ {},                            \
        __VA_ARGS__ /* API */                                         \
    )
"##,
                    h_file_id, self_packet.line,
                    self_packet.name,
                    h_file_id, self_packet.line,
                    h_file_id, self_packet.line,
                    remove_all_namespaces(&self_packet.name), self_packet.name,
                    get_all_namespaces(&self_packet.name), self_packet.name,
                    self_packet.args[SUPER_CLASS],
                    self_packet.args[1],
                ));
            }),
            TokenType::WidgetDeclaration => Box::new(|h_file_id, h_builder, _t_builder, self_packet|
            {
                h_builder.push_str(&format!(r##"
#ifdef {}_{}_MY_GENERATED_CLASS_BODY
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [{}] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* {}_{}_MY_GENERATED_CLASS_BODY */
#define {}_{}_MY_GENERATED_CLASS_BODY(...)                            \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_BODY_IMPL(         \
        /* My Class Name */            {}, /* ORIGIN VALUE: {} */     \
        /* My Class Spaces */          {}, /* ORIGIN ARG VALUE: {} */ \
        /* Super Class Name */         {},                            \
        /* Construction Helper Line */ {},                            \
        __VA_ARGS__ /* API */                                         \
    )
"##,
                    h_file_id, self_packet.line,
                    self_packet.name,
                    h_file_id, self_packet.line,
                    h_file_id, self_packet.line,
                    remove_all_namespaces(&self_packet.name), self_packet.name,
                    get_all_namespaces(&self_packet.name), self_packet.name,
                    self_packet.args[SUPER_CLASS],
                    self_packet.args[1],
                ));
            }),
            TokenType::WidgetDeclarationWithFactory => Box::new(|h_file_id, h_builder, _t_builder, self_packet|
            {
                h_builder.push_str(&format!(r##"
#ifdef {}_{}_MY_GENERATED_CLASS_BODY
    #if !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS
        #error "Generated packet [{}] included multiple times. Missing #pragma once or #ifndef guard?"
    #endif /* !PRIVATE_JAFG_IGNORE_GUARD_FOR_GENERATED_HEADERS */
#endif /* {}_{}_MY_GENERATED_CLASS_BODY */
#define {}_{}_MY_GENERATED_CLASS_BODY(...)                                 \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_BODY_IMPL( \
        /* My Class Name */            {}, /* ORIGIN VALUE: {} */          \
        /* My Class Spaces */          {}, /* ORIGIN ARG VALUE: {} */      \
        /* Super Class Name */         {},                                 \
        /* Construction Helper Line */ {},                                 \
        __VA_ARGS__ /* API */                                              \
    )
"##,
                    h_file_id, self_packet.line,
                    self_packet.name,
                    h_file_id, self_packet.line,
                    h_file_id, self_packet.line,
                    remove_all_namespaces(&self_packet.name), self_packet.name,
                    get_all_namespaces(&self_packet.name), self_packet.name,
                    self_packet.args[SUPER_CLASS],
                    self_packet.args[1],
                ));
            }),
            _ => panic!("[{}:{}]: Unknown class type [{:?}].", file, t.line, &t.ty),
        }
    });
}
