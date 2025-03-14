// Copyright mzoesch. All rights reserved.

use std::fmt::{Display};
use crate::core::application::{ModuleKind, Solution};
use crate::core::finder;
use crate::core::paths;
use crate::core::finder::ensure_file;

pub(crate) fn make_premake(solution: &Solution)
{
    make_script(solution);

    if cfg!(windows)
    {
        let premake_executable: String = format!("{}/Programs/Vendor/Premake/Bin/premake5.exe", paths::get_engine_root_dir());
        let target_script: String = format!("{}/{}/__buildSolution.lua", paths::get_engine_root_dir(), solution.get_saved_rel_dir_premake());
        finder::check_file(&premake_executable);
        finder::check_file(&target_script);
        let output = std::process::Command::new(premake_executable)
            .arg(format!("--file={}", target_script))
            .arg("vs2022")
            .output()
            .expect("Failed to run premake script");

        println!("Status: {}", output.status);
        println!("{}", String::from_utf8_lossy(&output.stdout));
        println!("{}", String::from_utf8_lossy(&output.stderr));
        if !output.status.success()
        {
            panic!("Premake failed.");
        }
    }
    else
    {
        panic!("Missing implementation for this platform.");
    }

    return;
}

fn wwi<T: Display>(builder: &mut String, indent: usize, content: T)
{
    for _ in 0..indent
    {
        builder.push_str("    ");
    }

    builder.push_str(content.to_string().as_str());
    builder.push_str("\n");

    return;
}

fn wni(builder: &mut String, content: &str)
{
    builder.push_str(content);
    builder.push_str("\n");

    return;
}

fn bool_to_lua(value: bool) -> &'static str
{
    return if value
    {
        "On"
    }
    else
    {
        "Off"
    }
}

impl ModuleKind
{
    fn to_lua(&self) -> &'static str
    {
        return match self
        {
            ModuleKind::Shared => "SharedLib",
            ModuleKind::Static => "StaticLib",
            ModuleKind::Launch => "ConsoleApp",
        }
    }
}

fn platform_to_lua_system(platform: &str) -> &'static str
{
    return match platform
    {
        "Windows" => "windows",
        "Wasm" => "windows", // emscripten, but premake doing premake things... just use windows for better intellisense. We have to rely on cmake for compiling anyway.
        _ => panic!("Unknown platform: [{}].", platform),
    }
}

fn entry_to_lua(entry: &str) -> &'static str
{
    return match entry
    {
        "WIN_MAIN" => "WinMainCRTStartup",
        "MAIN" => "",
        _ => panic!("Unknown entry point: [{}].", entry),
    }
}

fn get_host_python_executable() -> String
{
    return if cfg!(windows)
    {
        "python.exe".to_string()
    } else
    {
        "python3".to_string()
    }
}

