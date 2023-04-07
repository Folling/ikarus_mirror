use std::collections::HashMap;
use std::fmt::Display;
use std::fs::File;

use super::function_version::FunctionVersion;

use crate::util::make_pascal_case;
use serde_derive::Deserialize;

use std::io::Write;

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

        for (version, func_version) in self.versions.iter().enumerate() {
            let enum_name = format!("Ikarus{type_name_pascal}{func_name_pascal}V{version}Flags");
            let return_type_name =
                format!("Ikarus{type_name_pascal}{func_name_pascal}V{version}Result");

            let args = func_version
                .parameters
                .iter()
                .map(|param| format!("{} {}", param.r#type, param.name))
                .collect::<Vec<String>>()
                .join(", ");

            let enum_values = func_version
                .flags
                .iter()
                .enumerate()
                .map(|(i, value)| {
                    format!(
                        "\n    {enum_name}_{} = 1 << {i}",
                        make_pascal_case(&value.name)
                    )
                })
                .collect::<Vec<String>>()
                .join(",");

            let return_type_members = func_version
                .return_type
                .iter()
                .map(|member| format!("\n    {} {};", member.r#type, member.name))
                .collect::<Vec<String>>()
                .join("");

            writeln!(
                file,
                "\nenum {enum_name} {{\n    {enum_name}_None = 0,{enum_values}\n}};"
            )
            .expect("unable to write to file");

            writeln!(file, "\nstruct {return_type_name} {{{return_type_members}\n    StatusCode status_code;\n}};").expect("unable to write to file");

            writeln!(file, "\n{return_type_name} ikarus_{type_name}_{func_name}_v{version}({args}, {enum_name} flags);").expect("unable to write to file");
        }

        Ok(())
    }
}
