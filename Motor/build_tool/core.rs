// Copyright mzoesch. All rights reserved.

use crate::core::application::{BuildConfig, Module, ModuleKind, Platform, Solution, Target};

pub struct BuildTarget<'a>
{
    pub solution: &'a Solution,
    pub platform: &'a Platform,
    pub arch:     String,
    pub kind:     ModuleKind,
    pub config:   &'a BuildConfig,
    pub target:   &'a Target,
    pub module:   &'a Module,
}

impl BuildTarget<'_>
{
    pub fn get_root_bin_dir(&self) -> String
    {
        format!("Binaries/{}-{}/{}-{}", self.platform.name, self.arch, self.target.name, self.config.name)
    }

    pub fn get_bin_dir(&self) -> String
    {
        format!("{}/{}", self.get_root_bin_dir(), self.module.get_functional_rel_dir())
    }

    pub fn get_bin_dir_for_module(&self, module: &Module) -> String
    {
        format!("{}/{}", self.get_root_bin_dir(), module.get_functional_rel_dir())
    }

    pub fn get_shared_counterpart(&self) -> String
    {
        return self.platform.get_shared_counterpart_suffix();
    }

    pub fn get_shared_bin_ext(&self) -> String
    {
        return self.platform.get_shared_bin_suffix();
    }

    pub fn get_static_bin_ext(&self) -> String
    {
        return self.platform.get_static_bin_suffix();
    }

    pub fn get_symbols_bin_ext(&self) -> String
    {
        return self.platform.get_symbols_bin_extension();
    }
}

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
