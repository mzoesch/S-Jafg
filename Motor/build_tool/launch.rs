// Copyright mzoesch. All rights reserved.

use std::cmp::PartialEq;
use crate::Cli;
use crate::build_tool::core::BuildTarget;
use crate::build_tool::reflector::reflect_module;
use crate::core::application::{Application, BuildConfig, Module, ModuleKind, Platform, Solution, Target};
use crate::core::finder;

struct BuildTargetUnprocessed
{
    pub solution: String,
    pub platform: String,
    pub arch:     String,
    pub kind:     String,
    pub target:   String,
    pub module:   String,
}

struct _BuildTarget<'a>
{
    pub solution: Option<&'a Solution>,
    pub platform: Option<&'a Platform>,
    pub arch:     Option<String>,
    pub kind:     Option<ModuleKind>,
    pub config:   Option<&'a BuildConfig>,
    pub target:   Option<&'a Target>,
    pub module:   Option<&'a Module>,
}

enum BuildStep
{
    PreBuild,
    PostBuild,
}

impl PartialEq for BuildStep
{
    fn eq(&self, other: &Self) -> bool
    {
        match (self, other)
        {
            (BuildStep::PreBuild, BuildStep::PreBuild) => true,
            (BuildStep::PostBuild, BuildStep::PostBuild) => true,
            _ => false,
        }
    }
}

pub fn emulate(solution: &Solution)
{
    for platform in solution.platforms.iter()
    {
        for config in platform.configs.iter()
        {
            for target in config.targets.iter()
            {
                for module in target.modules.iter()
                {
                    // Obviously we cannot emulate post build tasks.
                    launch_pre_build(BuildTarget
                    {
                        solution: solution,
                        platform: platform,
                        arch: "".to_string(),
                        kind: module.kind.clone(),
                        config: config,
                        target: target,
                        module,
                    });
                }
                continue
            }
            continue
        }
        continue
    }

    return;
}

pub fn launch(app: &Application, args: &Cli)
{
    let step: BuildStep = match args.build_tool.len()
    {
        0 => panic!("No build step argument."),
        _ => match args.build_tool[0].as_str()
        {
            "pre-build" => BuildStep::PreBuild,
            "post-build" => BuildStep::PostBuild,
            _ => panic!("Invalid build step argument: [{}].", args.build_tool[0]),
        },
    };

    let mut build_target_unprocessed: BuildTargetUnprocessed = BuildTargetUnprocessed
    {
        solution: "".to_string(),
        module: "".to_string(),
        kind: "".to_string(),
        platform: "".to_string(),
        arch: "".to_string(),
        target: "".to_string(),
    };

    for arg in args.build_tool.iter().skip(1)
    {
        let parts: Vec<&str> = arg.split('=').collect();
        if parts.len() != 2
        {
            panic!("Invalid build tool argument: [{}].", arg);
        }

        match parts[0]
        {
            "SLN" => build_target_unprocessed.solution = parts[1].to_string(),
            "MODULE" => build_target_unprocessed.module = parts[1].to_string(),
            "KIND" => build_target_unprocessed.kind = parts[1].to_string(),
            "PLATFORM" => build_target_unprocessed.platform = parts[1].to_string(),
            "ARCH" => build_target_unprocessed.arch = parts[1].to_string(),
            "TARGET" => build_target_unprocessed.target = parts[1].to_string(),
            _ => panic!("Invalid build tool argument: [{}].", arg),
        }
    }

    if build_target_unprocessed.solution.len() == 0
    {
        panic!("No solution specified.");
    }
    if build_target_unprocessed.module.len() == 0
    {
        panic!("No module specified.");
    }
    if build_target_unprocessed.platform.len() == 0
    {
        panic!("No platform specified.");
    }
    if build_target_unprocessed.arch.len() == 0
    {
        panic!("No architecture specified.");
    }
    if build_target_unprocessed.target.len() == 0
    {
        panic!("No target specified.");
    }

    let mut build_target: _BuildTarget = _BuildTarget
    {
        solution: None,
        platform: None,
        arch: None,
        kind: None,
        config: None,
        target: None,
        module: None,
    };

    let parts: Vec<&str> = build_target_unprocessed.target.split("-").collect();

    build_target.solution = Option::from(app.find_solution_by_name_checked(&build_target_unprocessed.solution));
    build_target.platform = Option::from(build_target.solution.unwrap().find_platform_by_name_checked(&build_target_unprocessed.platform));
    build_target.arch     = Option::from(build_target_unprocessed.arch);
    build_target.kind     = ModuleKind::from_str(&build_target_unprocessed.kind);
    build_target.config   = Option::from(build_target.platform.unwrap().find_config_by_name_checked(parts[1]));
    build_target.target   = Option::from(build_target.config.unwrap().find_target_by_name_checked(parts[0]));
    build_target.module   = Option::from(build_target.target.unwrap().find_module_by_name_checked(&build_target_unprocessed.module));

    let out: BuildTarget = BuildTarget
    {
        solution: build_target.solution.unwrap(),
        platform: build_target.platform.unwrap(),
        arch:     build_target.arch.unwrap(),
        kind:     build_target.kind.unwrap(),
        config:   build_target.config.unwrap(),
        target:   build_target.target.unwrap(),
        module:   build_target.module.unwrap(),
    };

    if step == BuildStep::PreBuild
    {
        launch_pre_build(out);
    }
    else
    {
        launch_post_build(out);
    }

    return;
}

