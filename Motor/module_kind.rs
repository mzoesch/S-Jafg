// Copyright mzoesch. All rights reserved.

#[derive(Debug, Clone, clap::ValueEnum)]
#[clap(rename_all = "lowercase")]
pub enum ModuleKind
{
    Unknown,
    Shared,
    Static,
    Launch,
    Plugin,
}

impl ModuleKind
{
    #[allow(dead_code)]
    pub fn is_shared_weak(&self) -> bool
    {
        match self
        {
            ModuleKind::Shared => true,
            ModuleKind::Plugin => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn is_unknown(&self) -> bool
    {
        match self
        {
            ModuleKind::Unknown => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn is_shared(&self) -> bool
    {
        match self
        {
            ModuleKind::Shared => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn is_static(&self) -> bool
    {
        match self
        {
            ModuleKind::Static => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn is_launch(&self) -> bool
    {
        match self
        {
            ModuleKind::Launch => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn is_plugin(&self) -> bool
    {
        match self
        {
            ModuleKind::Plugin => true,
            _ => false,
        }
    }

    #[allow(dead_code)]
    pub fn from_str(kind: &str) -> Option<ModuleKind>
    {
        return match kind
        {
            "Unknown" => Some(ModuleKind::Unknown),
            "Shared" => Some(ModuleKind::Shared),
            "SharedLib" => Some(ModuleKind::Shared),
            "SHARED_LIBRARY" => Some(ModuleKind::Shared),
            "Static" => Some(ModuleKind::Static),
            "StaticLib" => Some(ModuleKind::Static),
            "STATIC_LIBRARY" => Some(ModuleKind::Static),
            "Launch" => Some(ModuleKind::Launch),
            "ConsoleApp" => Some(ModuleKind::Launch),
            "EXECUTABLE" => Some(ModuleKind::Launch),
            "Plugin" => Some(ModuleKind::Plugin),
            _ => None,
        }
    }
}
