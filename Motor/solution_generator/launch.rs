// Copyright mzoesch. All rights reserved.

use std::cmp::PartialEq;
use crate::Cli;
use crate::core::application::{Application, Solution};
use crate::solution_generator::make_cmake;
use crate::solution_generator::make_premake;
use crate::core::paths;
use crate::core::finder;

pub fn launch(app: &Application, args: &Cli)
{
    let emulate: bool = args.solution_generator.contains(&"EmulateCompiler".to_string());

    if args.solution_generator.contains(&"GenerateAll".to_string())
    {
        for solution in app.solutions.iter()
        {
            generate_solution(solution, emulate);
        }
    }
    else if args.solution_generator.iter().any(|x| x.contains(&"GEN=".to_string()))
    {
        let name: &str = &args.solution_generator[0][4..];
        let sln: &Solution = app.find_solution_by_name_checked(name);
        generate_solution(sln, emulate);
    }

    println!("================================");
    println!("Finished generating solutions.");
    println!("================================");

    return;
}

fn generate_solution(solution: &Solution, emulate: bool)
{
    println!("Generating solution [{}] ...", solution.name);

    make_pch_for_all_modules(solution);

    make_cmake::make_script(solution);
    if cfg!(target_os = "windows")
    {
        make_premake::make_script(solution);
    }

    if emulate
    {
        crate::build_tool::launch::emulate(solution);
    }

    make_cmake::make_cmake(solution);
    if cfg!(target_os = "windows")
    {
        make_premake::make_premake(solution);
    }

    return;
}

fn make_pch_for_all_modules(solution: &Solution)
{
    for platform in solution.platforms.iter()
    {
        for config in platform.configs.iter()
        {
            for target in config.targets.iter()
            {
                for module in target.modules.iter()
                {
                    let path_h: String = format!("{}/{}", solution.construct_relative_cgh_path(platform, config, target, module), paths::FILE_PCH_H);
                    let path_t: String = format!("{}/{}", solution.construct_relative_cgt_path(platform, config, target, module), paths::FILE_PCH_T);

                    if module.pch
                    {
                        finder::write_to_file_if_different(&path_h, true, &module.pch_content);
                        finder::write_to_file_if_different(&path_t, true, &format!("#include \"{}\"\n", paths::FILE_PCH_H));
                    }
                    else
                    {
                        finder::write_to_file_if_different(&path_t, true, "");
                        finder::write_to_file_if_different(&path_h, true, "");
                    }

                    continue
                }
                continue
            }
            continue
        }
        continue
    }

    return;
}

fn get_host_python_executable() -> String
{
    return if cfg!(target_os = "windows")
    {
        "/.venv/Scripts/python.exe".to_string()
    } 
    else
    {
        "/.venv/bin/python3".to_string()
    }
}

pub(crate) enum ScriptType
{
    Premake,
    Cmake,
}

impl PartialEq for ScriptType 
{
    fn eq(&self, other: &Self) -> bool
    {
        return match (self, other)
        {
            (ScriptType::Premake, ScriptType::Premake) => true,
            (ScriptType::Cmake, ScriptType::Cmake) => true,
            _ => false,
        }
    }
}

