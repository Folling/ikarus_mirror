use serde::de::Error;
use serde::{Deserialize, Deserializer};
use std::fs::File;

use std::io::Write;

use crate::util::split_maybe_once;

use super::parameter_validation_type::ParameterValidationType;

#[derive(Debug)]
pub struct FlagCondition {
    pub inverted: bool,
    pub flag: String,
}

#[derive(Debug)]
pub struct ParameterValidation {
    pub r#type: ParameterValidationType,
    pub flag_condition: Option<FlagCondition>,
}

impl<'a> Deserialize<'a> for ParameterValidation {
    fn deserialize<D: Deserializer<'a>>(deserializer: D) -> Result<Self, D::Error> {
        let str: &str = Deserialize::deserialize(deserializer)?;

        if str.chars().all(|c| c.is_whitespace()) {
            return Err(D::Error::custom("empty or blank parameter validation"));
        }

        return match split_maybe_once(str, '@') {
            (name, Some(flag)) => {
                let mut flag_chars = flag.trim().chars().peekable();

                let (flag, inverted) = match flag_chars.peek() {
                    Some('!') => (
                        flag_chars.skip(1).collect::<String>().trim().to_string(),
                        true,
                    ),
                    Some(_) => (flag_chars.collect::<String>().trim().to_string(), false),
                    None => return Err(D::Error::custom("invalid parameter validation format")),
                };

                Ok(ParameterValidation {
                    r#type: ParameterValidationType::parse(name.trim())
                        .map_err(D::Error::custom)?,
                    flag_condition: Some(FlagCondition { inverted, flag }),
                })
            }
            (name, None) => Ok(ParameterValidation {
                r#type: ParameterValidationType::parse(name.trim()).map_err(D::Error::custom)?,
                flag_condition: None,
            }),
        };
    }
}

impl ParameterValidation {
    pub fn generate(
        &self,
        file: &mut File,
        indent: usize,
        flag_enum_name: impl AsRef<str> + std::fmt::Display,
        param_name: impl AsRef<str> + std::fmt::Display,
    ) -> anyhow::Result<()> {
        let mut indent = indent;

        if let Some(condition) = &self.flag_condition {
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

        let call = match &self.r#type {
            ParameterValidationType::NotNull => {
                format!("validate_not_null(transformed_{})", param_name.as_ref())
            }
            ParameterValidationType::IdNotNull => {
                format!("validate_id_not_null(transformed_{})", param_name.as_ref())
            }
            ParameterValidationType::IdNotNone => {
                format!("validate_id_not_none(transformed_{})", param_name.as_ref())
            }
            ParameterValidationType::IdSpecified => {
                format!("validate_id_specified(transformed_{})", param_name.as_ref())
            }
            ParameterValidationType::Exists => {
                format!("validate_exists(transformed_project, transformed_{})", param_name.as_ref())
            }
            ParameterValidationType::PositionWithinBounds {
                bounds_folder_object,
            } => format!(
                "validate_position_within_bounds(transformed_project, transformed_{}, transformed_{})",
                param_name.as_ref(), bounds_folder_object
            ),
            ParameterValidationType::Is { expected_types } => format!(
                "validate_is(transformed_{}, {})",
                param_name.as_ref(),
                expected_types
                    .iter()
                    .map(|c| format!("EntityTypes_{}", c.trim()))
                    .collect::<Vec<String>>()
                    .join(" | ")
            ),
            ParameterValidationType::ValidPath => format!("validate_path(transformed_{})", param_name.as_ref()),
            ParameterValidationType::ValidUtf8 => format!("validate_utf8(transformed_{})", param_name.as_ref()),
            ParameterValidationType::NotBlank => {
                format!("validate_not_blank(transformed_{})", param_name.as_ref())
            }
            ParameterValidationType::PathExists => {
                format!("validate_path_exists(transformed_{})", param_name.as_ref())
            }
            ParameterValidationType::PathParentMustExist => {
                format!("validate_path_parent_exists(transformed_{})", param_name.as_ref())
            }
            ParameterValidationType::ValidPropertyValue {
                type_source,
                settings_source,
            } => {
                format!(
                    "validate_property_value(transformed_{}, transformed_{}, transformed_{})",
                    type_source, param_name.as_ref(), settings_source
                )
            }
            ParameterValidationType::ValidPropertyValueDb { property_source } => {
                format!(
                    "validate_property_value_db(transformed_{}, transformed_{})",
                    property_source, param_name.as_ref()
                )
            }
            ParameterValidationType::ValidSettings { type_source } => {
                format!("validate_property_settings(transformed_{}, transformed_{})", type_source, param_name.as_ref())
            }
            ParameterValidationType::ValidSettingsDb { property_source } => {
                format!("validate_property_settings_db(transformed_{}, transformed_{})", property_source, param_name.as_ref())
            }
        };

        writeln!(
            file,
            "{:indent$}LOG_VERBOSE(\"validating {} with validation {}\");",
            "",
            param_name.as_ref(),
            self.r#type
        )?;

        writeln!(file, "{:indent$}if (!util::{call}) {{", "")?;

        indent += 4;

        writeln!(
            file,
            "{:indent$}LOG_ERROR(\"validation of '{}' parameter failed\");",
            "",
            param_name.as_ref()
        )?;

        writeln!(
            file,
            "{:indent$}ret.status_code = StatusCode_{};",
            "",
            self.r#type.error_type()
        )?;

        writeln!(file, "{:indent$}return ret;", "")?;

        indent -= 4;

        writeln!(file, "{:indent$}}}", "")?;

        if self.flag_condition.is_some() {
            indent -= 4;

            writeln!(file, "{:indent$}}}", "")?;
        }

        Ok(())
    }
}
