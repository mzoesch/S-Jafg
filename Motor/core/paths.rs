// Copyright mzoesch. All rights reserved.

pub const PATH_WORKSPACE: &str = "jafg.jafgworkspace";
pub const PATH_SAVED_DIR: &str = "Saved";
pub const PATH_MOD_CACHE: &str = "Saved/structure.jproject";

pub const PATH_BINARIES: &str = "Binaries";
pub const PATH_INTERMEDIATES: &str = "Intermediate";

pub const FILE_PCH_H: &str = "ModulePch.generated.h";
pub const FILE_PCH_T: &str = "ModulePch.generated.cpp";

pub const FILE_ROOT_PLUGIN_IDENT: &str = "root.plugin.jafg";

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