pub(crate) fn expand_variables_for_step(in_step: &str, script_type: ScriptType) -> Vec<String>
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
                    if script_type == ScriptType::Premake
                    {
                        out.insert(i+1, "..".to_string());
                        i += 1;
                    }
                    i += 1;
                }
                else
                {
                    out.remove(i);
                }
                
                if script_type == ScriptType::Premake
                {
                    out.insert(i, "_WORKING_DIR".to_string());
                    out.insert(i+1, "..".to_string());
                    out.insert(i+2, format!("{}", get_host_python_executable()));

                    if after.len() > 0
                    {
                        out.insert(i+3, "..".to_string());
                        out.insert(i+4, after.to_string());
                    }
                }
                else if script_type == ScriptType::Cmake
                {
                    out.insert(i, format!("${{REAL_ENGINE_ROOT_DIR}}{}", get_host_python_executable()));
                    if after.len() > 0
                    {
                        out.insert(i+1, after.to_string());
                    }
                }
                else
                {
                    panic!("Unknown script type.");
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
                    if script_type == ScriptType::Premake
                    {
                        out.insert(i+1, "..".to_string());
                        i += 1;
                    }
                    i += 1;
                }
                else
                {
                    out.remove(i);
                }

                if script_type == ScriptType::Premake
                {
                    out.insert(i, "_WORKING_DIR".to_string());
                    out.insert(i+1, "..".to_string());
                    out.insert(i+2, "/Program.py".to_string());

                    if after.len() > 0
                    {
                        out.insert(i+3, "..".to_string());
                        out.insert(i+4, after.to_string());
                    }
                }
                else if script_type == ScriptType::Cmake
                {
                    out.insert(i, format!("${{REAL_ENGINE_ROOT_DIR}}/Program.py"));
                    if after.len() > 0
                    {
                        out.insert(i+1, after.to_string());
                    }
                }
                else
                {
                    panic!("Unknown script type.");
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
                    if script_type == ScriptType::Premake
                    {
                        out.insert(i+1, "..".to_string());
                        i += 1;
                    }
                    i += 1;
                }
                else
                {
                    out.remove(i);
                }

                out.insert(i, match script_type
                {
                    ScriptType::Premake => {"_WORKING_DIR".to_string()},
                    ScriptType::Cmake => {"${REAL_ENGINE_ROOT_DIR}".to_string()},
                });

                if after.len() > 0
                {
                    if script_type == ScriptType::Premake
                    {
                        out.insert(i+1, "..".to_string());
                        out.insert(i+2, after.to_string());
                    }
                    else if script_type == ScriptType::Cmake
                    {
                        out.insert(i+1, after.to_string());
                    }
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
                    if script_type == ScriptType::Premake
                    {
                        out.insert(i+1, "..".to_string());
                        i += 1;
                    }
                    i += 1;
                }
                else
                {
                    out.remove(i);
                }
                
                out.insert(i, match script_type 
                {
                    ScriptType::Premake => {"%{cfg.kind}".to_string()},
                    ScriptType::Cmake => {"${CUR_TARGET_KIND}".to_string()},
                });

                if after.len() > 0
                {
                    if script_type == ScriptType::Premake
                    {
                        out.insert(i+1, "..".to_string());
                        out.insert(i+2, after.to_string());
                    }
                    else if script_type == ScriptType::Cmake
                    {
                        out.insert(i+1, after.to_string());
                    }
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
                    if script_type == ScriptType::Premake
                    {
                        out.insert(i+1, "..".to_string());
                        i += 1;
                    }
                    i += 1;
                }
                else
                {
                    out.remove(i);
                }
                
                out.insert(i, match script_type 
                {
                    ScriptType::Premake => {"%{cfg.platform}".to_string()},
                    ScriptType::Cmake => {"${JAFG_TARGET_PLATFORM}".to_string()},
                });

                if after.len() > 0
                {
                    if script_type == ScriptType::Premake
                    {
                        out.insert(i+1, "..".to_string());
                        out.insert(i+2, after.to_string());
                    }
                    else if script_type == ScriptType::Cmake
                    {
                        out.insert(i+1, after.to_string());
                    }
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
                    if script_type == ScriptType::Premake
                    {
                        out.insert(i+1, "..".to_string());
                        i += 1;
                    }
                    i += 1;
                }
                else
                {
                    out.remove(i);
                }

                out.insert(i, match script_type 
                {
                    ScriptType::Premake => {"%{cfg.architecture}".to_string()},
                    ScriptType::Cmake => {"${JAFG_TARGET_ARCHITECTURE}".to_string()},
                });

                if after.len() > 0
                {
                    if script_type == ScriptType::Premake
                    {
                        out.insert(i+1, "..".to_string());
                        out.insert(i+2, after.to_string());
                    }
                    else if script_type == ScriptType::Cmake
                    {
                        out.insert(i+1, after.to_string());
                    }
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
                    if script_type == ScriptType::Premake
                    {
                        out.insert(i+1, "..".to_string());
                        i += 1;
                    }
                    i += 1;
                }
                else
                {
                    out.remove(i);
                }

                out.insert(i, match script_type
                {
                    ScriptType::Premake => {"%{cfg.buildcfg}".to_string()},
                    ScriptType::Cmake => {"${JAFG_TARGET_CONFIGURATION}".to_string()},
                });
                
                if after.len() > 0
                {
                    if script_type == ScriptType::Premake
                    {
                        out.insert(i+1, "..".to_string());
                        out.insert(i+2, after.to_string());
                    }
                    else if script_type == ScriptType::Cmake
                    {
                        out.insert(i+1, after.to_string());
                    }
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
            if script_type == ScriptType::Premake
            {
                real_out.push(format!("'{}'", step));
            }
            else
            {
                real_out.push(step.to_string());
            }
        }
    }

    return real_out;
}
