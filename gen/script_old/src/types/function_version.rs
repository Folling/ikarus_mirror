use std::fmt::Display;
use std::fs::File;
use std::io::Write;

use crate::types::db_access::DbAccess;
use crate::types::parameter_validation_type::{ParameterValidationType, ValidationPrecedence};
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
    ) -> anyhow::Result<()> {
        let flag_enum_name = format!("Ikarus{type_name_pascal}{func_name_pascal}V{version}Flags");
        let return_type_name =
            format!("Ikarus{type_name_pascal}{func_name_pascal}V{version}Result");

        let args = self
            .parameters
            .iter()
            .map(|param| format!("{} {}", param.r#type, param.name))
            .intersperse(String::from(", "))
            .collect::<String>();

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
    ) -> anyhow::Result<()> {
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

        // in the future we could log the flags as strings
        writeln!(
            file,
            "{:indent$}LOG_{}(\"Calling {}({}) with flags: {{}}\", {}, static_cast<int>(flags));\n",
            "",
            self.log_level.to_uppercase(),
            full_func_name,
            self.parameters
                .iter()
                .filter(|p| p.log)
                .map(|p| format!("{}: {{}}", p.name))
                .intersperse(String::from(", "))
                .collect::<String>(),
            self.parameters
                .iter()
                .filter(|p| p.log)
                .map(|p| {
                    if p.r#type.contains("*") {
                        format!("fmt::ptr({})", p.name)
                    } else {
                        p.name.clone()
                    }
                })
                .intersperse(String::from(", "))
                .collect::<String>()
        )?;

        writeln!(file, "{:indent$}{return_type_name} ret{{}};\n", "")?;

        writeln!(
            file,
            "{:indent$}LOG_VERBOSE(\"validation pre-transformation validations\");\n",
            ""
        )?;

        for param in &self.parameters {
            for validation in &param.validations {
                if validation.r#type.precedence() != ValidationPrecedence::BeforeTransformation {
                    continue;
                }

                validation.generate(file, indent, &param.name, &flag_enum_name)?
            }

            // db handle must only be fetched after the project has been validated
            if param.r#type == "Project" {
                writeln!(file, "LOG_VERBOSE(\"fetching DB handle\");")?;

                match self.db_access {
                    DbAccess::None => {}
                    DbAccess::ReadOnly => {
                        writeln!(file, "\n{:indent$}auto db_handle = sqlitecpp::DbHandle::read_lock(project.handle->get_db_mutex());", "")?;
                    }
                    DbAccess::ReadWrite => {
                        writeln!(file, "\n{:indent$}auto db_handle = sqlitecpp::DbHandle::write_lock(project.handle->get_db_mutex());", "")?;
                    }
                }
            }
        }

        writeln!(
            file,
            "{:indent$}LOG_VERBOSE(\"Transforming all arguments\");\n",
            ""
        )?;

        writeln!(file, "{:indent$}bool transformation_success{{}};", "")?;

        for param in &self.parameters {
            writeln!(
                file,
                "{:indent$}auto transformed_{} = util::Transformer<cppbase::remove_all<decltype({})>>::transform({}, &transformation_success);",
                "", param.name, param.name, param.name
            )?;

            writeln!(file, "{:indent$}if (!transformation_success) {{", "")?;

            indent += 4;

            writeln!(file, "{:indent$}return ret;", "")?;

            indent -= 4;

            writeln!(file, "{:indent$}}}", "")?;
        }

        for flag in &self.flags {
            if let Some(mutex) = &flag.mutex {
                writeln!(
                    file,
                    "{:indent$}LOG_VERBOSE(\"validating the mutual exclusives of flag '{}'\");",
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

                writeln!(file, "{:indent$}LOG_ERROR(\"Flag '{}' is mutually exclusive with ({}) but two or more were set.\");", "", flag.name, mutex.join(", "))?;
                writeln!(
                    file,
                    "{:indent$}ret.status_code = StatusCode_InvalidArgument;",
                    ""
                )?;
                writeln!(file, "{:indent$}return ret;", "")?;
                indent -= 4;

                writeln!(file, "{:indent$}}}\n", "")?;
            }
        }

        writeln!(
            file,
            "LOG_VERBOSE(\"performing all post-transformation validations\");"
        )?;

        for param in &self.parameters {
            for validation in &param.validations {
                if validation.r#type.precedence() != ValidationPrecedence::AfterTransformation {
                    continue;
                }

                validation.generate(
                    file,
                    indent,
                    format!("transformed_{}", param.name),
                    &flag_enum_name,
                )?
            }
        }

        let param_list = self
            .parameters
            .iter()
            .map(|p| format!("transformed_{}", p.name))
            .intersperse(String::from(", "))
            .collect::<String>();

        if let Some(member) = &self.r#return {
            writeln!(
                file,
                "\n{:indent$}{full_func_name}_impl({}, flags).veput_into(ret.{}, ret.status_code);",
                "", param_list, member.name,
            )?;
        } else {
            writeln!(
                file,
                "\n{:indent$}{full_func_name}_impl({}, flags).eput_into(ret.status_code);",
                "", param_list
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

        writeln!(file, "{:indent$}{cpp_return_type_name} ikarus_{type_name}_{func_name}_v{version}_wrapper({typed_parameters}, {flag_enum_name} flags) {{", "")?;

        indent += 4;

        let parameter_list = self
            .parameters
            .iter()
            .map(|p| p.name.clone())
            .intersperse(String::from(", "))
            .collect::<String>();

        writeln!(file, "{:indent$} if(auto ret = ikarus_{type_name}_{func_name}_v{version}({parameter_list}, flags); ret.status_code != StatusCode_Ok) {{", "")?;

        indent += 4;

        writeln!(file, "{:indent$}return cppbase::err(ret.status_code);", "")?;

        indent -= 4;

        writeln!(file, "{:indent$}}} else {{", "")?;

        indent += 4;

        if let Some(member) = &self.r#return {
            writeln!(
                file,
                "{:indent$}return cppbase::ok(ret.{});",
                "", member.name
            )?;
        } else {
            writeln!(file, "{:indent$}return cppbase::ok();", "")?;
        };

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
    ) -> anyhow::Result<()> {
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

        // some validations need to happen BEFORE transformation, some AFTER
        // this can be identified based on the validation type and should then be generated accordingly

        let typed_transformed_parameters = self
            .parameters
            .iter()
            .map(|param| {
                format!(
                    "util::Transformer<cppbase::remove_all<{}>>::type {}",
                    param.r#type, param.name
                )
            })
            .intersperse(String::from(", "))
            .collect::<String>();

        writeln!(file, "\n{return_type_name} ikarus_{type_name}_{func_name}_v{version}_wrapper({typed_parameters}, {flag_enum_name} flags);")?;

        writeln!(file, "\n{return_type_name} ikarus_{type_name}_{func_name}_v{version}_impl({typed_transformed_parameters}, {flag_enum_name} flags);")?;

        Ok(())
    }
}
