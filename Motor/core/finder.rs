// Copyright mzoesch. All rights reserved.

use std::io::Read;

/// Ensures that a file exists at the given path. If not found, the file is created.
pub fn ensure_file(path: &str)
{
    if !std::path::Path::new(path).exists()
    {
        println!("No such file or directory: [{}]. Creating it.", path);
        std::fs::File::create(path).unwrap();
    }

    return;
}

pub fn read_file(path: &str) -> String
{
    let file: std::fs::File = std::fs::File::open(path).unwrap();
    let mut reader: std::io::BufReader<std::fs::File> = std::io::BufReader::new(file);
    let mut string: String = String::new();
    reader.read_to_string(&mut string).unwrap();
    return string
}
