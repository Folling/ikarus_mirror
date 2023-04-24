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
    pub fn generate_include_header(
        &self,
        file: &mut File,
        sub_path: &PathBuf,
    ) -> anyhow::Result<()> {
        writeln!(file, "// AUTOGENERATED CODE, DO NOT MANUALLY EDIT")?;
        writeln!(file, "#pragma once\n")?;

        let type_name = &self.name;
        let type_name_pascal = make_pascal_case(type_name);

        writeln!(file, "#if defined(__cplusplus)\n")?;

        for header in ["cstdbool", "cstddef", "cstdint"] {
            writeln!(file, "#include <{header}>")?;
        }

        writeln!(file, "")?;

        writeln!(file, "using std::size_t;")?;

        writeln!(file, "extern \"C\" {{")?;

        writeln!(file, "\n#elif\n")?;

        for header in ["stdbool.h", "stddef.h", "stdint.h"] {
            writeln!(file, "#include <{header}>")?;
        }

        writeln!(file, "\n#endif")?;

        writeln!(file, "")?;

        writeln!(
            file,
            "#include \"{}macros.h\"",
            "../".repeat(sub_path.components().count() - 1)
        )?;

        for dependency in &self.depends_on {
            writeln!(file, "#include \"{dependency}.h\"")?;
        }

        writeln!(file, "")?;

        write_commented(file, &self.description, 0, 0, 0)?;

        if self.generate_struct {
            writeln!(file, "struct {type_name_pascal};")?;
        }

        for func in &self.functions {
            func.generate(file, &type_name, &type_name_pascal)?;
        }

        writeln!(file, "\n#if defined(__cplusplus)")?;

        writeln!(file, "}}")?;

        writeln!(file, "#endif")?;

        Ok(())
    }

    pub fn generate_include_source(
        &self,
        file: &mut File,
        sub_path: &PathBuf,
    ) -> anyhow::Result<()> {
        writeln!(file, "// AUTOGENERATED CODE, DO NOT MANUALLY EDIT")?;
        writeln!(
            file,
            "#include \"ikarus/{}\"\n",
            sub_path.with_extension("h").display(),
        )?;
        writeln!(
            file,
            "#include <generated/{}>\n",
            sub_path.with_extension("hpp").display()
        )?;

        writeln!(file, "#include <cppbase/logger.hpp>\n")?;

        writeln!(file, "#include <sqlitecpp/db_handle.hpp>\n")?;

        writeln!(file, "#include <impl/project.hpp>")?;
        writeln!(file, "#include <impl/util/format.hpp>")?;
        writeln!(file, "#include <impl/util/validation.hpp>")?;

        writeln!(file)?;

        let type_name = &self.name;
        let type_name_pascal = make_pascal_case(type_name);

        for func in &self.functions {
            func.generate_include_source(file, &type_name, &type_name_pascal)?;
        }

        Ok(())
    }

    pub fn generate_impl_header(&self, file: &mut File, sub_path: &PathBuf) -> anyhow::Result<()> {
        writeln!(file, "// AUTOGENERATED CODE, DO NOT MANUALLY EDIT")?;
        writeln!(file, "#pragma once\n")?;

        writeln!(file, "#include <cppbase/result.hpp>",)?;

        writeln!(file, "#include <ikarus/status.h>",)?;
        writeln!(
            file,
            "#include <ikarus/{}>",
            sub_path.with_extension("h").display()
        )?;

        let type_name = &self.name;
        let type_name_pascal = make_pascal_case(type_name);

        for func in &self.functions {
            func.generate_impl_header(file, &type_name, &type_name_pascal)?;
        }

        Ok(())
    }
}
