use std::fmt::Display;
use std::fs::File;
use std::io::Write;

use crate::types::db_access::DbAccess;
use crate::types::parameter_validation_type::ParameterValidationType;
use serde_derive::Deserialize;

use crate::util::write_commented;

use super::flag::Flag;
use super::parameter::Parameter;
use super::return_type_member::ReturnTypeMember;

#[derive(Debug, Deserialize)]
pub struct FunctionVersion {
    pub r#return: Option<ReturnTypeMember>,
    pub parameters: Vec<Parameter>,
    pub flags: Vec<Flag>,
    pub log_level: String,
    pub db_access: DbAccess,
    #[serde(default)]
    pub description: String,
}

impl FunctionVersion {
    pub fn generate(
        &self,
        file: &mut File,
        type_name: impl AsRef<str> + Display,
        type_name_pascal: impl AsRef<str> + Display,
        func_name: impl AsRef<str> + Display,
        func_name_pascal: impl AsRef<str> + Display,
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
            write_commented(file, &flag.description, 4, 0, 0)?;
            write!(file, "    {flag_enum_name}_{} = 1 << {i}", flag.name)?;
        }

        writeln!(file, "\n}};")?;

        writeln!(file, "\nstruct {return_type_name} {{")?;

        if let Some(member) = &self.r#return {
            write_commented(file, &member.description, 4, 0, 0)?;
            write!(file, "    {} {};\n", member.r#type, member.name)?;
        }

        writeln!(file, "    // The result of the operation.")?;
        writeln!(file, "    StatusCode status_code;")?;

        writeln!(file, "}};\n")?;

        write_commented(file, &self.description, 0, 0, 0)?;

        write_commented(file, "Parameters: ", 0, 0, 0)?;

        writeln!(
            file,
            "// -----------------------------------------------------------------------------------------------------------------------------------------"
        )?;

        for param in &self.parameters {
            write_commented(
                file,
                format!("- {}: {}", param.name, param.description),
                0,
                0,
                2,
            )?;
            writeln!(
                file,
                "// -----------------------------------------------------------------------------------------------------------------------------------------"
            )?;
        }

        write_commented(
            file,
            format!("- flags: Optional behaviours that can be toggled on."),
            0,
            0,
            0,
        )?;

        writeln!(
            file,
            "// -----------------------------------------------------------------------------------------------------------------------------------------"
        )?;

        writeln!(file, "IKA_API {return_type_name} ikarus_{type_name}_{func_name}_v{version}({args}, {flag_enum_name} flags);")?;

