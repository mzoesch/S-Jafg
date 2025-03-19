// Copyright mzoesch. All rights reserved.

use serde::{Serialize, Deserialize};
use crate::core::paths;

#[derive(Serialize, Deserialize)]
pub struct Application
{
    pub solutions: Vec<Solution>,
}

impl Default for Application
{
    fn default() -> Self
    {
        Self
        {
            solutions: Vec::new(),
        }
    }
}

impl Application
{
    pub fn find_solution_by_name(&self, name: &str) -> Option<&Solution>
    {
        for solution in self.solutions.iter()
        {
            if solution.name == name
            {
                return Some(solution);
            }
        }

        return None;
    }

    pub fn find_solution_by_name_checked(&self, name: &str) -> &Solution
    {
        match self.find_solution_by_name(name)
        {
            Some(solution) => solution,
            None => panic!("Could not find solution with name [{}].", name),
        }
    }
}

#[derive(Serialize, Deserialize)]
pub struct MaxModule
{
    pub name: String,
    pub relative_dir: String,
}

impl MaxModule
{
    pub fn is_equal_str(&self, name: &str) -> bool
    {
        return self.name == name || self.relative_dir == name;
    }

    pub fn get_functional_rel_dir(&self) -> String
    {
        return self.relative_dir[1..].to_string();
    }

    pub fn get_functional_parent_rel_dir(&self) -> String
    {
        let mut parts: Vec<&str> = self.relative_dir[1..].split('/').collect();
        parts.pop();
        return parts.join("/");
    }
}

#[derive(Serialize, Deserialize)]
pub struct Solution
{
    pub name: String,
    pub relative_dir: String,
    pub startup: String,
    pub max_modules: Vec<MaxModule>,
    pub platforms: Vec<Platform>,
}

impl Solution
{
    pub fn get_functional_rel_dir(&self) -> String
    {
        return self.relative_dir[1..].to_string();
    }

    pub fn get_saved_rel_dir_premake(&self) -> String
    {
        return format!("{}/{}/P_SLN_{}", paths::PATH_SAVED_DIR, self.get_functional_rel_dir(), self.name);
    }

    pub fn get_saved_rel_dir_cmake(&self) -> String
    {
        return format!("{}/{}/C_SLN_{}", paths::PATH_SAVED_DIR, self.get_functional_rel_dir(), self.name);
    }

    pub fn construct_relative_root_saved_dir(&self, p: &Platform, m: &Module) -> String
    {
        return format!("{}/{}/SLN_{}/{}/{}", paths::PATH_SAVED_DIR, self.get_functional_rel_dir(), self.name, m.get_functional_rel_dir(), p.name);
    }

    pub fn construct_relative_cgh_path(&self, p: &Platform, c: &BuildConfig, t: &Target, m: &Module) -> String
    {
        return format!("{}/{}/SLN_{}/{}/{}/cgh/{}", paths::PATH_SAVED_DIR, self.get_functional_rel_dir(), self.name, m.get_functional_rel_dir(), p.name, t.make_build_target_name(c));
    }

    pub fn construct_relative_cgt_path(&self, p: &Platform, c: &BuildConfig, t: &Target, m: &Module) -> String
    {
        return format!("{}/{}/SLN_{}/{}/{}/cgt/{}", paths::PATH_SAVED_DIR, self.get_functional_rel_dir(), self.name, m.get_functional_rel_dir(), p.name, t.make_build_target_name(c));
    }

    pub fn construct_relative_gh_path(&self, p: &Platform, m: &Module) -> String
    {
        return format!("{}/{}/SLN_{}/{}/{}/gh", paths::PATH_SAVED_DIR, self.get_functional_rel_dir(), self.name, m.get_functional_rel_dir(), p.name);
    }

    pub fn construct_relative_gt_path(&self, p: &Platform, m: &Module) -> String
    {
        return format!("{}/{}/SLN_{}/{}/{}/gt", paths::PATH_SAVED_DIR, self.get_functional_rel_dir(), self.name, m.get_functional_rel_dir(), p.name);
    }

