// Copyright mzoesch. All rights reserved.

use std::io::Read;
use walkdir::WalkDir;

/// Ensures that a file exists at the given path. If not found, the file is created.
#[allow(dead_code)]
pub fn ensure_file(path: &str)
{
    ensure_path(std::path::Path::new(path).parent().unwrap().to_str().unwrap());

    if !std::path::Path::new(path).exists()
    {
        println!("[{}]: No such file. Creating it.", path);
        std::fs::File::create(path).unwrap();
    }

    return;
}

#[allow(dead_code)]
pub fn check_file(path: &str)
{
    if std::path::Path::new(path).exists()
    {
        return;
    }

    panic!("[{}]: No such file.", path);
}

#[allow(dead_code)]
pub fn check_dir(path: &str)
{
    if std::path::Path::new(path).exists()
    {
        return;
    }

    panic!("[{}]: No such directory.", path);
}

#[allow(dead_code)]
pub fn ensure_path(path: &str)
{
    if !std::path::Path::new(path).exists()
    {
        println!("[{}]: No such directory. Creating it.", path);
        std::fs::create_dir_all(path).unwrap();
    }

    return;
}

#[allow(dead_code)]
pub fn get_file_name(path: &str) -> String
{
    return std::path::Path::new(path).file_name().unwrap().to_str().unwrap().to_string();
}

#[allow(dead_code)]
pub fn read_file(path: &str) -> String
{
    ensure_file(path);

    let file: std::fs::File = std::fs::File::open(path).unwrap();
    let mut reader: std::io::BufReader<std::fs::File> = std::io::BufReader::new(file);
    let mut string: String = String::new();
    reader.read_to_string(&mut string).unwrap();
    return string
}

#[allow(dead_code)]
pub fn exists_file(path: &str) -> bool
{
    return std::path::Path::new(path).exists();
}

#[allow(dead_code)]
pub fn exists_dir(path: &str) -> bool
{
    return std::path::Path::new(path).exists();
}

#[allow(dead_code)]
pub fn delete_file(path: &str) -> bool
{
    if std::path::Path::new(path).exists()
    {
        std::fs::remove_file(path).unwrap();
        return true;
    }

    return false;
}

#[allow(dead_code)]
pub fn write_to_file_if_different(path: &str, emit: bool, content: &str) -> bool
{
    ensure_path(std::path::Path::new(path).parent().unwrap().to_str().unwrap());

    if content.len() == 0
    {
        if delete_file(path)
        {
            if emit
            {
                println!("[{}]: New file content is empty. Deleting file.", path);
            }
            return true;
        }
        return false;
    }

    ensure_file(path);

    let cur_content: String = read_file(path);
    if cur_content.len() != content.len()
    {
        if emit
        {
            println!("[{}]: New content length differs from current content. Writing new content.", path);
        }
        std::fs::write(path, content).unwrap();
        return true;
    }

    if cur_content != content
    {
        if emit
        {
            println!("[{}]: New content differs from current content. Writing new content.", path);
        }
        std::fs::write(path, content).unwrap();
        return true;
    }

    return false;
}

#[allow(dead_code)]
pub fn copy_to_dir_if_different(src: &str, dst: &str, emit: bool) -> bool
{
    ensure_path(std::path::Path::new(dst).parent().unwrap().to_str().unwrap());

    if !std::path::Path::new(src).exists()
    {
        panic!("[{}]: No such file.", src);
    }

    if !std::path::Path::new(dst).exists()
    {
        if emit
        {
            println!("[{}]: No such file. Copying it.", dst);
        }
        std::fs::copy(src, dst).unwrap();
        return true;
    }

    let src_content: Vec<u8> = std::fs::read(src).unwrap();
    let dst_content: Vec<u8> = std::fs::read(dst).unwrap();

    if src_content.len() != dst_content.len()
    {
        if emit
        {
            println!("[{}]: New content length differs from current content. Copying new content.", dst);
        }
        std::fs::copy(src, dst).unwrap();
        return true;
    }

    if src_content != dst_content
    {
        if emit
        {
            println!("[{}]: New content differs from current content. Copying new content.", dst);
        }
        std::fs::copy(src, dst).unwrap();
        return true;
    }

    return false;
}

/// Files relative to the engine root directory.
#[allow(dead_code)]
pub fn get_files_recursive(dir: &str) -> Vec<String>
{
    check_dir(dir);

    let mut files: Vec<String> = Vec::new();
    for entry in WalkDir::new(dir).into_iter().filter_map(|e| e.ok())
    {
        if entry.path().is_file()
        {
            files.push(entry.path().to_str().unwrap().to_string());
        }
    }

    return files;
}
