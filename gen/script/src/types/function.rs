use std::collections::HashMap;
use std::fmt::Display;
use std::fs::File;
use std::io::Write;

use serde_derive::Deserialize;

use crate::util::{make_pascal_case, write_commented};

use super::function_version::FunctionVersion;

#[derive(Debug, Deserialize)]
pub struct Function {
    pub name: String,
    pub description: String,
    pub errors: HashMap<String, Vec<String>>,
    pub versions: Vec<FunctionVersion>,
}

impl Function {
    pub fn generate<S1: AsRef<str> + Display, S2: AsRef<str> + Display>(
        &self,
        file: &mut File,
        type_name: S1,
        type_name_pascal: S2,
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
}
