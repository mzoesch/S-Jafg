// Copyright mzoesch. All rights reserved.

use std::io::Read;

/// Ensures that a file exists at the given path. If not found, the file is created.
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

pub fn check_file(path: &str)
{
    if std::path::Path::new(path).exists()
    {
        return;
    }

    panic!("[{}]: No such file.", path);
}

pub fn check_dir(path: &str)
{
    if std::path::Path::new(path).exists()
    {
        return;
    }

    panic!("[{}]: No such directory.", path);
}

pub fn ensure_path(path: &str)
{
    if !std::path::Path::new(path).exists()
    {
        println!("[{}]: No such directory. Creating it.", path);
        std::fs::create_dir_all(path).unwrap();
    }

    return;
}

pub fn read_file(path: &str) -> String
{
    ensure_file(path);

    let file: std::fs::File = std::fs::File::open(path).unwrap();
    let mut reader: std::io::BufReader<std::fs::File> = std::io::BufReader::new(file);
    let mut string: String = String::new();
    reader.read_to_string(&mut string).unwrap();
    return string
}

pub fn exists_file(path: &str) -> bool
{
    return std::path::Path::new(path).exists();
}

pub fn exists_dir(path: &str) -> bool
{
    return std::path::Path::new(path).exists();
}

pub fn delete_file(path: &str) -> bool
{
    if std::path::Path::new(path).exists()
    {
        std::fs::remove_file(path).unwrap();
        return true;
    }

    return false;
}

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
