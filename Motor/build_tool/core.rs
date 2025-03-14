// Copyright mzoesch. All rights reserved.

use crate::core::application::{BuildConfig, Module, Platform, Solution, Target};

pub struct BuildTarget<'a>
{
    pub solution: &'a Solution,
    pub platform: &'a Platform,
    pub config: &'a BuildConfig,
    pub target: &'a Target,
    pub module: &'a Module,
}

/// A unit of multiple packets.
pub struct JPacketUnit
{
    pub name: String,
    pub packets: Vec<JPacket>,
}

/// A packet for a call of the source.
pub struct JPacket
{
    pub name: String,
    pub line: u32,
    pub args: Vec<String>,
    pub callback: Option<Box<dyn Fn(&Vec<String>) -> String>>,
}
