// Copyright mzoesch. All rights reserved.

mod module_kind;
mod finder;
mod paths;
mod reflector;
mod tokenizer;

use clap::Parser;
use module_kind::ModuleKind;

/// Main CLI structure for the motor of Jafg.
#[derive(clap::Parser, Debug)]
#[command(author, version, about)]
struct Cli
{
    /// Whether to emit verbose output.
    #[arg(long = "Verbose", default_value_t = false)]
    verbose: bool,

    /// Whether to invoke pre-build tasks. This or #post_build must be set.
    #[arg(long = "PreBuild", default_value_t = false)]
    pre_build: bool,
    /// Whether to invoke post-build tasks. This or #pre_build must be set.
    #[arg(long = "PostBuild", default_value_t = false)]
    post_build: bool,

    /// The module to build for.
    #[arg(long = "Module")]
    module: String,

    /// The platform to build for.
    #[arg(long = "Platform")]
    platform: String,

    /// The architecture to build for.
    #[arg(long = "Architecture")]
    architecture: String,

    /// The target to build for.
    #[arg(long = "Target")]
    target: String,

    /// The configuration to build for.
    #[arg(long = "Configuration")]
    config: String,

    /// The kind of the module to build.
    #[arg(long = "Kind")]
    kind: ModuleKind,
}

fn main()
{
    let cwd: std::path::PathBuf = std::env::current_dir().unwrap();
    let new_cwd: String = paths::get_engine_root_dir();
    std::env::set_current_dir(&new_cwd).unwrap();

    let args: Cli = Cli::parse();

    if !args.pre_build && !args.post_build
    {
        eprintln!("Error: Either --PreBuild or --PostBuild must be set.");
        std::process::exit(1);
    }

    if args.pre_build
    {
        launch_pre_build(&args);
    }

    if args.post_build
    {
        launch_post_build(&args);
    }

    std::env::set_current_dir(cwd).unwrap();
    std::process::exit(0);
}

fn launch_pre_build(args: &Cli)
{
    if args.verbose
    {
        println!("Launching pre-build for [{}] ...", args.module);
    }

    /*
     * Construct paths so we do not have to deal with missing dirs when crating random files in
     * them - some platforms forbid to create files in dirs that don't exist.
     * Also better for the target IDE performance.
     */
    finder::ensure_path(&paths::construct_relative_gh_path(args));
    finder::ensure_path(&paths::construct_relative_gt_path(args));

    reflector::reflect_module(args);

    return;
}

fn launch_post_build(args: &Cli)
{
    if args.verbose
    {
        println!("Launching post-build for [{}] ...", args.module);
    }

    return;
}
