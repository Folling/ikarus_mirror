use std::fs::File;
use std::io::Write;

use serde_derive::Deserialize;

use crate::util::{make_pascal_case, write_commented};

use super::function::Function;

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
}
