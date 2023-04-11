use std::fmt::Display;
use std::fs::File;
use std::io::Write;

use crate::types::parameter_validation_type::ParameterValidationType;
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

        writeln!(file, "IKA_API {return_type_name} ikarus_{type_name}_{func_name}_v{version}({args}, {flag_enum_name} flags);")?;

        Ok(())
    }
    pub fn generate_include_source<
        S1: AsRef<str> + Display,
        S2: AsRef<str> + Display,
        S3: AsRef<str> + Display,
        S4: AsRef<str> + Display,
        S5: AsRef<str> + Display,
    >(
        &self,
        file: &mut File,
        type_name: S1,
        type_name_pascal: S2,
        func_name: S3,
        func_name_pascal: S4,
        version: usize,
        log_level: S5,
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

        let mut indent = 0;

        let full_func_name = format!("ikarus_{type_name}_{func_name}_v{version}");

        writeln!(file, "{:indent$}{return_type_name} ikarus_{type_name}_{func_name}_v{version}({args}, {flag_enum_name} flags) {{", "")?;

        indent = 4;

        writeln!(
            file,
            "{:indent$}LOG_{}(\"Calling {}({}) with flags: {{}}\", {}, flags)\n",
            "",
            log_level.as_ref().to_uppercase(),
            full_func_name,
            self.parameters
                .iter()
                .map(|p| format!("{}: {{}}", p.name))
                .intersperse(String::from(", "))
                .collect::<String>(),
            self.parameters
                .iter()
                .map(|p| p.name.as_ref())
                .intersperse(", ")
                .collect::<String>()
        )?;

        writeln!(file, "{:indent$}{return_type_name} ret{{}};", "")?;

        for param in &self.parameters {
            for validation in &param.validation {
                writeln!(file)?;

                if let Some(condition) = &validation.flag_condition {
                    writeln!(
                        file,
                        "{:indent$}if ((flags & {}_{}) {} 0) {{",
                        "",
                        flag_enum_name,
                        condition.flag,
                        // == 0 to check if the flag is *not* set
                        if condition.inverted { "==" } else { "!=" }
                    )?;

                    indent += 4;
                }

                let call = match &validation.r#type {
                    ParameterValidationType::NotNull => {
                        format!("validate_not_null({})", param.name)
                    }
                    ParameterValidationType::IdNotNull => {
                        format!("validate_id_not_null({})", param.name)
                    }
                    ParameterValidationType::IdNotNone => {
                        format!("validate_id_not_none({})", param.name)
                    }
                    ParameterValidationType::IdSpecified => {
                        format!("validate_id_specified({})", param.name)
                    }
                    ParameterValidationType::Exists => format!("validate_exists({})", param.name),
                    ParameterValidationType::PositionWithinBounds {
                        bounds_folder_object,
                    } => format!(
                        "validate_position_within_bounds({}, {})",
                        param.name, bounds_folder_object
                    ),
                    ParameterValidationType::Is { expected_types } => format!(
                        "validate_is({}, {})",
                        param.name,
                        expected_types
                            .iter()
                            .map(|c| format!("EntityTypes_{}", c))
                            .collect::<Vec<String>>()
                            .join(" | ")
                    ),
                    ParameterValidationType::ValidPath => format!("validate_path({})", param.name),
                    ParameterValidationType::ValidUtf8 => format!("validate_utf8({})", param.name),
                    ParameterValidationType::NotBlank => format!("validate_path({})", param.name),
                    ParameterValidationType::PathParentMustExist => {
                        format!("validate_path_parent_exists({})", param.name)
                    }
                    ParameterValidationType::ValidPropertyValue { type_source } => {
                        format!("validate_property_value({}, {})", type_source, param.name)
                    }
                    ParameterValidationType::ValidPropertyValueDb { property_source } => format!(
                        "validate_property_value_db({}, {})",
                        property_source, param.name
                    ),
                };

                writeln!(
                    file,
                    "{:indent$}LOG_VERBOSE(\"validating {} with validation {}\")",
                    "", param.name, validation.r#type
                )?;

                writeln!(file, "{:indent$}if(!{call}) {{", "")?;

                indent += 4;

                writeln!(file, "{:indent$}LOG_ERROR(\"validation failed\")", "")?;

                writeln!(
                    file,
                    "{:indent$}ret.status_code = StatusCode_InvalidArgument;",
                    ""
                )?;

                writeln!(file, "{:indent$}return ret;", "")?;

                indent -= 4;

                writeln!(file, "{:indent$}}}", "")?;

                if validation.flag_condition.is_some() {
                    indent -= 4;

                    writeln!(file, "{:indent$}}}", "")?;
                }
            }
        }

        writeln!(
            file,
            "\n{:indent$}{full_func_name}_impl({}, &ret, flags);",
            "",
            self.parameters
                .iter()
                .map(|p| p.name.as_str())
                .intersperse(", ")
                .collect::<String>()
        )?;

        writeln!(file, "\n{:indent$}return ret;", "")?;

        indent -= 4;

        writeln!(file, "{:indent$}}}\n", "")?;

        Ok(())
    }
}
