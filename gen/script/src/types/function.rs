use std::collections::HashMap;
use std::fmt::Display;
use std::fs::File;
use std::io::Write;

use serde_derive::Deserialize;

use crate::util::{make_pascal_case, sanitised_string, write_commented};

use super::function_version::FunctionVersion;

#[derive(Debug, Deserialize)]
pub struct Function {
    #[serde(deserialize_with = "sanitised_string")]
    pub name: String,
    pub log_level: String,
    pub description: String,
    pub errors: HashMap<String, Vec<String>>,
    pub versions: Vec<FunctionVersion>,
}

impl Function {
    pub fn generate(
        &self,
        file: &mut File,
        type_name: impl AsRef<str> + Display,
        type_name_pascal: impl AsRef<str> + Display,
    ) -> std::io::Result<()> {
        let func_name = &self.name;
        let func_name_pascal = make_pascal_case(func_name);

        writeln!(file, "")?;
        write_commented(file, format!("ikarus_{}_{}", type_name, func_name), 0)?;
        write_commented(file, &self.description, 0)?;

        for (version, func_version) in self.versions.iter().enumerate() {
            func_version.generate(
                file,
                &type_name,
                &type_name_pascal,
                &func_name,
                &func_name_pascal,
                version + 1,
            )?;
        }

        Ok(())
    }

    pub fn generate_include_source(
        &self,
        file: &mut File,
        type_name: impl AsRef<str> + Display,
        type_name_pascal: impl AsRef<str> + Display,
    ) -> std::io::Result<()> {
        let func_name = &self.name;
        let func_name_pascal = make_pascal_case(func_name);

        for (version, func_version) in self.versions.iter().enumerate() {
            func_version.generate_include_source(
                file,
                &type_name,
                &type_name_pascal,
                &func_name,
                &func_name_pascal,
                version + 1,
                &self.log_level,
            )?;
        }

        Ok(())
    }

    pub fn generate_impl_header(
        &self,
        file: &mut File,
        type_name: impl AsRef<str> + Display,
        type_name_pascal: impl AsRef<str> + Display,
    ) -> std::io::Result<()> {
        let func_name = &self.name;
        let func_name_pascal = make_pascal_case(func_name);

        for (version, func_version) in self.versions.iter().enumerate() {
            func_version.generate_impl_header(
                file,
                &type_name,
                &type_name_pascal,
                &func_name,
                &func_name_pascal,
                version + 1,
                &self.log_level,
            )?;
        }

        Ok(())
    }
}
