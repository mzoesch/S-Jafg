// Copyright mzoesch. All rights reserved.

use std::collections::HashMap;
use serde::{Serialize, Deserialize};
use std::fmt::{Display};
use crate::core::application::{BuildConfig, Module, ModuleKind, Platform, Solution, Target};
use crate::core::paths;
use crate::finder;
use crate::solution_generator::launch::ScriptType;

fn expand_cmake_vars(content: &mut String, s: &Solution, p: &Platform, c: &BuildConfig, t: &Target)
{
    *content = content.replace("$SOLUTION_NAME", &s.name);
    *content = content.replace("$PLATFORM_NAME", &p.name);
    *content = content.replace("$ARCHITECTURE_NAME", &p.architecture);
    *content = content.replace("$NATIVE_BUILD_TARGET", format!("{}-{}", t.name, c.name).as_str());

    return;
}

fn expand_toolset_to_compiler_c(toolset: &str) -> &str
{
    return match toolset
    {
        "msc" => "cl.exe",
        "em" => "emcc",
        "clang" => "clang",
        _ => panic!("Unrecognized toolset [{}].", toolset),
    };
}

fn expand_toolset_to_compiler_cxx(toolset: &str) -> &str
{
    return match toolset
    {
        "msc" => "cl.exe",
        "em" => "em++",
        "clang" => "clang++",
        _ => panic!("Unrecognized toolset [{}].", toolset),
    };
}

pub(crate) fn make_cmake(solution: &Solution)
{
    for platform in solution.platforms.iter()
    {
        for config in platform.configs.iter() 
        {
            for target in config.targets.iter() 
            {
                let platform_suffix: &str = match platform.name.as_str()
                {
                    "Wasm" => "-wasm",
                    _ => "",
                };

                // bat
                {

                    let target_path: String = format!("{}/{}-{}/{}-{}", solution.get_saved_rel_dir_cmake(), platform.name, platform.architecture, target.name, config.name);
                    let gen_file: String = format!("{}/generate.bat", target_path);
                    let build_file: String = format!("{}/build.bat", target_path);
                    let clean_build_file: String = format!("{}/clean_build.bat", target_path);

                    let mut gen_bat: String = finder::read_file(&format!("Programs/Shell/Stubs/CmakeGenerateForTarget{}.bat", &platform_suffix));
                    expand_cmake_vars(&mut gen_bat, solution, platform, config, target);
                    gen_bat = match platform.toolset.as_str()
                    {
                        "msc" => gen_bat.replace("$PRE_EXECUTION_COMMAND", &finder::read_file("Programs/Shell/Stubs/MakeEnvForLatestMsvc.bat")),
                        _ => gen_bat.replace("$PRE_EXECUTION_COMMAND", ""),
                    };
                    finder::write_to_file_if_different(&gen_file, true, &gen_bat);

                    let mut build_bat: String = finder::read_file("Programs/Shell/Stubs/CmakeBuildForTarget.bat");
                    expand_cmake_vars(&mut build_bat, solution, platform, config, target);
                    build_bat = match platform.toolset.as_str()
                    {
                        "msc" => build_bat.replace("$PRE_EXECUTION_COMMAND", &finder::read_file("Programs/Shell/Stubs/MakeEnvForLatestMsvc.bat")),
                        _ => build_bat.replace("$PRE_EXECUTION_COMMAND", ""),
                    };
                    finder::write_to_file_if_different(&build_file, true, &build_bat);

                    let mut clean_build_bat: String = finder::read_file("Programs/Shell/Stubs/CmakeCleanBuildForTarget.bat");
                    expand_cmake_vars(&mut clean_build_bat, solution, platform, config, target);
                    clean_build_bat = match platform.toolset.as_str()
                    {
                        "msc" => clean_build_bat.replace("$PRE_EXECUTION_COMMAND", &finder::read_file("Programs/Shell/Stubs/MakeEnvForLatestMsvc.bat")),
                        _ => clean_build_bat.replace("$PRE_EXECUTION_COMMAND", ""),
                    };
                    finder::write_to_file_if_different(&clean_build_file, true, &clean_build_bat);
                }

                // sh
                {
                    let target_path: String = format!("{}/{}-{}/{}-{}", solution.get_saved_rel_dir_cmake(), platform.name, platform.architecture, target.name, config.name);
                    let gen_file: String = format!("{}/generate.sh", target_path);
                    let build_file: String = format!("{}/build.sh", target_path);
                    let clean_build_file: String = format!("{}/clean_build.sh", target_path);

                    let mut gen_sh: String = finder::read_file(&format!("Programs/Shell/Stubs/CmakeGenerateForTarget{}.sh", &platform_suffix));
                    expand_cmake_vars(&mut gen_sh, solution, platform, config, target);
                    gen_sh = match platform.toolset.as_str()
                    {
                        "msc" => gen_sh.replace("$PRE_EXECUTION_COMMAND", &finder::read_file("Programs/Shell/Stubs/MakeEnvForLatestMsvc.sh")),
                        _ => gen_sh.replace("$PRE_EXECUTION_COMMAND", ""),
                    };
                    finder::write_to_file_if_different(&gen_file, true, &gen_sh);
                    #[cfg(unix)]
                    std::process::Command::new("chmod").arg("+x").arg(&gen_file).output().expect("Failed to chmod +x generate.sh.");
                    
                    let mut build_sh: String = finder::read_file("Programs/Shell/Stubs/CmakeBuildForTarget.sh");
                    expand_cmake_vars(&mut build_sh, solution, platform, config, target);
                    build_sh = match platform.toolset.as_str()
                    {
                        "msc" => build_sh.replace("$PRE_EXECUTION_COMMAND", &finder::read_file("Programs/Shell/Stubs/MakeEnvForLatestMsvc.sh")),
                        _ => build_sh.replace("$PRE_EXECUTION_COMMAND", ""),
                    };
                    finder::write_to_file_if_different(&build_file, true, &build_sh);
                    #[cfg(unix)]
                    std::process::Command::new("chmod").arg("+x").arg(&build_file).output().expect("Failed to chmod +x generate.sh.");
                    
                    let mut clean_build_sh: String = finder::read_file("Programs/Shell/Stubs/CmakeCleanBuildForTarget.sh");
                    expand_cmake_vars(&mut clean_build_sh, solution, platform, config, target);
                    clean_build_sh = match platform.toolset.as_str()
                    {
                        "msc" => clean_build_sh.replace("$PRE_EXECUTION_COMMAND", &finder::read_file("Programs/Shell/Stubs/MakeEnvForLatestMsvc.sh")),
                        _ => clean_build_sh.replace("$PRE_EXECUTION_COMMAND", ""),
                    };
                    finder::write_to_file_if_different(&clean_build_file, true, &clean_build_sh);
                    #[cfg(unix)]
                    std::process::Command::new("chmod").arg("+x").arg(&clean_build_file).output().expect("Failed to chmod +x generate.sh.");
                }

                continue
            }
            continue
        }
        continue
    }

    // Presets
    {
        #[derive(Serialize, Deserialize)]
        #[serde(rename_all = "camelCase")]
        struct Out
        {
            pub version: i32,
            pub configure_presets: Vec<ConfigurePreset>,
        }

        #[derive(Serialize, Deserialize)]
        #[serde(rename_all = "camelCase")]
        struct ConfigurePreset
        {
            pub name: String,
            pub display_name: String,
            pub generator: String,
            pub binary_dir: String,
            pub cache_variables: HashMap<String, String>,
        }

        let mut out: Out = Out
        {
            version: 3,
            configure_presets: Vec::new(),
        };

        for platform in solution.platforms.iter()
        {
            for config in platform.configs.iter()
            {
                for target in config.targets.iter()
                {
                    let mut map: HashMap<String, String> = HashMap::new();
                    map.insert("JAFG_TARGET_PLATFORM".to_string(), platform.name.clone());
                    map.insert("JAFG_TARGET_ARCHITECTURE".to_string(), platform.architecture.clone());
                    map.insert("JAFG_TARGET_CONFIGURATION".to_string(), target.make_build_target_name(config));
                    map.insert("CMAKE_C_COMPILER".to_string(), expand_toolset_to_compiler_c(&platform.toolset).to_string());
                    map.insert("CMAKE_CXX_COMPILER".to_string(), expand_toolset_to_compiler_cxx(&platform.toolset).to_string());

                    out.configure_presets.push(ConfigurePreset
                    {
                        name: format!("{}_{}", platform.name, target.make_build_target_name(config)),
                        display_name: format!("{} | {}", platform.name, target.make_build_target_name(config)),
                        generator: "Ninja".to_string(),
                        binary_dir: format!("{}-{}/{}-{}", platform.name, platform.architecture, target.name, config.name),
                        cache_variables: map,
                    });
                }
            }
        }

        let mut builder: String = String::new();
        let b: &mut String = &mut builder;
        b.push_str(&serde_json::to_string(&out).unwrap());
        finder::write_to_file_if_different(&format!("{}/CMakePresets.json", solution.get_saved_rel_dir_cmake()), true, &builder);
    }


    return;
}

