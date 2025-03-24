// Copyright mzoesch. All rights reserved.

use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize)]
#[allow(non_snake_case)]
pub struct Config
{
    pub bPredefineJafgMacros: bool,
    pub VFilesystemIgnores: Vec<String>,
}

impl Default for Config
{
    fn default() -> Self
    {
        Config
        {
            bPredefineJafgMacros: false,
            VFilesystemIgnores: Vec::new(),
        }
    }
}