    pub fn find_platform_by_name(&self, name: &str) -> Option<&Platform>
    {
        for platform in self.platforms.iter()
        {
            if platform.name == name
            {
                return Some(platform);
            }
        }

        return None;
    }

    pub fn find_platform_by_name_checked(&self, name: &str) -> &Platform
    {
        match self.find_platform_by_name(name)
        {
            Some(platform) => platform,
            None => panic!("Could not find platform with name [{}].", name),
        }
    }

    pub(crate) fn get_ch_dir_up_rel_to_build_file(&self) -> String
    {
        return format!("../../..{}", "/..".repeat(self.relative_dir.chars().filter(|c| *c == '/').count()));
    }
}

#[derive(Serialize, Deserialize)]
pub struct Platform
{
    pub name: String,
    pub version: String,
    pub architecture: String,
    pub toolset: String,
    pub lnk_flags: Vec<String>,
    pub build_flags: Vec<String>,
    pub defines: Vec<String>,
    pub configs: Vec<BuildConfig>,
}

impl Platform
{
    pub fn find_config_by_name(&self, name: &str) -> Option<&BuildConfig>
    {
        for config in self.configs.iter()
        {
            if config.name == name
            {
                return Some(config);
            }
        }

        return None;
    }

    pub fn find_config_by_name_checked(&self, name: &str) -> &BuildConfig
    {
        match self.find_config_by_name(name)
        {
            Some(config) => config,
            None => panic!("Could not find config with name [{}].", name),
        }
    }

    pub fn get_shared_counterpart(&self) -> String
    {
        return match self.name.as_str()
        {
            "Windows" => ".lib",
            _ => ".not_supported",
        }.to_string()
    }

    pub fn get_shared_bin_extension(&self) -> String
    {
        return match self.name.as_str()
        {
            "Windows" => ".dll",
            _ => ".not_supported",
        }.to_string()
    }

    pub fn get_static_bin_extension(&self) -> String
    {
        return match self.name.as_str()
        {
            "Windows" => ".lib",
            _ => ".not_supported",
        }.to_string()
    }

    pub fn get_symbols_bin_extension(&self) -> String
    {
        return match self.name.as_str()
        {
            "Windows" => ".pdb",
            _ => ".not_supported",
        }.to_string()
    }
}

#[derive(Serialize, Deserialize)]
pub struct BuildConfig
{
    pub name: String,
    pub runtime: String,
    pub symbols: bool,
    pub optimize: bool,
    pub defines: Vec<String>,
    pub targets: Vec<Target>,
}

impl BuildConfig
{
    pub fn find_target_by_name(&self, name: &str) -> Option<&Target>
    {
        for target in self.targets.iter()
        {
            if target.name == name
            {
                return Some(target);
            }
        }

        return None;
    }

    pub fn find_target_by_name_checked(&self, name: &str) -> &Target
    {
        match self.find_target_by_name(name)
        {
            Some(target) => target,
            None => panic!("Could not find target with name [{}].", name),
        }
    }
}

#[derive(Serialize, Deserialize)]
pub struct Target
{
    pub name: String,
    pub defines: Vec<String>,
    pub modules: Vec<Module>,
}

impl Target
{
    pub fn make_build_target_name(&self, build_config: &BuildConfig) -> String
    {
        return format!("{}-{}", self.name, build_config.name);
    }

    pub fn find_module_by_name(&self, name: &str) -> Option<&Module>
    {
        for module in self.modules.iter()
        {
            if module.name == name
            {
                return Some(module);
            }
        }

        for module in self.modules.iter()
        {
            if module.relative_dir == name
            {
                return Some(module);
            }
        }

        return None;
    }

    pub fn find_module_by_name_checked(&self, name: &str) -> &Module
    {
        match self.find_module_by_name(name)
        {
            Some(module) => module,
            None => panic!("Could not find module with name [{}].", name),
        }
    }
}

#[derive(Serialize, Deserialize)]
pub enum ModuleKind
{
    Shared,
    Static,
    Launch,
}