        Ok(())
    }
    pub fn generate_include_source(
        &self,
        file: &mut File,
        type_name: impl AsRef<str> + Display,
        type_name_pascal: impl AsRef<str> + Display,
        func_name: impl AsRef<str> + Display,
        func_name_pascal: impl AsRef<str> + Display,
        version: usize,
    ) -> std::io::Result<()> {
        let flag_enum_name = format!("Ikarus{type_name_pascal}{func_name_pascal}V{version}Flags");
        let return_type_name =
            format!("Ikarus{type_name_pascal}{func_name_pascal}V{version}Result");

        let typed_parameters = self
            .parameters
            .iter()
            .map(|param| format!("{} {}", param.r#type, param.name))
            .intersperse(String::from(", "))
            .collect::<String>();

        let mut indent = 0;

        let full_func_name = format!("ikarus_{type_name}_{func_name}_v{version}");

        writeln!(file, "{:indent$}{return_type_name} ikarus_{type_name}_{func_name}_v{version}({typed_parameters}, {flag_enum_name} flags) {{", "")?;

        indent = 4;

        let log_parameters_list = self
            .parameters
            .iter()
            .map(|p| format!("{}: {{}}", p.name))
            .intersperse(String::from(", "))
            .collect::<String>();

        let parameter_list = self
            .parameters
            .iter()
            .map(|p| p.name.clone())
            .intersperse(String::from(", "))
            .collect::<String>();

        let log_parameter_arg_list = self
            .parameters
            .iter()
            .map(|p| {
                if p.r#type.contains("*") {
                    format!("fmt::ptr({})", p.name)
                } else {
                    p.name.clone()
                }
            })
            .intersperse(String::from(", "))
            .collect::<String>();

        writeln!(
            file,
            "{:indent$}LOG_{}(\"Calling {}({}) with flags: {{}}\", {}, flags);\n",
            "",
            self.log_level.to_uppercase(),
            full_func_name,
            log_parameters_list,
            log_parameter_arg_list
        )?;

        writeln!(file, "{:indent$}{return_type_name} ret{{}};", "")?;

        match self.db_access {
            DbAccess::None => {}
            DbAccess::ReadOnly => {
                writeln!(file, "{:indent$}auto handle = sqlitecpp::DbHandle::read_lock(project->get_db_mutex());", "")?;
            }
            DbAccess::ReadWrite => {
                writeln!(file, "{:indent$}auto handle = sqlitecpp::DbHandle::write_lock(project->get_db_mutex());", "")?;
            }
        }

        for flag in &self.flags {
            if let Some(mutex) = &flag.mutex {
                writeln!(
                    file,
                    "{:indent$}LOG_VERBOSE(\"validating flag {{}}'s mutual exclusivity\", {}",
                    "", flag.name
                )?;

                writeln!(
                    file,
                    "{:indent$}if((flags & {flag_enum_name}_{}) != 0 && (flags & ({})) != 0) {{",
                    "",
                    flag.name,
                    mutex
                        .iter()
                        .map(|name| format!("{flag_enum_name}_{}", name))
                        .intersperse(String::from("|"))
                        .collect::<String>()
                )?;

                indent += 4;

                writeln!(file, "{:indent$}LOG_ERROR(\"Flag {} is mutually exclusive with ({}) but two or more were set.\")", "", flag.name, mutex.join(", "))?;
                writeln!(
                    file,
                    "{:indent$}return cppbase::err(StatusCode_InvalidParameter);",
                    ""
                )?;

                indent -= 4;

                writeln!(file, "{:indent$}}}\n", "")?;
            }
        }

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
                    ParameterValidationType::Exists => {
                        format!("validate_exists(project, {})", param.name)
                    }
                    ParameterValidationType::PositionWithinBounds {
                        bounds_folder_object,
                    } => format!(
                        "validate_position_within_bounds(project, {}, {})",
                        param.name, bounds_folder_object
                    ),
                    ParameterValidationType::Is { expected_types } => format!(
                        "validate_is({}, {})",
                        param.name,
                        expected_types
                            .iter()
                            .map(|c| format!("EntityTypes_{}", c.trim()))
                            .collect::<Vec<String>>()
                            .join(" | ")
                    ),
                    ParameterValidationType::ValidPath => format!("validate_path({})", param.name),
                    ParameterValidationType::ValidUtf8 => format!("validate_utf8({})", param.name),
                    ParameterValidationType::NotBlank => {
                        format!("validate_not_blank({})", param.name)
                    }
                    ParameterValidationType::PathExists => {
                        format!("validate_path_exists({})", param.name)
                    }
                    ParameterValidationType::PathParentMustExist => {
                        format!("validate_path_parent_exists({})", param.name)
                    }
                    ParameterValidationType::ValidPropertyValue { type_source } => {
                        format!("validate_property_value({}, {})", type_source, param.name)
                    }
                    ParameterValidationType::ValidPropertyValueDb { property_source } => {
                        format!(
                            "validate_property_value_db({}, {})",
                            property_source, param.name
                        )
                    }
                    ParameterValidationType::ValidSettings { type_source } => {
                        format!("validate_settings({}, {})", type_source, param.name)
                    }
                    ParameterValidationType::ValidSettingsDb { property_source } => {
                        format!("validate_settings_db({}, {})", property_source, param.name)
                    }
                };

                writeln!(
                    file,
                    "{:indent$}LOG_VERBOSE(\"validating {} with validation {}\");",
                    "", param.name, validation.r#type
                )?;

                writeln!(file, "{:indent$}if (!{call}) {{", "")?;

                indent += 4;

                writeln!(file, "{:indent$}LOG_ERROR(\"validation failed\");", "")?;

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

        if let Some(member) = &self.r#return {
            writeln!(
                file,
                "\n{:indent$}{full_func_name}_impl({}, flags).veput_into(ret.{}, ret.status_code);",
                "", parameter_list, member.name,
            )?;
        } else {
            writeln!(
                file,
                "\n{:indent$}{full_func_name}_impl({}, flags).eput_into(ret.status_code);",
                "", parameter_list
            )?;
        }

        writeln!(
            file,
            "\n{:indent$}if (ret.status_code != StatusCode_Ok) {{",
            ""
        )?;

        indent += 4;

        writeln!(
            file,
            "{:indent$}LOG_{}(\"Error in {}: {{}}\", ret.status_code);",
            "",
            self.log_level.to_uppercase(),
            full_func_name
        )?;

        indent -= 4;

        writeln!(file, "{:indent$}}} else {{", "")?;

        indent += 4;

        writeln!(
            file,
            "{:indent$}LOG_ERROR(\"Finished {}\");",
            "", full_func_name
        )?;

        indent -= 4;

        writeln!(file, "{:indent$}}}", "")?;

        writeln!(file, "\n{:indent$}return ret;", "")?;

        indent -= 4;

        writeln!(file, "{:indent$}}}\n", "")?;

        // cpp wrapper using Result

        let cpp_return_type_name = if let Some(member) = &self.r#return {
            format!("cppbase::Result<{}, StatusCode>", member.r#type)
        } else {
            String::from("cppbase::Result<void, StatusCode>")
        };

        writeln!(file, "\n{:indent$}{cpp_return_type_name} ikarus_{type_name}_{func_name}_v{version}_wrapper({typed_parameters}, {flag_enum_name} flags) {{", "")?;

        indent += 4;

        writeln!(file, "{:indent$} if(auto ret = ikarus_{type_name}_{func_name}_v{version}({parameter_list}, flags); ret.status_code != StatusCode_Ok) {{", "")?;

        indent += 4;

        writeln!(file, "{:indent$}return cppbase::err(ret.status_code);", "")?;

        indent -= 4;

        writeln!(file, "{:indent$}}} else {{", "")?;

        indent += 4;

        let member = if let Some(member) = &self.r#return {
            member.name.clone()
        } else {
            String::from("")
        };

        writeln!(file, "{:indent$}return cppbase::ok({});", "", member)?;

        indent -= 4;

        writeln!(file, "{:indent$}}}", "")?;

        indent -= 4;

        writeln!(file, "{:indent$}}}\n", "")?;

        Ok(())
    }

    pub fn generate_impl_header(
        &self,
        file: &mut File,
        type_name: impl AsRef<str> + Display,
        type_name_pascal: impl AsRef<str> + Display,
        func_name: impl AsRef<str> + Display,
        func_name_pascal: impl AsRef<str> + Display,
        version: usize,
    ) -> std::io::Result<()> {
        let flag_enum_name = format!("Ikarus{type_name_pascal}{func_name_pascal}V{version}Flags");
        let return_type_name = if let Some(member) = &self.r#return {
            format!("cppbase::Result<{}, StatusCode>", member.r#type)
        } else {
            String::from("cppbase::Result<void, StatusCode>")
        };

        let typed_parameters = self
            .parameters
            .iter()
            .map(|param| format!("{} {}", param.r#type, param.name))
            .intersperse(String::from(", "))
            .collect::<String>();

        writeln!(file, "\n{return_type_name} ikarus_{type_name}_{func_name}_v{version}_wrapper({typed_parameters}, {flag_enum_name} flags);")?;

        writeln!(file, "\n{return_type_name} ikarus_{type_name}_{func_name}_v{version}_impl({typed_parameters}, {flag_enum_name} flags);")?;

        Ok(())
    }
}
