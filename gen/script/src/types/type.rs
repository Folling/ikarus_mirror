use crate::util::{make_pascal_case, write_commented};
use serde_derive::Deserialize;
use std::fs::File;

use super::function::Function;
use std::io::Write;

#[derive(Debug, Deserialize)]
pub struct Type {
    pub name: String,
    pub description: String,
    pub generate_struct: bool,
    pub depends_on: Vec<String>,
    pub functions: Vec<Function>,
}

impl Type {
    pub fn generate(&self, file: &mut File) -> std::io::Result<()> {
        let type_name = &self.name;
        let type_name_pascal = make_pascal_case(type_name);

        writeln!(file, "#pragma once\n")?;

        for header in ["stdbool.h", "stddef.h", "stdint.h"] {
            writeln!(file, "#include <{header}>")?;
        }

        writeln!(file, "")?;

        for dependency in &self.depends_on {
            writeln!(file, "#include \"{dependency}.h\"")?;
        }

        write!(file, "\n")?;

        write_commented(file, &self.description)?;

        write!(file, "\n")?;

        if self.generate_struct {
            writeln!(file, "struct {type_name_pascal};")?;
        }

        for func in &self.functions {
            func.generate(file, &type_name, &type_name_pascal)?;
        }

        Ok(())
    }
}
