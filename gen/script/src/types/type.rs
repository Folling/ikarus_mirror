use std::fs::File;
use std::io::Write;
use std::path::PathBuf;

use serde_derive::Deserialize;

use crate::util::{make_pascal_case, sanitised_string, write_commented};

use super::function::Function;

#[derive(Debug, Deserialize)]
pub struct Type {
    #[serde(deserialize_with = "sanitised_string")]
    pub name: String,
    pub description: String,
    pub generate_struct: bool,
    pub depends_on: Vec<String>,
    pub functions: Vec<Function>,
}

impl Type {
    pub fn generate_include_header(&self, file: &mut File, depth: usize) -> anyhow::Result<()> {
        let type_name = &self.name;
        let type_name_pascal = make_pascal_case(type_name);

        writeln!(file, "#pragma once\n")?;

        writeln!(file, "#if defined(__cplusplus)")?;

        for header in ["cstdbool", "cstddef", "cstdint"] {
            writeln!(file, "    #include <{header}>")?;
        }

        writeln!(file, "")?;

        writeln!(file, "    using std::size_t;")?;

        writeln!(file, "    extern \"C\" {{")?;

        writeln!(file, "#elif")?;

        for header in ["stdbool.h", "stddef.h", "stdint.h"] {
            writeln!(file, "    #include <{header}>")?;
        }

        writeln!(file, "#endif")?;

        writeln!(file, "")?;

        writeln!(file, "#include \"{}macros.h\"", "../".repeat(depth))?;

        for dependency in &self.depends_on {
            writeln!(file, "#include \"{dependency}.h\"")?;
        }

        writeln!(file, "")?;

        write_commented(file, &self.description, 0)?;

        if self.generate_struct {
            writeln!(file, "struct {type_name_pascal};")?;
        }

        for func in &self.functions {
            func.generate(file, &type_name, &type_name_pascal)?;
        }

        writeln!(file, "\n#if defined(__cplusplus)")?;

        writeln!(file, "    }}")?;

        writeln!(file, "#endif")?;

        Ok(())
    }

    pub fn generate_include_source(&self, file: &mut File, sub_path: &PathBuf, depth: usize) -> anyhow::Result<()> {
        writeln!(file, "#include \"\"")
        Ok(())
    }

    pub fn generate_impl_header(&self, file: &mut File, sub_path: &PathBuf, depth: usize) -> anyhow::Result<()> {
        Ok(())
    }
}