fn expand_variables_for_step(in_step: &str) -> Vec<String>
{
    let mut out: Vec<String> = Vec::new();
    out.push(in_step.to_string());

    let mut modified: bool = true;
    while modified
    {
        modified = false;
        for mut i in 0..out.len()
        {
            let idx: Option<usize> = out[i].find("$PythonExecutable");
            if idx.is_some()
            {
                modified = true;
                let before: String = out[i].split_at(idx.unwrap()).0.to_string();
                let after: String = out[i].split_at(idx.unwrap() + "$PythonExecutable".len()).1.to_string();

                if before.len() > 0
                {
                    out[i] = format!("{}", before.to_string());
                    out.insert(i+1, "..".to_string());
                    i += 2;
                }
                else
                {
                    out.remove(i);
                }

                out.insert(i, "_WORKING_DIR".to_string());
                out.insert(i+1, "..".to_string());
                out.insert(i+2, format!("/.venv/Scripts/{}", get_host_python_executable()));

                if after.len() > 0
                {
                    out.insert(i+3, "..".to_string());
                    out.insert(i+4, after.to_string());
                }

                break;
            }

            let idx: Option<usize> = out[i].find("$PythonScript");
            if idx.is_some()
            {
                modified = true;
                let before: String = out[i].split_at(idx.unwrap()).0.to_string();
                let after: String = out[i].split_at(idx.unwrap() + "$PythonScript".len()).1.to_string();

                if before.len() > 0
                {
                    out[i] = before.to_string();
                    out.insert(i+1, "..".to_string());
                    i += 2;
                }
                else
                {
                    out.remove(i);
                }

                out.insert(i, "_WORKING_DIR".to_string());
                out.insert(i+1, "..".to_string());
                out.insert(i+2, "/Program.py".to_string());

                if after.len() > 0
                {
                    out.insert(i+3, "..".to_string());
                    out.insert(i+4, after.to_string());
                }

                break
            }

            let idx: Option<usize> = out[i].find("$RootDir");
            if idx.is_some()
            {
                modified = true;
                let before: String = out[i].split_at(idx.unwrap()).0.to_string();
                let after: String = out[i].split_at(idx.unwrap() + "$RootDir".len()).1.to_string();

                if before.len() > 0
                {
                    out[i] = before.to_string();
                    out.insert(i+1, "..".to_string());
                    i += 2;
                }
                else
                {
                    out.remove(i);
                }

                out.insert(i, "_WORKING_DIR".to_string());

                if after.len() > 0
                {
                    out.insert(i+1, "..".to_string());
                    out.insert(i+2, after.to_string());
                }

                break
            }

            let idx: Option<usize> = out[i].find("$Kind");
            if idx.is_some()
            {
                modified = true;
                let before: String = out[i].split_at(idx.unwrap()).0.to_string();
                let after: String = out[i].split_at(idx.unwrap() + "$Kind".len()).1.to_string();

                if before.len() > 0
                {
                    out[i] = before.to_string();
                    out.insert(i+1, "..".to_string());
                    i += 2;
                }
                else
                {
                    out.remove(i);
                }

                out.insert(i, "%{cfg.kind}".to_string());

                if after.len() > 0
                {
                    out.insert(i+1, "..".to_string());
                    out.insert(i+2, after.to_string());
                }

                break
            }

            let idx: Option<usize> = out[i].find("$Platform");
            if idx.is_some()
            {
                modified = true;
                let before: String = out[i].split_at(idx.unwrap()).0.to_string();
                let after: String = out[i].split_at(idx.unwrap() + "$Platform".len()).1.to_string();

                if before.len() > 0
                {
                    out[i] = before.to_string();
                    out.insert(i+1, "..".to_string());
                    i += 2;
                }
                else
                {
                    out.remove(i);
                }

                out.insert(i, "%{cfg.platform}".to_string());

                if after.len() > 0
                {
                    out.insert(i+1, "..".to_string());
                    out.insert(i+2, after.to_string());
                }

                break
            }

            let idx: Option<usize> = out[i].find("$Arch");
            if idx.is_some()
            {
                modified = true;
                let before: String = out[i].split_at(idx.unwrap()).0.to_string();
                let after: String = out[i].split_at(idx.unwrap() + "$Arch".len()).1.to_string();

                if before.len() > 0
                {
                    out[i] = before.to_string();
                    out.insert(i+1, "..".to_string());
                    i += 2;
                }
                else
                {
                    out.remove(i);
                }

                out.insert(i, "%{cfg.architecture}".to_string());

                if after.len() > 0
                {
                    out.insert(i+1, "..".to_string());
                    out.insert(i+2, after.to_string());
                }

                break
            }

            let idx: Option<usize> = out[i].find("$Target");
            if idx.is_some()
            {
                modified = true;
                let before: String = out[i].split_at(idx.unwrap()).0.to_string();
                let after: String = out[i].split_at(idx.unwrap() + "$Target".len()).1.to_string();

                if before.len() > 0
                {
                    out[i] = before.to_string();
                    out.insert(i+1, "..".to_string());
                    i += 2;
                }
                else
                {
                    out.remove(i);
                }

                out.insert(i, "%{cfg.buildcfg}".to_string());

                if after.len() > 0
                {
                    out.insert(i+1, "..".to_string());
                    out.insert(i+2, after.to_string());
                }

                break
            }

            continue
        }

        continue
    }

    let mut real_out: Vec<String> = Vec::new();
    for step in out.iter()
    {
        if step == ".."
        {
            real_out.push(step.to_string());
        }
        else if step == "_WORKING_DIR"
        {
            real_out.push(step.to_string());
        }
        else
        {
            real_out.push(format!("'{}'", step));
        }
    }

    return real_out;
}

