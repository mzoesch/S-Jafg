// Copyright mzoesch. All rights reserved.

use crate::Cli;

#[allow(dead_code)]
pub const PATH_WORKSPACE: &str = "jafg.jafgworkspace";
#[allow(dead_code)]
pub const PATH_SAVED_DIR: &str = "Saved";

#[allow(dead_code)]
pub const PATH_BINARIES: &str = "bin";
#[allow(dead_code)]
pub const PATH_INTERMEDIATES: &str = "Temp/int";

#[allow(dead_code)]
pub const FILE_PCH_H: &str = "ModulePch.generated.h";
#[allow(dead_code)]
pub const FILE_PCH_T: &str = "ModulePch.generated.cpp";

#[allow(dead_code)]
pub const FILE_ROOT_PLUGIN_IDENT: &str = "root.plugin.jafg";

#[allow(dead_code)]
pub fn get_engine_root_dir() -> String
{
    let exe: std::path::PathBuf = std::env::current_exe().unwrap();
    let exe_dir: std::path::PathBuf = exe.parent().unwrap().to_path_buf();

    let mut out: std::path::PathBuf = exe_dir.clone();
    loop
    {
        let mut found: bool = false;
        for entry in std::fs::read_dir(&out).unwrap()
        {
            let entry: std::fs::DirEntry = entry.unwrap();
            let path: std::path::PathBuf = entry.path();
            if path.ends_with(PATH_WORKSPACE)
            {
                found = true;
                break;
            }
        }

        if found
        {
            break;
        }

        out = out.parent().unwrap().to_path_buf();
    }

    return out.to_str().unwrap().to_string();
}

#[allow(dead_code)]
pub fn to_absolute_path(path: &str) -> String
{
    let mut result: String = String::new();
    if path.starts_with("/")
    {
        result.push_str(path);
    }
    else
    {
        result.push_str(get_engine_root_dir().as_str());
        result.push('/');
        result.push_str(path);
    }

    return result;
}

#[allow(dead_code)]
pub fn to_posix_path(path: &str) -> String
{
    let mut result: String = String::new();
    for c in path.chars()
    {
        if c == '\\'
        {
            result.push('/');
        }
        else
        {
            result.push(c);
        }

        continue
    }

    return result;
}

#[allow(dead_code)]
pub fn construct_relative_gh_path(args: &Cli) -> String
{
    return format!("{PATH_INTERMEDIATES}/{}-{}/{}-{}/{}/gh",
        args.platform,
        args.architecture,
        args.target,
        args.config,
        args.module,
    );
}

#[allow(dead_code)]
pub fn construct_relative_gt_path(args: &Cli) -> String
{
    return format!("{PATH_INTERMEDIATES}/{}-{}/{}-{}/{}/gt",
       args.platform,
       args.architecture,
       args.target,
       args.config,
       args.module,
    );
}

#[allow(dead_code)]
pub fn get_relative_source_dir(args: &Cli) -> String
{
    return format!("{}/src", args.module);
}

#[allow(dead_code)]
pub fn get_relative_source_dir_public(args: &Cli) -> String
{
    return format!("{}/include", args.module);
}

#[allow(dead_code)]
pub fn get_relative_source_dir_test(args: &Cli) -> String
{
    return format!("{}/tests", args.module);
}
