// Copyright mzoesch. All rights reserved.

use walkdir::WalkDir;
use crate::core::finder;
use crate::core::paths;
use crate::build_tool::core::{BuildTarget, JPacketUnit};
use crate::build_tool::tokenizer;
use crate::build_tool::tokenizer::{Token};

pub(crate) fn reflect_module(b: &BuildTarget)
{
    let dir_internal: String = format!("{}/Internal", b.module.get_functional_rel_source_dir());
    let dir_public: String = format!("{}/Public", b.module.get_functional_rel_source_dir());
    // Relative to the engine root directory.
    let mut files: Vec<String> = Vec::new();

    if finder::exists_dir(&dir_internal)
    {
        for entry in WalkDir::new(dir_internal).into_iter().filter_map(|e| e.ok())
        {
            if entry.path().is_file() && entry.path().extension().unwrap().to_str().unwrap() == "h"
            {
                files.push(paths::to_posix_path(entry.path().to_str().unwrap()));
            }
        }
    }
    if finder::exists_dir(&dir_public)
    {
        for entry in WalkDir::new(dir_public).into_iter().filter_map(|e| e.ok())
        {
            if entry.path().is_file() && entry.path().extension().unwrap().to_str().unwrap() == "h"
            {
                files.push(paths::to_posix_path(entry.path().to_str().unwrap()));
            }
        }
    }

    let mut units: Vec<Option<JPacketUnit>> = Vec::new();

    for f in files
    {
        units.push(reflect_file(b, &f));
    }

    println!("Reflecting module packets: {}", units.len());

    return;
}

fn reflect_file(b: &BuildTarget, file: &str) -> Option<JPacketUnit>
{
    if file == "Engine/Engine/Source/Public/Engine/ObjectMacros.h"
    {
        // Very, very special file that declares a lot of stuff. But sadly confuses our tokenizer...
        // It would take simply too much time to implement special rules that are commonly accepted over all files
        // so that false tokenizing does not occur. But we do not really need that - just makes everything slower and
        // more complicated. So we just hardcode the ignore here.
        return None;
    }

    let tokens: Vec<Token> = tokenizer::tokenize_file(file);

    if tokens.len() > 0
    {
        println!("[{}]: Number of tokens: {}", file, tokens.len());
    }

    return None;
}
