use serde::de::Error;
use serde::{Deserialize, Deserializer};
use serde_derive::Deserialize;
use std::collections::HashMap;

use anyhow::bail;

#[derive(Debug, Deserialize)]
pub struct Type {
    pub name: String,
    pub description: String,
    pub generate_struct: bool,
    pub depends_on: Vec<String>,
    pub functions: Vec<Function>,
}

#[derive(Debug, Deserialize)]
pub struct Function {
    pub name: String,
    pub description: String,
    pub errors: HashMap<String, Vec<String>>,
    pub versions: Vec<FunctionVersion>,
}

#[derive(Debug, Deserialize)]
pub struct FunctionVersion {
    pub return_type: Vec<ReturnTypeMember>,
    pub parameters: Vec<Parameter>,
    pub flags: Vec<Flag>,
}

#[derive(Debug)]
pub struct FlagCondition {
    pub inverted: bool,
    pub flag: String,
}

#[derive(Debug, Eq, PartialEq)]
pub enum ParameterValidationType {
    NotNull,
    IdNotNull,
    IdNotNone,
    IdSpecified,
    Exists,
    PositionWithinBounds { bounds_folder_object: String },
    Is { expected_types: Vec<String> },
    ValidPath,
    ValidUtf8,
    NotBlank,
    ParentMustExist { path_source: String },
    ValidPropertyValue { type_source: String },
    ValidPropertyValueDb { property_source: String },
}

fn param_validation(validation: &str) -> anyhow::Result<ParameterValidationType> {
    // yes this is all super
    match &validation[..] {
        "NotNull" => return Ok(ParameterValidationType::NotNull),
        "IdNotNull" => return Ok(ParameterValidationType::IdNotNull),
        "IdNotNone" => return Ok(ParameterValidationType::IdNotNone),
        "IdSpecified" => return Ok(ParameterValidationType::IdSpecified),
        "Exists" => return Ok(ParameterValidationType::Exists),
        "ValidPath" => return Ok(ParameterValidationType::ValidPath),
        "ValidUtf8" => return Ok(ParameterValidationType::ValidUtf8),
        "NotBlank" => return Ok(ParameterValidationType::NotBlank),
        _ => {}
    }

    if let Some((name, options)) = validation.split_once('(') {
        if !options.ends_with(")") {
            bail!("flag {} options are missing closing parenthesis", name);
        }

        let options = options.trim_end_matches(")");

        match &name[..] {
            "PositionWithinBounds" => {
                if options.chars().any(|c| !c.is_alphabetic() && c != '_') {
                    bail!("PositionWithinBounds validation options has invalid characters: {} (perhaps specified more than one)", options);
                }

                return Ok(ParameterValidationType::PositionWithinBounds {
                    bounds_folder_object: options.to_string(),
                });
            }
            "Is" => {
                let types = options
                    .split(",")
                    .map(|c| c.trim().to_string())
                    .collect::<Vec<String>>();

                if let Some(invalid) = types.iter().find(|r#type| {
                    !["Folder", "Page", "Template", "Property"].contains(&r#type.as_str())
                }) {
                    bail!(
                        "Is validation has an invalid entity type specified: {}.",
                        invalid
                    );
                }
                return Ok(ParameterValidationType::Is {
                    expected_types: options
                        .split(",")
                        .map(|c| c.to_string())
                        .collect::<Vec<String>>(),
                });
            }
            "ParentMustExist" => {
                if options.chars().any(|c| !c.is_alphabetic() && c != '_') {
                    bail!("ParentMustExist validation options has invalid characters: {} (perhaps specified more than one)", options);
                }

                return Ok(ParameterValidationType::ParentMustExist {
                    path_source: options.to_string(),
                });
            }
            "ValidPropertyValue" => {
                if options.chars().any(|c| !c.is_alphabetic() && c != '_') {
                    bail!("ValidPropertyValue validation options has invalid characters: {} (perhaps specified more than one)", options);
                }

                return Ok(ParameterValidationType::ValidPropertyValue {
                    type_source: options.to_string(),
                });
            }
            "ValidPropertyValueDb" => {
                if options.chars().any(|c| !c.is_alphabetic() && c != '_') {
                    bail!("ValidPropertyValueDb validation options has invalid characters: {} (perhaps specified more than one)", options);
                }

                return Ok(ParameterValidationType::ValidPropertyValueDb {
                    property_source: options.to_string(),
                });
            }
            _ => {
                bail!("invalid validation type: {}", name);
            }
        }
    } else {
        bail!(
            "non-trivial validation {} is missing options or doesn't exist",
            validation
        );
    }
}

#[derive(Debug)]
pub struct ParameterValidation {
    pub validation: ParameterValidationType,
    pub flag_condition: Option<FlagCondition>,
}

// pattern isn't stable yet
pub fn split_maybe_once(string: &str, delimiter: char) -> (&str, Option<&str>) {
    if let Some(idx) = string.find(delimiter) {
        unsafe {
            (
                string.get_unchecked(..idx),
                Some(string.get_unchecked(idx..)),
            )
        }
    } else {
        (string, None)
    }
}

impl<'a> Deserialize<'a> for ParameterValidation {
    fn deserialize<D: Deserializer<'a>>(deserializer: D) -> Result<Self, D::Error> {
        let str: &str = Deserialize::deserialize(deserializer)?;

        if str.chars().all(|c| c.is_whitespace()) {
            return Err(D::Error::custom("empty or blank parameter validation"));
        }

        return match split_maybe_once(str, '@') {
            (name, Some(flag)) => {
                let mut flag_chars = flag.chars().peekable();

                let inverted = if flag_chars.peek() == Some(&'!') {
                    flag_chars.next();
                    true
                } else {
                    false
                };

                Ok(ParameterValidation {
                    validation: param_validation(name.trim()).map_err(D::Error::custom)?,
                    flag_condition: Some(FlagCondition {
                        inverted,
                        flag: flag_chars.collect::<String>(),
                    }),
                })
            }
            (name, None) => Ok(ParameterValidation {
                validation: param_validation(name.trim()).map_err(D::Error::custom)?,
                flag_condition: None,
            }),
        };
    }
}

#[derive(Debug, Deserialize)]
pub struct Parameter {
    pub name: String,
    pub description: String,
    #[serde(default = "Vec::new")]
    pub validation: Vec<ParameterValidation>,
    pub r#type: String,
}

#[derive(Debug, Deserialize)]
pub struct ReturnTypeMember {
    pub name: String,
    pub description: String,
    pub r#type: String,
}

#[derive(Debug, Deserialize)]
pub struct Flag {
    pub name: String,
    pub description: String,
}
