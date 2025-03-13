// Copyright mzoesch. All rights reserved.

use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize)]
pub struct Application
{
    pub solutions: Vec<Solution>,
}

impl Default for Application
{
    fn default() -> Self
    {
        Self
        {
            solutions: Vec::new(),
        }
    }
}

#[derive(Serialize, Deserialize)]
pub struct Solution
{
    pub name: String,
    pub rel_dir: String,
    pub startup: String,
    pub platforms: Vec<Platform>,
}

#[derive(Serialize, Deserialize)]
pub struct Platform
{
    pub name: String,
    pub defines: Vec<String>,
    pub configs: Vec<BuildConfig>,
}

#[derive(Serialize, Deserialize)]
pub struct BuildConfig
{
    pub name: String,
    pub defines: Vec<String>,
    pub targets: Vec<Target>,
}

#[derive(Serialize, Deserialize)]
pub struct Target
{
    pub name: String,
    pub defines: Vec<String>,
    pub runtime: String,
    pub symbols: bool,
    pub optimize: bool,
    pub modules: Vec<Module>,
}

#[derive(Serialize, Deserialize)]
pub enum ModuleKind
{
    Shared,
    Static,
    Launch,
}

#[derive(Serialize, Deserialize)]
pub struct Module
{
    pub name: String,
    pub friendly_name: String,
    pub relative_dir: String,
    pub pch: bool,
    pub pch_content: String,
    pub kind: ModuleKind,
    pub public_dependencies: Vec<String>,
    pub private_dependencies: Vec<String>,
    pub native_includes: Vec<String>,
    pub native_dependencies: Vec<String>,
    pub native_runtime_dependencies: Vec<String>,
}
