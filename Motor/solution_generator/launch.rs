// Copyright mzoesch. All rights reserved.

use crate::Cli;
use crate::core::application::{Application, Solution};
use crate::solution_generator::make_cmake::make_cmake;
use crate::solution_generator::make_premake::make_premake;
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

    make_cmake(solution, emulate);
    make_premake(solution, emulate);

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
