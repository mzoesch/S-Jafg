// Copyright mzoesch. All rights reserved.

#![allow(dead_code)]

mod core;

use clap::{ArgAction, Parser};
use crate::core::application::Application;
use crate::core::paths;
use crate::core::finder;

/// Main CLI structure for the motor of Jafg.
#[derive(Parser, Debug)]
#[command(author, version, about)]
struct Cli
{
    /// Solution generator for C++ projects. Usage: [GEN=SOLUTION_NAME | GenerateAll]
    #[arg(short = 'S', long = "SolutionGenerator", action = ArgAction::Append)]
    solution_generator: Vec<String>,

    /// Build tool working before and after module compilation.
    #[arg(short = 'B', long = "BuildTool", action = ArgAction::Append)]
    build_tool: Vec<String>,

    /// Emulate MSVC, emscripten, ... compiler commands.
    #[arg(short = 'E', long = "EmulationTool", action = ArgAction::Append)]
    emulation_tool: Vec<String>,

    /// Does nothing. For debugging purposes.
    #[arg(long = "DoNothing")]
    do_nothing: bool,
}

fn main()
{
    let cwd: std::path::PathBuf = std::env::current_dir().unwrap();
    let new_cwd: String = paths::get_engine_root_dir();
    println!("Changing working directory from [{}] to [{}]", cwd.to_str().unwrap(), new_cwd);
    std::env::set_current_dir(&new_cwd).unwrap();

    let args: Cli = Cli::parse();
    let mut app: Application = Default::default();

    load_workspace(&mut app);
    route_to_subprogram(&mut app, args);

    print!("Popping working directory from [{}] to [{}]", new_cwd, cwd.to_str().unwrap());
    std::env::set_current_dir(cwd).unwrap();
    std::process::exit(0);
}

fn load_workspace(app: &mut Application)
{
    if app.solutions.len() > 0
    {
        panic!("Workspace already loaded.");
    }

    finder::ensure_file(paths::PATH_MOD_CACHE);
    let cache: String = finder::read_file(paths::PATH_MOD_CACHE);
    if cache.len() == 0
    {
        println!("No cache found. Skipping load.");
        return;
    }

    let cached_app: Result<Application, serde_json::Error> = serde_json::from_str(&cache);
    if cached_app.is_err()
    {
        println!("Failed to load cache.");
        cached_app.unwrap();
        return;
    }

    *app = cached_app.unwrap();

    return;
}

fn route_to_subprogram(_app: &mut Application, args: Cli)
{
    println!("SolutionGenerator: {:?}", args.solution_generator);
    println!("BuildTool: {:?}", args.build_tool);
    println!("EmulationTool: {:?}", args.emulation_tool);
    println!("DoNothing: {:?}", args.do_nothing);
}