fn launch_pre_build(b: BuildTarget)
{
    println!("Launching pre-build for [{}] ...", b.module.name);

    // Construct paths so we do not have to deal with missing dirs when crating random files in
    // them - some platforms forbid to create files in dirs that don't exist.
    // Also better for the target IDE performance.
    finder::ensure_path(&b.solution.construct_relative_gh_path(b.platform, b.module));
    finder::ensure_path(&b.solution.construct_relative_cgh_path(b.platform, b.config, b.target, b.module));
    finder::ensure_path(&b.solution.construct_relative_gt_path(b.platform, b.module));
    finder::ensure_path(&b.solution.construct_relative_cgt_path(b.platform, b.config, b.target, b.module));

    reflect_module(&b);

    println!("Launching pre-build for [{}] ... Ok.", b.module.name);
    return;
}

fn launch_post_build(b: BuildTarget)
{
    println!("Launching post-build for [{}] ...", b.module.name);

    if b.module.kind.is_shared()
    {
        let shared_bin_dir: String = b.get_bin_dir();
        finder::check_dir(&shared_bin_dir);

        for module in b.target.modules.iter().filter(|m| m.kind.is_launch())
        {
            let launch_bin_dir: String = b.get_bin_dir_for_module(module);
            finder::ensure_path(&launch_bin_dir);

            let src_runtime_lib: String = format!("{}/{}{}", shared_bin_dir, b.module.name, b.get_shared_bin_ext());
            let dst_runtime_lib: String = format!("{}/{}{}", launch_bin_dir, b.module.name, b.get_shared_bin_ext());
            finder::check_file(&src_runtime_lib);

            let src_runtime_pdb: String = format!("{}/{}{}", shared_bin_dir, b.module.name, b.get_symbols_bin_ext()); // ok if not exists
            let dst_runtime_pdb: String = format!("{}/{}{}", launch_bin_dir, b.module.name, b.get_symbols_bin_ext());

            finder::copy_to_dir_if_different(&src_runtime_lib, &dst_runtime_lib, true);
            if finder::exists_file(&src_runtime_pdb)
            {
                finder::copy_to_dir_if_different(&src_runtime_pdb, &dst_runtime_pdb, true);
            }

            continue
        }
    }

    for module in b.target.modules.iter().filter(|m| m.kind.is_launch())
    {
        let launch_bin_dir: String = b.get_bin_dir_for_module(module);
        finder::ensure_path(&launch_bin_dir);

        for dependency in b.module.native_runtime_dependencies.iter()
        {
            finder::check_file(dependency);
            let dst_dependency: String = format!("{}/{}", launch_bin_dir, finder::get_file_name(dependency));
            finder::copy_to_dir_if_different(dependency, &dst_dependency, true);
            continue
        }

        for dependency in b.module.native_additional_runtime_dependencies.iter()
        {
            finder::check_file(dependency);
            let dst_dependency: String = format!("{}/{}", launch_bin_dir, finder::get_file_name(dependency));
            finder::copy_to_dir_if_different(dependency, &dst_dependency, true);
            continue
        }

        continue
    }

    println!("Launching post-build for [{}] ... Ok.", b.module.name);
    return;
}