// Wwi stands for Write With Indentation (abbreviated because used literally in every line here - sorry).
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

// Wni stands for Write No Indentation.
fn wni<T: Display>(builder: &mut String, content: T)
{
    builder.push_str(content.to_string().as_str());
    builder.push_str("\n");

    return;
}

fn platform_to_cmake_var(platform: &Platform) -> String
{
    return format!("JAFG_PLATFORM_{}", platform.name.to_uppercase());
}

fn entry_to_lnk_flag(entry: &str) -> String
{
    if entry == "WIN_MAIN"
    {
        return "/SUBSYSTEM:WINDOWS".to_string();
    }
    else if entry == "MAIN"
    {
        return "".to_string();
    }
    else
    {
        panic!("Unrecognized entry point [{}].", entry);
    }
}

pub(crate) fn make_script(solution: &Solution)
{
    let cmake_root_dir: String = solution.get_saved_rel_dir_cmake();
    println!("Making cmake script at [{}].", cmake_root_dir);
    let cmake_file: String = format!("{}/CMakeLists.txt", cmake_root_dir);
    finder::ensure_file(&cmake_file);

    let mut content: String = String::new();
    let b: &mut String = &mut content; // String Builder

    wni(b, "# Copyright mzoesch. All rights reserved.");
    wni(b, "# This file is automatically generated by Jafg and can be deleted with no fear.\n");

    wni(b, "cmake_minimum_required(VERSION 3.21...3.30)");
    wni(b, "find_package(Python REQUIRED COMPONENTS Interpreter)");
    wni(b, "message(STATUS \"Python_EXECUTABLE: ${Python_EXECUTABLE}\")");
    wni(b, "if(CMAKE_CONFIGURATION_TYPES)");
    wwi(b, 1, "message(FATAL_ERROR \"Multi-configuration generators are not allowed. Please use a single-configuration generator with -G <generator>.\")");
    wni(b, "endif()");

    let mut allowed_platforms_var: Vec<String> = Vec::new();
    for platform in solution.platforms.iter()
    {
        wni(b, format!("set({} \"{}\")", platform_to_cmake_var(platform), &platform.name));
        allowed_platforms_var.push(format!("${{{}}}", platform_to_cmake_var(platform)));
    }
    wni(b, format!("set(JAFG_ALLOWED_PLATFORMS {})", allowed_platforms_var.join(" ")).as_str());
    wni(b, "if(NOT DEFINED JAFG_TARGET_PLATFORM)");
    wwi(b, 1, "message(FATAL_ERROR \"JAFG_TARGET_PLATFORM is not defined. Please define it with -DJAFG_TARGET_PLATFORM=<platform>. Allowed platforms are: ${JAFG_ALLOWED_PLATFORMS}.\")");
    wni(b, "endif()");
    wni(b, "list(FIND JAFG_ALLOWED_PLATFORMS ${JAFG_TARGET_PLATFORM} JAFG_TARGET_PLATFORM_INDEX)");
    wni(b, "if(JAFG_TARGET_PLATFORM_INDEX EQUAL -1)");
    wwi(b, 1, "message(FATAL_ERROR \"JAFG_TARGET_PLATFORM [${JAFG_TARGET_PLATFORM}] is not an allowed platform. Please define it with -DJAFG_TARGET_PLATFORM=<platform>. Allowed platforms are: ${JAFG_ALLOWED_PLATFORMS}.\")");
    wni(b, "endif()");
    wni(b, "message(STATUS \"JAFG_TARGET_PLATFORM: ${JAFG_TARGET_PLATFORM}\")");
    wni(b, "set_property(CACHE JAFG_TARGET_PLATFORM PROPERTY STRINGS ${JAFG_ALLOWED_PLATFORMS})");
    wni(b, "if(NOT DEFINED JAFG_TARGET_CONFIGURATION)");
    wwi(b, 1, "message(FATAL_ERROR \"JAFG_TARGET_CONFIGURATION is not defined. Please define it with -DJAFG_TARGET_CONFIGURATION=<configuration>.\")");
    wni(b, "endif()");
    wni(b, "if(NOT DEFINED JAFG_TARGET_ARCHITECTURE)");
    wwi(b, 1, "set(_ARCH ${CMAKE_SYSTEM_PROCESSOR})");
    wwi(b, 1, "if(_ARCH STREQUAL \"AMD64\")");
    wwi(b, 2, "set(JAFG_TARGET_ARCHITECTURE \"x86_64\")");
    wwi(b, 2, "message(WARNING \"JAFG_TARGET_ARCHITECTURE is not defined. Defaulting to x86_64.\")");
    wwi(b, 1, "elseif(_ARCH STREQUAL \"x86_64\")");
    wwi(b, 2, "set(JAFG_TARGET_ARCHITECTURE \"x86_64\")");
    wwi(b, 2, "message(WARNING \"JAFG_TARGET_ARCHITECTURE is not defined. Defaulting to x86_64.\")");
    wwi(b, 1, "elseif(_ARCH STREQUAL \"x86\")");
    wwi(b, 2, "set(JAFG_TARGET_ARCHITECTURE \"x86\")");
    wwi(b, 2, "message(WARNING \"JAFG_TARGET_ARCHITECTURE is not defined. Defaulting to x86.\")");
    wwi(b, 1, "else()");
    wwi(b, 2, "message(FATAL_ERROR \"JAFG_TARGET_ARCHITECTURE is not defined. Please define it with -DJAFG_TARGET_ARCHITECTURE=<architecture>.\")");
    wwi(b, 1, "endif()");
    wni(b, "endif()");
    wni(b, "message(STATUS \"JAFG_TARGET_ARCHITECTURE: ${JAFG_TARGET_ARCHITECTURE}\")");

    wni(b, "project(");
    wwi(b, 1, "Jafg");
    wwi(b, 1, "LANGUAGES C CXX");
    wwi(b, 1, ")");
    wni(b, "set(CMAKE_CXX_STANDARD 20)");
    wni(b, "set(CMAKE_CXX_STANDARD_REQUIRED ON)");
    wni(b, "set(CMAKE_CXX_EXTENSIONS ON)");
    wni(b, "set(CMAKE_POSITION_INDEPENDENT_CODE ON)");
    wni(b, format!("set(REL_ENGINE_ROOT_DIR \"${{CMAKE_SOURCE_DIR}}/{}\")", solution.get_ch_dir_up_rel_to_build_file()));
    wni(b, "get_filename_component(REAL_ENGINE_ROOT_DIR \"${REL_ENGINE_ROOT_DIR}\" REALPATH)");
    wni(b, "if(NOT REAL_ENGINE_ROOT_DIR)");
    wwi(b, 1, "message(FATAL_ERROR \"REAL_ENGINE_ROOT_DIR is not defined. Something critical went wrong. Rel path: [${REL_ENGINE_ROOT_DIR}].\")");
    wni(b, "endif()");
    wni(b, "message(STATUS \"REAL_ENGINE_ROOT_DIR: ${REAL_ENGINE_ROOT_DIR}\")");

    wni(b, "message(STATUS \"Detected C compiler: ${CMAKE_C_COMPILER}\")");
    wni(b, "message(STATUS \"Detected CXX compiler: ${CMAKE_CXX_COMPILER}\")");
    wni(b, "message(STATUS \"Hosting on: ${CMAKE_HOST_SYSTEM_NAME}\")");
    wni(b, "message(STATUS \"With CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}\")");
    wni(b, "message(STATUS \"With CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}\")");
    wni(b, "message(STATUS \"With CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}\")");

    for platform in solution.platforms.iter()
    {
        wni(b, &format!("if(JAFG_TARGET_PLATFORM STREQUAL {})", platform_to_cmake_var(platform)));

        if platform.toolset == "msc"
        {
            wwi(b, 1, "if(NOT CMAKE_CXX_COMPILER_ID STREQUAL \"MSVC\")");
            wwi(b, 2, "set(CMAKE_C_COMPILER \"cl.exe\")");
            wwi(b, 2, "set(CMAKE_CXX_COMPILER \"cl.exe\")");
            // wwi(b, 2, "message(FATAL_ERROR \"CXX compiler [MSVC] is required for this platform [${JAFG_TARGET_PLATFORM}].\")");
            wwi(b, 1, "endif()");
            wwi(b, 1, "set(CMAKE_GENERATOR_PLATFORM ${JAFG_TARGET_ARCHITECTURE})");
        }
        else if platform.toolset == "em"
        {
            wwi(b, 1, "string(REGEX MATCH \".*em\\\\+\\\\+.*\" REGREX_MATCHED ${CMAKE_CXX_COMPILER})");
            wwi(b, 1, "if(NOT REGREX_MATCHED)");
            wwi(b, 2, "message(FATAL_ERROR \"CXX compiler [EM] is required for this platform [${JAFG_TARGET_PLATFORM}].\")");
            wwi(b, 1, "endif()");
            // Do not set CMAKE_GENERATOR_PLATFORM for emscripten they will do that for us.
        }
        else if platform.toolset == "clang"
        {
            wwi(b, 1, "string(REGEX MATCH \".*clang\\\\+\\\\+.*\" REGREX_MATCHED ${CMAKE_CXX_COMPILER})");
            wwi(b, 1, "if(NOT REGREX_MATCHED)");
            wwi(b, 2, "set(CMAKE_C_COMPILER \"clang\")");
            wwi(b, 2, "set(CMAKE_CXX_COMPILER \"clang++\")");
            // wwi(b, 2, "message(FATAL_ERROR \"CXX compiler [Clang] is required for this platform [${JAFG_TARGET_PLATFORM}].\")");
            wwi(b, 1, "endif()");
            wwi(b, 1, "set(CMAKE_GENERATOR_PLATFORM ${JAFG_TARGET_ARCHITECTURE})");
        }
        else
        {
            panic!("Unrecognized toolset [{}].", platform.toolset);
        }

        wwi(b, 1, format!("set(CMAKE_CXX_FLAGS \"${{CMAKE_CXX_FLAGS}} {}\")", platform.build_flags.join(" ")));
        wwi(b, 1, format!("set(CMAKE_EXE_LINKER_FLAGS \"${{CMAKE_EXE_LINKER_FLAGS}} {}\")", platform.lnk_flags.join(" ")));
        wwi(b, 1, format!("set(CMAKE_SHARED_LINKER_FLAGS \"${{CMAKE_SHARED_LINKER_FLAGS}} {}\")", platform.lnk_flags.join(" ")));
        wwi(b, 1, format!("set(CMAKE_MODULE_LINKER_FLAGS \"${{CMAKE_MODULE_LINKER_FLAGS}} {}\")", platform.lnk_flags.join(" ")));

        wni(b, "endif()");
    }

    wni(b, "set(_FOUND_BUILD_TARGET FALSE)");
    for platform in solution.platforms.iter()
    {
        wni(b, &format!("if(JAFG_TARGET_PLATFORM STREQUAL {})", platform_to_cmake_var(platform)));
        wwi(b, 1, format!("add_compile_definitions({})", platform.defines.join(" ")));
        for config in platform.configs.iter()
        {
            wwi(b, 1, format!("string(REGEX MATCH \"-{}\" REGEX_MATCHED ${{JAFG_TARGET_CONFIGURATION}})", config.name));
            wwi(b, 1, "if(REGEX_MATCHED)");
            wwi(b, 2, format!("add_compile_definitions({})", config.defines.join(" ")));
            for target in config.targets.iter()
            {
                wwi(b, 2, format!("string(REGEX MATCH \"{}-\" REGEX_MATCHED ${{JAFG_TARGET_CONFIGURATION}})", target.name));
                wwi(b, 2, "if(REGEX_MATCHED)");
                wwi(b, 3, "if(_FOUND_BUILD_TARGET)");
                wwi(b, 4, "message(FATAL_ERROR \"Multiple targets found for the platform [${JAFG_TARGET_PLATFORM}] and configuration [${JAFG_TARGET_CONFIGURATION}]. Please check your -DJAFG_TARGET_PLATFORM and -DJAFG_TARGET_CONFIGURATION.\")");
                wwi(b, 3, "endif()");
                wwi(b, 3, "set(_FOUND_BUILD_TARGET TRUE)");
                wwi(b, 3, format!("add_compile_definitions({})", target.defines.join(" ")));
                wwi(b, 2, "endif()");
                continue
            }

            if config.symbols
            {
                if platform.toolset == "msc"
                {
                    wwi(b, 2, "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} /Zi\")");
                    wwi(b, 2, "set(CMAKE_EXE_LINKER_FLAGS \"${CMAKE_EXE_LINKER_FLAGS} /DEBUG\")");
                    wwi(b, 2, "set(CMAKE_SHARED_LINKER_FLAGS \"${CMAKE_SHARED_LINKER_FLAGS} /DEBUG\")");
                    wwi(b, 2, "set(CMAKE_MODULE_LINKER_FLAGS \"${CMAKE_MODULE_LINKER_FLAGS} /DEBUG\")");
                }
                else if platform.toolset == "em"
                {
                }
                else if platform.toolset == "clang"
                {
                    wwi(b, 2, "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -g\")");
                }
            }
            else
            {
                if platform.toolset == "msc"
                {
                    wwi(b, 2, "set(CMAKE_EXE_LINKER_FLAGS \"${CMAKE_EXE_LINKER_FLAGS} /DEBUG:NONE\")");
                    wwi(b, 2, "set(CMAKE_SHARED_LINKER_FLAGS \"${CMAKE_SHARED_LINKER_FLAGS} /DEBUG:NONE\")");
                    wwi(b, 2, "set(CMAKE_MODULE_LINKER_FLAGS \"${CMAKE_MODULE_LINKER_FLAGS} /DEBUG:NONE\")");
                }
                else if platform.toolset == "em"
                {
                }
                else if platform.toolset == "clang"
                {
                    wwi(b, 2, "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -g0\")");
                }
            }

            if config.optimize == false
            {
                if platform.toolset == "msc"
                {
                    wwi(b, 2, "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} /Od\")");
                }
                else if platform.toolset == "em"
                {
                }
                else if platform.toolset == "clang"
                {
                    wwi(b, 2, "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -O0\")");
                }
            }
            else if config.optimize && config.symbols
            {
                if platform.toolset == "msc"
                {
                    wwi(b, 2, "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} /O2\")");
                }
                else if platform.toolset == "em"
                {
                }
                else if platform.toolset == "clang"
                {
                    wwi(b, 2, "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -O2\")");
                }
            }
            else if config.optimize && config.symbols == false
            {
                if platform.toolset == "msc"
                {
                    // /fp:fast?
                    wwi(b, 2, "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} /Ox /GL\")"); // !!DANGEROUS!! /GL may not what we want to use. We shall see.
                }
                else if platform.toolset == "em"
                {
                }
                else if platform.toolset == "clang"
                {
                    wwi(b, 2, "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -O3\")");
                }
            }

            wwi(b, 1, "endif()");
            continue
        }
        wni(b, "endif()");
        continue
    }
    wni(b, "if(NOT _FOUND_BUILD_TARGET)");
    wwi(b, 1, "message(FATAL_ERROR \"No such target found for the platform [${JAFG_TARGET_PLATFORM}] and configuration [${JAFG_TARGET_CONFIGURATION}]. Please check your -DJAFG_TARGET_PLATFORM and -DJAFG_TARGET_CONFIGURATION.\")");
    wni(b, "endif()");

    for platform in solution.platforms.iter()
    {
        wni(b, &format!("if(JAFG_TARGET_PLATFORM STREQUAL {})", platform_to_cmake_var(platform)));
        for config in platform.configs.iter()
        {
            for target in config.targets.iter()
            {
                let native_name: String = target.make_build_target_name(config);
                wwi(b, 1, format!("if(JAFG_TARGET_CONFIGURATION STREQUAL \"{}\")", native_name));

                let mut unity_runtimes: Vec<String> = Vec::new();
                let mut native_dependencies: Vec<String> = Vec::new();
                let mut pre_build_commands: Vec<String> = Vec::new();
                let mut post_build_commands: Vec<String> = Vec::new();
                if platform.unity
                {
                    wwi(b, 2, format!("set({}_GLOBAL_UNITY_FILES \"\")", platform.name.to_uppercase()));
                    wwi(b, 2, format!("set({}_GLOBAL_UNITY_INCLUDES \"\")", platform.name.to_uppercase()));
                }

                for module in target.modules.iter()
                {
                    let transitive_dependencies: Vec<String> = module.get_dependencies_transitive(target);

                    wwi(b, 2, format!("message(STATUS \"Adding module [{}] ...\")", module.name).as_str());

                    wwi(b, 2, format!("file(GLOB_RECURSE {}_SRC_FILES", module.name));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/*.md\"", module.get_functional_rel_dir()));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/*.py\"", module.get_functional_rel_dir()));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/*.h\"", module.get_functional_rel_source_dir()));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/*.hpp\"", module.get_functional_rel_source_dir()));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/*.c\"", module.get_functional_rel_source_dir()));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/*.cpp\"", module.get_functional_rel_source_dir()));
                    wwi(b, 3, ")");
                    wwi(b, 2, format!("source_group(TREE ${{REAL_ENGINE_ROOT_DIR}} FILES ${{{}_SRC_FILES}})", module.name));
                    wwi(b, 2, format!("file(GLOB_RECURSE {}_C_SRC_FILES", module.name));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/*.c\"", module.get_functional_rel_source_dir()));
                    wwi(b, 3, ")");
                    wwi(b, 2, format!("source_group(TREE ${{REAL_ENGINE_ROOT_DIR}} FILES ${{{}_C_SRC_FILES}})", module.name));
                    wwi(b, 3, format!("file(GLOB_RECURSE {}_GEN_SRC_FILES", module.name));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/*.generated.h\"", solution.construct_relative_gh_path(platform, module)));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/*.generated.cpp\"", solution.construct_relative_gt_path(platform, module)));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/*.generated.h\"", solution.construct_relative_cgh_path(platform, config, target, module)));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/*.generated.cpp\"", solution.construct_relative_cgt_path(platform, config, target, module)));
                    wwi(b, 3, ")");
                    wwi(b, 2, format!("source_group(TREE \"${{REAL_ENGINE_ROOT_DIR}}/{}\" FILES ${{{}_GEN_SRC_FILES}})", solution.construct_relative_root_saved_dir(platform, module), module.name));

                    if platform.unity && module.preserve_unity == false && module.kind.is_launch()
                    {
                        panic!("Launch module [{}] is not allowed to be preserved in unity build.", module.name);
                    }
                    else if platform.unity && module.preserve_unity == false
                    {
                        if module.kind.is_static()
                        {
                            wwi(b, 2, format!("add_library({} STATIC ${{{}_SRC_FILES}} ${{{}_C_SRC_FILES}} ${{{}_GEN_SRC_FILES}})", module.name, module.name, module.name, module.name));
                        }
                        else if module.kind.is_shared()
                        {
                            wwi(b, 2, format!("add_library({} SHARED ${{{}_SRC_FILES}} ${{{}_C_SRC_FILES}} ${{{}_GEN_SRC_FILES}})", module.name, module.name, module.name, module.name));
                        }
                    }
                    else if platform.unity
                    {
                        if module.kind.is_launch()
                        {
                            unity_runtimes.push(module.name.to_string());
                        }
                        wwi(b, 2, format!("list(APPEND {}_GLOBAL_UNITY_FILES ${{{}_SRC_FILES}} ${{{}_C_SRC_FILES}} ${{{}_GEN_SRC_FILES}})", platform.name.to_uppercase(), module.name, module.name, module.name));
                    }
                    else
                    {
                        if module.kind.is_launch()
                        {
                            wwi(b, 2, format!("add_executable({}{} ${{{}_SRC_FILES}} ${{{}_C_SRC_FILES}} ${{{}_GEN_SRC_FILES}})",
                                module.name,
                                match platform.name.as_str()
                                {
                                    "Windows" => " WIN32",
                                    _ => "",
                                },
                                module.name, module.name, module.name
                            ));
                        }
                        else if module.kind.is_static()
                        {
                            wwi(b, 2, format!("add_library({} STATIC ${{{}_SRC_FILES}} ${{{}_C_SRC_FILES}} ${{{}_GEN_SRC_FILES}})", module.name, module.name, module.name, module.name));
                        }
                        else if module.kind.is_shared()
                        {
                            wwi(b, 2, format!("add_library({} SHARED ${{{}_SRC_FILES}} ${{{}_C_SRC_FILES}} ${{{}_GEN_SRC_FILES}})", module.name, module.name, module.name, module.name));
                        }
                    }

                    if platform.unity == false || module.preserve_unity == false
                    {
                        wwi(b, 2, format!("foreach(src_file ${{{}_C_SRC_FILES}})", module.name));
                        wwi(b, 3, "set_source_files_properties(${src_file} PROPERTIES LANGUAGE C)");
                        wwi(b, 3, "set_source_files_properties(${src_file} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)");
                        wwi(b, 2, "endforeach()");
                    }

                    if platform.unity == false || module.preserve_unity == false
                    {
                        wwi(b, 2, format!("set_target_properties({} PROPERTIES", module.name));
                        wwi(b, 3, format!("ARCHIVE_OUTPUT_DIRECTORY \"${{REAL_ENGINE_ROOT_DIR}}/Binaries/${{JAFG_TARGET_PLATFORM}}-${{JAFG_TARGET_ARCHITECTURE}}/${{JAFG_TARGET_CONFIGURATION}}/{}\"", module.get_functional_rel_dir()));
                        wwi(b, 3, format!("LIBRARY_OUTPUT_DIRECTORY \"${{REAL_ENGINE_ROOT_DIR}}/Binaries/${{JAFG_TARGET_PLATFORM}}-${{JAFG_TARGET_ARCHITECTURE}}/${{JAFG_TARGET_CONFIGURATION}}/{}\"", module.get_functional_rel_dir()));
                        wwi(b, 3, format!("RUNTIME_OUTPUT_DIRECTORY \"${{REAL_ENGINE_ROOT_DIR}}/Binaries/${{JAFG_TARGET_PLATFORM}}-${{JAFG_TARGET_ARCHITECTURE}}/${{JAFG_TARGET_CONFIGURATION}}/{}\"", module.get_functional_rel_dir()));
                        if platform.target_props_lnk_flags.len() > 0
                        {
                            wwi(b, 3, format!("LINK_FLAGS \"{}\"", platform.target_props_lnk_flags.join(" ")));
                        }
                        if platform.target_props_build_flags.len() > 0
                        {
                            wwi(b, 3, format!("COMPILE_FLAGS \"{}\"", platform.target_props_build_flags.join(" ")));
                        }
                        if platform.target_props_sfx.len() > 0
                        {
                            wwi(b, 3, format!("SUFFIX \"{}\"", &platform.target_props_sfx));
                        }
                        wwi(b, 3, ")");
                    }

                    if platform.unity && module.preserve_unity
                    {
                        wwi(b, 2, format!("list(APPEND {}_GLOBAL_UNITY_INCLUDES", platform.name.to_uppercase()));
                    }
                    else
                    {
                        wwi(b, 2, format!("target_include_directories({} PRIVATE", module.name));
                        wwi(b, 3, "\"${REAL_ENGINE_ROOT_DIR}\"");
                    }
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/Internal\"", module.get_functional_rel_source_dir()));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/Public\"", module.get_functional_rel_source_dir()));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}\"", solution.construct_relative_gh_path(platform, module)));
                    wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}\"", solution.construct_relative_cgh_path(platform, config, target, module)));
                    for dependency in transitive_dependencies.iter()
                    {
                        wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}/Public\"", target.find_module_by_name_checked(dependency).get_functional_rel_source_dir()));
                        wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}\"", solution.construct_relative_gh_path(platform, target.find_module_by_name_checked(dependency))));
                    }
                    for dependency in module.native_includes.iter()
                    {
                        wwi(b, 3, format!("\"${{REAL_ENGINE_ROOT_DIR}}/{}\"", dependency));
                    }
                    wwi(b, 3, ")");

                    if platform.unity == false || module.preserve_unity == false
                    {
                        for dependency in transitive_dependencies.iter()
                        {
                            let d: &Module = target.find_module_by_name_checked(dependency);
                            if d.kind.is_launch()
                            {
                                panic!("Dependency [{}] is not allowed for launch module [{}].", d.name, d.relative_dir);
                            }
                            wwi(b, 2, format!("add_dependencies({} {})", module.name, d.name));
                            wwi(b, 2, format!("target_link_libraries({} PRIVATE \"${{REAL_ENGINE_ROOT_DIR}}/Binaries/${{JAFG_TARGET_PLATFORM}}-${{JAFG_TARGET_ARCHITECTURE}}/${{JAFG_TARGET_CONFIGURATION}}/{}/{}{}{}\")",
                                module.name,
                                d.get_functional_rel_dir(),
                                match d.kind
                                {
                                    ModuleKind::Shared => platform.get_shared_counterpart_prefix(),
                                    ModuleKind::Static => platform.get_static_bin_prefix(),
                                    _ => panic!("Module kind not allowed for linking."),
                                },
                                d.name,
                                match d.kind
                                {
                                    ModuleKind::Shared => platform.get_shared_counterpart_suffix(),
                                    ModuleKind::Static => platform.get_static_bin_suffix(),
                                    _ => panic!("Module kind not allowed for linking."),
                                }
                            ));
                        }
                    }

                    for dependency in module.native_dependencies.iter()
                    {
                        let d: String = format!("${{REAL_ENGINE_ROOT_DIR}}/{}", dependency);
                        if platform.unity && module.preserve_unity
                        {
                            if native_dependencies.contains(&d) == false
                            {
                                native_dependencies.push(d);
                            }
                        }
                        else
                        {
                            wwi(b, 2, format!("target_link_libraries({} PRIVATE \"{}\")", module.name, d));
                        }
                    }
                    for dependency in module.native_runtime_dependencies.iter()
                    {
                        let d: String = format!("${{REAL_ENGINE_ROOT_DIR}}/{}", dependency);
                        if platform.unity && module.preserve_unity
                        {
                            if native_dependencies.contains(&d) == false
                            {
                                native_dependencies.push(d);
                            }
                        }
                        else
                        {
                            wwi(b, 2, format!("target_link_libraries({} PRIVATE \"{}\")", module.name, d));
                        }
                    }

                    if platform.unity == false || module.preserve_unity == false
                    {
                        if module.kind.is_shared()
                        {
                            wwi(b, 2, format!("target_compile_definitions({} PRIVATE {} {})",
                                module.name,
                                format!("{}_API=PLATFORM_CALLSPEC_OUT", module.name.to_uppercase()),
                                format!("{}_EXTERN=PLATFORM_EXTERNSPEC_OUT", module.name.to_uppercase())
                            ));
                        }
                        else
                        {
                            wwi(b, 2, format!("target_compile_definitions({} PRIVATE {} {})",
                                module.name,
                                format!("{}_API=", module.name.to_uppercase()),
                                format!("{}_EXTERN=", module.name.to_uppercase())
                            ));
                        }
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
                                        wwi(b, 2, format!("target_compile_definitions({} PRIVATE {} {})",
                                            module.name,
                                            format!("{}_API=PLATFORM_CALLSPEC_IN", target_module.name.to_uppercase()),
                                            format!("{}_EXTERN=PLATFORM_EXTERNSPEC_IN", target_module.name.to_uppercase())
                                        ));
                                    }
                                    else
                                    {
                                        wwi(b, 2, format!("target_compile_definitions({} PRIVATE {} {})",
                                            module.name,
                                            format!("{}_API=", target_module.name.to_uppercase()),
                                            format!("{}_EXTERN=", target_module.name.to_uppercase())
                                        ));
                                    }
                                }
                            }
                        }
                    }

                    if (platform.unity == false || module.preserve_unity == false) && module.pch
                    {
                        wwi(b, 2, format!("target_precompile_headers({} PRIVATE \"${{REAL_ENGINE_ROOT_DIR}}/{}/{}\")",
                            module.name,
                            solution.construct_relative_cgh_path(platform, config, target, module),
                            paths::FILE_PCH_H
                        ));
                    }

                    if (platform.unity == false || module.preserve_unity == false) && module.kind.is_launch() && entry_to_lnk_flag(&module.entry) != ""
                    {
                        wwi(b, 2, format!("set_target_properties({} PROPERTIES LINK_FLAGS \"{}\")", module.name, entry_to_lnk_flag(&module.entry)));
                    }

                    if platform.unity == false || module.preserve_unity == false
                    {
                        wwi(b, 2, format!("get_target_property(CUR_TARGET_KIND {} TYPE)", module.name));
                        wwi(b, 2, format!("add_custom_target({}_PRE_BUILD_COMMAND", module.name));
                    }
                    for _step in module.pre_builds.iter()
                    {
                        let steps: Vec<String> = crate::solution_generator::launch::expand_variables_for_step(&_step, ScriptType::Cmake);
                        let compound: String = steps.join("");
                        if platform.unity && module.preserve_unity
                        {
                            pre_build_commands.push(compound);
                        }
                        else
                        {
                            wwi(b, 3, format!("COMMAND {}", compound));
                        }
                        continue
                    }
                    if platform.unity == false || module.preserve_unity == false
                    {
                        wwi(b, 3, ")");
                        wwi(b, 2, format!("add_dependencies({} {}_PRE_BUILD_COMMAND)", module.name, module.name));
                    }

                    if platform.unity == false || module.preserve_unity == false
                    {
                        wwi(b, 2, format!("add_custom_command(TARGET {} POST_BUILD", module.name));
                    }
                    for _step in module.post_builds.iter()
                    {
                        let steps: Vec<String> = crate::solution_generator::launch::expand_variables_for_step(&_step, ScriptType::Cmake);
                        let compound: String = steps.join("");
                        if platform.unity && module.preserve_unity
                        {
                            post_build_commands.push(compound);
                        }
                        else
                        {
                            wwi(b, 3, format!("COMMAND {}", compound));
                        }
                        continue
                    }
                    if platform.unity == false || module.preserve_unity == false
                    {
                        wwi(b, 3, ")");
                    }

                    continue;
                }

                if platform.unity
                {
                    for runtime in unity_runtimes.iter()
                    {
                        wwi(b, 2, format!("add_executable({} ${{{}_GLOBAL_UNITY_FILES}})", runtime, platform.name.to_uppercase()));
                        wwi(b, 2, format!("target_include_directories({} PRIVATE \"${{REAL_ENGINE_ROOT_DIR}}\" ${{{}_GLOBAL_UNITY_INCLUDES}})", runtime, platform.name.to_uppercase()));
                        // wwi(b, 2, format!("target_compile_options({} PRIVATE -include CoreAfx.h)", runtime));
                        wwi(b, 2, format!("set_target_properties({} PROPERTIES", runtime));
                        wwi(b, 3, format!("ARCHIVE_OUTPUT_DIRECTORY \"${{REAL_ENGINE_ROOT_DIR}}/Binaries/${{JAFG_TARGET_PLATFORM}}-${{JAFG_TARGET_ARCHITECTURE}}/${{JAFG_TARGET_CONFIGURATION}}/{}\"", target.find_module_by_name_checked(runtime).get_functional_rel_dir()));
                        wwi(b, 3, format!("LIBRARY_OUTPUT_DIRECTORY \"${{REAL_ENGINE_ROOT_DIR}}/Binaries/${{JAFG_TARGET_PLATFORM}}-${{JAFG_TARGET_ARCHITECTURE}}/${{JAFG_TARGET_CONFIGURATION}}/{}\"", target.find_module_by_name_checked(runtime).get_functional_rel_dir()));
                        wwi(b, 3, format!("RUNTIME_OUTPUT_DIRECTORY \"${{REAL_ENGINE_ROOT_DIR}}/Binaries/${{JAFG_TARGET_PLATFORM}}-${{JAFG_TARGET_ARCHITECTURE}}/${{JAFG_TARGET_CONFIGURATION}}/{}\"", target.find_module_by_name_checked(runtime).get_functional_rel_dir()));
                        if platform.target_props_lnk_flags.len() > 0
                        {
                            wwi(b, 3, format!("LINK_FLAGS \"{}\"", platform.target_props_lnk_flags.join(" ")));
                        }
                        if platform.target_props_build_flags.len() > 0
                        {
                            wwi(b, 3, format!("COMPILE_FLAGS \"{}\"", platform.target_props_build_flags.join(" ")));
                        }
                        if platform.target_props_sfx.len() > 0
                        {
                            wwi(b, 3, format!("SUFFIX \"{}\"", &platform.target_props_sfx));
                        }
                        wwi(b, 3, ")");
                        for module in target.modules.iter()
                        {
                            if module.preserve_unity == false
                            {
                                if module.kind.is_launch()
                                {
                                    panic!("Launch module [{}] is not allowed to be non preserved in unity build.", module.name);
                                }
                                else if module.kind.is_shared()
                                {
                                    wwi(b, 2, format!("target_compile_definitions({} PRIVATE {} {})",
                                        runtime,
                                        format!("{}_API=PLATFORM_CALLSPEC_IN", module.name.to_uppercase()),
                                        format!("{}_EXTERN=PLATFORM_EXTERNSPEC_IN", module.name.to_uppercase())
                                    ));
                                }
                                else
                                {
                                    wwi(b, 2, format!("target_compile_definitions({} PRIVATE {} {})",
                                        runtime,
                                        format!("{}_API=", module.name.to_uppercase()),
                                        format!("{}_EXTERN=", module.name.to_uppercase())
                                    ));
                                }
                                wwi(b, 2, format!("add_dependencies({} {})", runtime, module.name));
                                wwi(b, 2, format!("target_link_libraries({} PRIVATE \"${{REAL_ENGINE_ROOT_DIR}}/Binaries/${{JAFG_TARGET_PLATFORM}}-${{JAFG_TARGET_ARCHITECTURE}}/${{JAFG_TARGET_CONFIGURATION}}/{}/{}{}{}\")",
                                    runtime,
                                    module.get_functional_rel_dir(),
                                    match module.kind
                                    {
                                        ModuleKind::Shared => platform.get_shared_counterpart_prefix(),
                                        ModuleKind::Static => platform.get_static_bin_prefix(),
                                        _ => panic!("Module kind not allowed for linking."),
                                    },
                                    module.name,
                                    match module.kind
                                    {
                                        ModuleKind::Shared => platform.get_shared_counterpart_suffix(),
                                        ModuleKind::Static => platform.get_static_bin_suffix(),
                                        _ => panic!("Module kind not allowed for linking."),
                                    }
                                ));
                            }
                            else
                            {
                                wwi(b, 2,
                                    format!("target_compile_definitions({} PRIVATE {}_API= {}_EXTERN=)",
                                    runtime, module.name.to_uppercase(), module.name.to_uppercase()
                                ));
                            }
                        }
                        for d in native_dependencies.iter()
                        {
                            wwi(b, 1, format!("target_link_libraries({} PRIVATE \"{}\")", runtime, d));
                        }

                        wwi(b, 2, format!("get_target_property(CUR_TARGET_KIND {} TYPE)", runtime));
                        wwi(b, 2, format!("add_custom_target({}_PRE_BUILD_COMMAND", runtime));
                        for command in pre_build_commands.iter()
                        {
                            wwi(b, 3, format!("COMMAND {}", command));
                        }
                        wwi(b, 3, ")");
                        wwi(b, 2, format!("add_dependencies({} {}_PRE_BUILD_COMMAND)", runtime, runtime));

                        wwi(b, 2, format!("add_custom_command(TARGET {} POST_BUILD", runtime));
                        for command in post_build_commands.iter()
                        {
                            wwi(b, 3, format!("COMMAND {}", command));
                        }
                        wwi(b, 3, ")");
                    }
                }

                wwi(b, 1, "endif()");
                continue;
            }
            continue;
        }
        wni(b, "endif()");
        continue;
    }

    finder::write_to_file_if_different(&cmake_file, true, &content);

    return;
}
