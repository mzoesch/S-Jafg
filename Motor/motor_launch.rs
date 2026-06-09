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

fn generate_map_macros(n_max: usize) -> String
{
    let api_prefix = "JAFG";
    let detail_api_prefix = "DETAIL_JAFG";

    let mut out = String::new();
    out.push_str("// Copyright mzoesch. All rights reserved.\n\n");

    for n in 1..=n_max
    {
        let mut args = String::new();
        args.push_str("Transform");

        for i in 1..=n
        {
            if i % 10 == 1
            {
                args.push_str(", \\\n    ");
            }
            else
            {
                args.push_str(", ");
            }
            args.push_str(&format!("What{}", i));
        }

        let mut body = String::new();

        for i in 1..=n
        {
            if i % 10 == 1
            {
                body.push_str("\\\n    ");
            }
            else
            {
                body.push(' ');
            }
            body.push_str(&format!("Transform(What{})", i));
        }

        out.push_str(&format!(
            "#define {prefix}_MAP_{n}({args}) {body}\n",
            prefix = detail_api_prefix
            ));
    }

    out.push('\n');
    out.push('\n');

    out.push_str(&format!(
        "#define {api_prefix}_GET_MAP( ",
        ));

    for n in 1..=n_max
    {
        if n % 10 == 1
        {
            out.push_str("\\\n    ");
        }
        out.push_str(&format!("_{}, ", n));
    }

    out.push_str("\\\n    NAME, ...) NAME\n\n\n");

    out.push_str(&format!(
        "#define {api_prefix}_MAP(Transform, ...) \\\n    {api_prefix}_GET_MAP(__VA_ARGS__, ",
        ));

    for n in (2..=n_max).rev()
    {
        if n % 10 == 0
        {
            out.push_str("\\\n    ");
        }
        out.push_str(&format!("{prefix}_MAP_{n}, ", prefix = detail_api_prefix));
    }

    out.push_str(&format!(
        "{prefix}_MAP_1) \\\n    (Transform, __VA_ARGS__)\n",
        prefix = detail_api_prefix
        ));

    out
}

fn main()
{
    let cwd: std::path::PathBuf = std::env::current_dir().unwrap();
    let new_cwd: String = paths::get_engine_root_dir();
    std::env::set_current_dir(&new_cwd).unwrap();

    let args: Cli = Cli::parse();

    if args.verbose
    {
        println!("Launching pre-build for [{}] ...", args.module);
    }

    let jafg_map = paths::construct_relative_gh_path(&args) + "/JafgMap.h";
    if args.module == "Engine" && !finder::exists_file(&jafg_map)
    {
        if args.verbose
        {
            println!("Generating JafgMap.h ...");
        }
        let map_content: String = generate_map_macros(360);
        finder::write_to_file_if_different(&jafg_map, args.verbose, &map_content);
    }

    /*
     * Construct paths so we do not have to deal with missing dirs when crating random files in
     * them - some platforms forbid to create files in dirs that don't exist.
     * Also better for the target IDE performance.
     */
    finder::ensure_path(&paths::construct_relative_gh_path(&args));
    finder::ensure_path(&paths::construct_relative_gt_path(&args));

    /*
     * Reflect the module by gathering all necessary packages and exporting their macros.
     */
    reflector::reflect_module(&args);

    /*
     * Search for a unit test file in the module's test source directory and supply the compiler
     * with their definitions.
     */
    if args.target == "TestUnit"
    {
        /*
         * The file to the header that marks the module as a module that uses the default
         * simple test registration procedure.
         * A module might still register its own test that are not inside the TestModule.h file,
         * but they will then have to handle the registration themselves - we simply do not care.
         */
        let test_module_f: String = format!("{}/TestModule.h", paths::get_relative_source_dir_test(&args));

        /*
         * The path to the translation file that contains the module files of unit tests
         * if the module uses the default simple test registration procedure.
         */
        let test_module_t: String = format!("{}/_TestModule.generated.cpp", paths::construct_relative_gt_path(&args));

        if finder::exists_file(test_module_f.as_str())
        {
            if args.verbose
            {
                println!("[{}]: Found module test file. Creating auto unit test registration files.", test_module_f);
            }

            let mut t_builder: String = String::new();
            t_builder.push_str(&format!(r##"
// Copyright mzoesch. All rights reserved.

/*-----------------------------------------------------------------------------
    This translation file was generated by the Jafg build tool.
    Do not modify it manually.
-----------------------------------------------------------------------------*/

#ifdef PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION
    #undef PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION
#endif /* PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION */
#define PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION               1

#include "{test_module_f}"

#ifdef PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION
    #undef PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION
#endif /* PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION */

"##,
            ));

            finder::write_to_file_if_different(&test_module_t, args.verbose, &t_builder);
        }
        else
        {
            if finder::exists_file(test_module_t.as_str())
            {
                if args.verbose
                {
                    println!("[{}]: Auto registration file exists but the module no longer uses the default automatic unit test registration procedure.", test_module_t);
                }

                finder::delete_file(test_module_t.as_str());
            }
        }
    }

    std::env::set_current_dir(cwd).unwrap();
    std::process::exit(0);
}
