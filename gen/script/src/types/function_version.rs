use std::fmt::Display;
use std::fs::File;
use std::io::Write;

use serde_derive::Deserialize;

use crate::util::write_commented;

use super::flag::Flag;
use super::parameter::Parameter;
use super::return_type_member::ReturnTypeMember;

#[derive(Debug, Deserialize)]
pub struct FunctionVersion {
    pub return_type: Vec<ReturnTypeMember>,
    pub parameters: Vec<Parameter>,
    pub flags: Vec<Flag>,
    #[serde(default)]
    pub description: String,
}

impl FunctionVersion {
    pub fn generate<
        S1: AsRef<str> + Display,
        S2: AsRef<str> + Display,
        S3: AsRef<str> + Display,
        S4: AsRef<str> + Display,
    >(
        &self,
        file: &mut File,
        type_name: S1,
        type_name_pascal: S2,
        func_name: S3,
        func_name_pascal: S4,
        version: usize,
    ) -> std::io::Result<()> {
        let flag_enum_name = format!("Ikarus{type_name_pascal}{func_name_pascal}V{version}Flags");
        let return_type_name =
            format!("Ikarus{type_name_pascal}{func_name_pascal}V{version}Result");

        let args = self
            .parameters
            .iter()
            .map(|param| format!("{} {}", param.r#type, param.name))
            .collect::<Vec<String>>()
            .join(", ");

        writeln!(file, "\nenum {flag_enum_name} {{")?;

        writeln!(
            file,
            "    // Pass this if you don't want any special behaviour to occur."
        )?;
        write!(file, "    {flag_enum_name}_None = 0")?;

        for (i, flag) in self.flags.iter().enumerate() {
            writeln!(file, ",")?;
            write_commented(file, &flag.description, 4)?;
            write!(file, "    {flag_enum_name}_{} = 1 << {i}", flag.name)?;
        }

        writeln!(file, "\n}};")?;

        writeln!(file, "\nstruct {return_type_name} {{")?;

        for (i, member) in self.return_type.iter().enumerate() {
            write_commented(file, &member.description, 4)?;
            write!(file, "    {} {};", member.r#type, member.name)?;
            if i != self.return_type.len() - 1 {
                writeln!(file, ",")?;
            } else {
                writeln!(file, "")?;
            }
        }

        writeln!(file, "    // The result of the operation.")?;
        writeln!(file, "    StatusCode status_code;")?;

        writeln!(file, "}};\n")?;

        write_commented(file, &self.description, 0)?;

        write_commented(file, "Parameters: ", 0)?;

        for param in &self.parameters {
            write_commented(file, format!("- {}: {}", param.name, param.description), 0)?;
        }

        write_commented(
            file,
            format!("- flags: Optional behaviours that can be toggled on."),
            0,
        )?;

        writeln!(file, "{return_type_name} ikarus_{type_name}_{func_name}_v{version}({args}, {flag_enum_name} flags);")?;

        Ok(())
    }
}