fn make_script(solution: &Solution)
{
    let premake_root_dir: String = solution.get_saved_rel_dir_premake();
    println!("Making premake script at [{}].", premake_root_dir);
    let premake_file: String = format!("{}/__buildSolution.lua", premake_root_dir);
    ensure_file(premake_file.as_str());

    let mut content: String = String::new();
    let b: &mut String = &mut content;

    wni(b, "-- Copyright mzoesch. All rights reserved.");
    wni(b, "-- This file is automatically generated by Jafg and can be deleted with no fear.\n");

    wni(b, "local host = os.host()");
    wni(b, "local scriptPath = debug.getinfo(2, \"S\").source:sub(2)");
    wni(b, "local scriptDir = path.getdirectory(scriptPath)");
    wni(b, "printf('Working insdie directory: [%s].', _WORKING_DIR)");
    wni(b, "language 'C++'");
    wni(b, "cppdialect 'C++20'");

    wni(b, "workspace 'Jafg'");
    wwi(b, 1, format!("startproject '{}'", solution.startup));
    wwi(b, 1, "platforms {");
    for platform in solution.platforms.iter()
    {
        wwi(b, 2, format!("'{}',", platform.name));
    }
    wwi(b, 2, "}");

    let mut premake_configs: Vec<String> = Vec::new();
    for platform in solution.platforms.iter()
    {
        for config in platform.configs.iter()
        {
            for target in config.targets.iter()
            {
                let config: String = target.make_build_target_name(config);
                if !premake_configs.contains(&config)
                {
                    premake_configs.push(config);
                }
            }
        }
    }
    wwi(b, 1, "configurations {");
    for premake_config in premake_configs.iter()
    {
        wwi(b, 2, format!("'{}',", premake_config));
    }
    wwi(b, 2, "}");

    for platform in solution.platforms.iter()
    {
        wwi(b, 1, format!("filter {{ 'platforms:{}' }}", platform.name));
        wwi(b, 2, format!("system '{}'", platform_to_lua_system(&platform.name)));
        wwi(b, 2, format!("systemversion '{}'", platform.version));
        wwi(b, 2, format!("architecture '{}'", platform.architecture));
        wwi(b, 2, format!("toolset '{}'", platform.toolset));
        wwi(b, 2, "defines {");
        for define in platform.defines.iter()
        {
            wwi(b, 3, format!("'{}',", define));
        }
        wwi(b, 3, "}");
        wwi(b, 2, "linkoptions {");
        for lnk_flag in platform.lnk_flags.iter()
        {
            wwi(b, 3, format!("'{}',", lnk_flag));
        }
        wwi(b, 3, "}");
        wwi(b, 2, "buildoptions {");
        for build_flag in platform.build_flags.iter()
        {
            wwi(b, 3, format!("'{}',", build_flag));
        }
        wwi(b, 3, "}");
        wwi(b, 1 , "filter { }")
    }

    for platform in solution.platforms.iter()
    {
        for config in platform.configs.iter()
        {
            for target in config.targets.iter()
            {
                let native_name: String = target.make_build_target_name(config);
                wwi(b, 1, format!("filter {{ 'platforms:{}', 'configurations:{}' }}", platform.name, native_name));
                wwi(b, 2, format!("runtime '{}'", config.runtime));
                wwi(b, 2, format!("symbols '{}'", bool_to_lua(config.symbols)));
                wwi(b, 2, format!("optimize '{}'", bool_to_lua(config.optimize)));
                wwi(b, 2, "defines {");
                for define in config.defines.iter()
                {
                    wwi(b, 3, format!("'{}',", define));
                }
                for define in target.defines.iter()
                {
                    wwi(b, 3, format!("'{}',", define));
                }
                wwi(b, 3, "}");
                wwi(b, 1, "filter { }")
            }
        }
    }

    for mm in solution.max_modules.iter()
    {
        wwi(b, 0, format!("group '{}'", mm.get_functional_parent_rel_dir()));
        wwi(b, 0, format!("project '{}'", mm.name));
        wwi(b, 1, format!("location '{}'", mm.get_functional_rel_dir()));
        wwi(b, 1, "rtti 'Off'");
        wwi(b, 1, format!("targetdir (_WORKING_DIR .. '/{}/%{{cfg.platform}}-%{{cfg.architecture}}/%{{cfg.buildcfg}}/{}')", paths::PATH_BINARIES, mm.get_functional_rel_dir()));
        wwi(b, 1, format!("objdir (_WORKING_DIR .. '/{}/%{{cfg.platform}}-%{{cfg.architecture}}/%{{cfg.buildcfg}}/{}')", paths::PATH_INTERMEDIATES, mm.get_functional_rel_dir()));

        for platform in solution.platforms.iter()
        {
            for config in platform.configs.iter()
            {
                for target in config.targets.iter()
                {
                    for module in target.modules.iter()
                    {
                        if mm.is_equal_str(&module.relative_dir) == false
                        {
                            continue;
                        }

                        let native_name: String = target.make_build_target_name(config);
                        wwi(b, 1, format!("filter {{ 'platforms:{}', 'configurations:{}' }}", platform.name, native_name));

                        let transitive_dependencies: Vec<String> = module.get_dependencies_transitive(target);

                        wwi(b, 2, format!("kind '{}'", module.kind.to_lua()));

                        wwi(b, 2, "files {");
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}/**.md',", module.get_functional_rel_dir()));
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}/**.py',", module.get_functional_rel_dir()));
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}/Source/**.h',", module.get_functional_rel_dir()));
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}/Source/**.hpp',", module.get_functional_rel_dir()));
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}/Source/**.c',", module.get_functional_rel_dir()));
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}/Source/**.cpp',", module.get_functional_rel_dir()));
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}/**.generated.h',", solution.construct_relative_gh_path(platform, module)));
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}/**.generated.cpp',", solution.construct_relative_gt_path(platform, module)));
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}/**.generated.h',", solution.construct_relative_cgh_path(platform, config, target, module)));
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}/**.generated.cpp',", solution.construct_relative_cgt_path(platform, config, target, module)));
                        wwi(b, 3, "}");

                        wwi(b, 2, "vpaths {");
                        wwi(b, 3, "['/*'] = {");
                        wwi(b, 4, format!("_WORKING_DIR .. '/{}/*.md',", module.get_functional_rel_dir()));
                        wwi(b, 4, format!("_WORKING_DIR .. '/{}/*.py',", module.get_functional_rel_dir()));
                        wwi(b, 4, format!("_WORKING_DIR .. '/{}/**.generated.h',", solution.construct_relative_root_saved_dir(platform, module)));
                        wwi(b, 4, format!("_WORKING_DIR .. '/{}/**.generated.cpp',", solution.construct_relative_root_saved_dir(platform, module)));
                        wwi(b, 4, "},");
                        wwi(b, 3, "['Source/*'] = {");
                        wwi(b, 4, format!("_WORKING_DIR .. '/{}/Source/**.md',", module.get_functional_rel_dir()));
                        wwi(b, 4, format!("_WORKING_DIR .. '/{}/Source/**.py',", module.get_functional_rel_dir()));
                        wwi(b, 4, format!("_WORKING_DIR .. '/{}/Source/**.h',", module.get_functional_rel_dir()));
                        wwi(b, 4, format!("_WORKING_DIR .. '/{}/Source/**.hpp',", module.get_functional_rel_dir()));
                        wwi(b, 4, format!("_WORKING_DIR .. '/{}/Source/**.c',", module.get_functional_rel_dir()));
                        wwi(b, 4, format!("_WORKING_DIR .. '/{}/Source/**.cpp',", module.get_functional_rel_dir()));
                        wwi(b, 4, "}");
                        wwi(b, 3, "}");

                        wwi(b, 2, "includedirs {");
                        wwi(b, 3, "_WORKING_DIR .. '/',");
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}/Source/Internal',", module.get_functional_rel_dir()));
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}/Source/Public',", module.get_functional_rel_dir()));
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}',", solution.construct_relative_gh_path(platform, module)));
                        wwi(b, 3, format!("_WORKING_DIR .. '/{}',", solution.construct_relative_cgh_path(platform, config, target, module)));
                        for dependency in transitive_dependencies.iter()
                        {
                            wwi(b, 3, format!("_WORKING_DIR .. '/{}/Source/Public',", target.find_module_by_name_checked(dependency).get_functional_rel_dir()));
                            wwi(b, 3, format!("_WORKING_DIR .. '/{}',", solution.construct_relative_gh_path(platform, target.find_module_by_name_checked(dependency))));
                        }
                        for dependency in module.native_includes.iter()
                        {
                            wwi(b, 3, format!("_WORKING_DIR .. '/{}',", dependency));
                        }
                        wwi(b, 3, "}");

                        wwi(b, 2, "links {");
                        for dependency in transitive_dependencies.iter()
                        {
                            wwi(b, 3, format!("'{}',", target.find_module_by_name_checked(dependency).name));
                        }
                        for dependency in module.native_dependencies.iter()
                        {
                            wwi(b, 3, format!("_WORKING_DIR .. '/{}',", dependency));
                        }
                        wwi(b, 3, "}");

                        wwi(b, 2, "defines {");
                        wwi(b, 3, format!("'{}_API=PLATFORM_CALLSPEC_OUT',", module.name.to_uppercase()));
                        wwi(b, 3, format!("'{}_EXTERN=PLATFORM_EXTERNSPEC_OUT',", module.name.to_uppercase()));
                        for target_module in target.modules.iter()
                        {
                            for dependency in transitive_dependencies.iter()
                            {
                                if target_module.is_equal_str(dependency)
                                {
                                    if target_module.kind.is_launch()
                                    {
                                        panic!("Dependency [{}] is not allowed for launch module [{}].", dependency, target_module.relative_dir);
                                    }
                                    else if target_module.kind.is_shared()
                                    {
                                        wwi(b, 3, format!("'{}_API=PLATFORM_CALLSPEC_IN',", target_module.name.to_uppercase()));
                                        wwi(b, 3, format!("'{}_EXTERN=PLATFORM_EXTERNSPEC_IN',", target_module.name.to_uppercase()));
                                    }
                                    else
                                    {
                                        wwi(b, 3, format!("'{}_API=',", target_module.name.to_uppercase()));
                                        wwi(b, 3, format!("'{}_EXTERN=',", target_module.name.to_uppercase()));
                                    }
                                }
                            }
                        }
                        wwi(b, 3, "}");

                        if module.pch
                        {
                            wwi(b, 2, format!("pchheader '{}'", paths::FILE_PCH_H));
                            wwi(b, 2, format!("pchsource( _WORKING_DIR .. '/{}/{}' )", solution.construct_relative_cgt_path(platform, config, target, module), paths::FILE_PCH_T));
                            wwi(b, 2, format!("filter {{ 'platforms:{}', 'configurations:{}', 'files:**.cpp' }}", platform.name, native_name));
                            wwi(b, 2, format!("forceincludes {{ '{}' }}", paths::FILE_PCH_H));
                            wwi(b, 2, format!("filter {{ 'platforms:{}', 'configurations:{}', 'files:**.c' }}", platform.name, native_name));
                            wwi(b, 2, "flags { 'NoPCH' }");
                            wwi(b, 2, format!("filter {{ 'platforms:{}', 'configurations:{}' }}", platform.name, native_name));
                        }

                        if module.kind.is_launch() && entry_to_lua(&module.entry) != ""
                        {
                            wwi(b, 2, format!("entrypoint '{}'", entry_to_lua(&module.entry)));
                        }

                        wwi(b, 2, "prebuildcommands {");
                        for _step in module.pre_builds.iter()
                        {
                            let steps: Vec<String> = expand_variables_for_step(&_step);
                            let compound: String = steps.join(" ");
                            wwi(b, 3, compound);
                            continue
                        }
                        wwi(b, 3, "}");
                    }
                    wwi(b, 2, "filter { }");
                }
            }
        }

        wwi(b, 0, "group ''");
        continue
    }

    finder::write_to_file_if_different(&premake_file, true, &content);

    return;
}