impl ModuleKind
{
    pub fn clone(&self) -> ModuleKind
    {
        match self
        {
            ModuleKind::Shared => ModuleKind::Shared,
            ModuleKind::Static => ModuleKind::Static,
            ModuleKind::Launch => ModuleKind::Launch,
        }
    }

    pub fn is_shared(&self) -> bool
    {
        match self
        {
            ModuleKind::Shared => true,
            _ => false,
        }
    }

    pub fn is_static(&self) -> bool
    {
        match self
        {
            ModuleKind::Static => true,
            _ => false,
        }
    }

    pub fn is_launch(&self) -> bool
    {
        match self
        {
            ModuleKind::Launch => true,
            _ => false,
        }
    }

    pub fn from_str(kind: &str) -> Option<ModuleKind>
    {
        return match kind
        {
            "Shared" => Some(ModuleKind::Shared),
            "SharedLib" => Some(ModuleKind::Shared),
            "SHARED_LIBRARY" => Some(ModuleKind::Shared),
            "Static" => Some(ModuleKind::Static),
            "StaticLib" => Some(ModuleKind::Static),
            "STATIC_LIBRARY" => Some(ModuleKind::Static),
            "Launch" => Some(ModuleKind::Launch),
            "ConsoleApp" => Some(ModuleKind::Launch),
            "EXECUTABLE" => Some(ModuleKind::Launch),
            _ => None,
        }
    }
}

#[derive(Serialize, Deserialize)]
pub struct Module
{
    pub name: String,
    pub friendly_name: String,
    pub relative_dir: String,
    pub pch: bool,
    pub pch_content: String,
    pub kind: ModuleKind,
    pub entry: String,
    pub pre_builds: Vec<String>,
    pub post_builds: Vec<String>,
    pub public_dependencies: Vec<String>,
    pub private_dependencies: Vec<String>,
    pub native_includes: Vec<String>,
    pub native_dependencies: Vec<String>,
    pub native_runtime_dependencies: Vec<String>,
    pub native_additional_runtime_dependencies: Vec<String>,
}

impl Module
{
    pub fn is_equal_str(&self, name: &str) -> bool
    {
        return self.name == name || self.relative_dir == name;
    }

    pub fn get_functional_rel_dir(&self) -> String
    {
        return self.relative_dir[1..].to_string();
    }

    pub fn get_functional_rel_source_dir(&self) -> String
    {
        return format!("{}/Source", self.get_functional_rel_dir());
    }

    pub fn get_functional_parent_rel_dir(&self) -> String
    {
        let mut parts: Vec<&str> = self.relative_dir[1..].split('/').collect();
        parts.pop();
        return parts.join("/");
    }

    pub fn get_all_dependencies(&self) -> Vec<String>
    {
        let mut result: Vec<String> = Vec::new();
        result.extend(self.public_dependencies.clone());
        result.extend(self.private_dependencies.clone());
        return result;
    }

    pub fn get_public_dependencies(&self, out: &mut Vec<String>)
    {
        for dependency in self.public_dependencies.iter()
        {
            if !out.contains(dependency)
            {
                out.push(dependency.clone());
            }

            continue
        }

        return;
    }

    fn get_public_dependencies_transitive(&self, target: &Target, out: &mut Vec<String>)
    {
        for dependency in self.public_dependencies.iter()
        {
            if out.contains(dependency) == false
            {
                out.push(dependency.clone());
                target.find_module_by_name_checked(dependency).get_public_dependencies_transitive(target, out);
            }

            continue
        }

        return
    }

    pub fn get_dependencies_transitive(&self, target: &Target) -> Vec<String>
    {
        let mut result: Vec<String> = Vec::new();
        self.get_public_dependencies_transitive(target, &mut result);

        for dependency in self.private_dependencies.iter()
        {
            if result.contains(dependency) == false
            {
                result.push(dependency.clone());
            }

            continue
        }

        if result.contains(&self.name)
        {
            panic!("Module [{}] has a circular dependency.", self.name);
        }

        return result;
    }
}
