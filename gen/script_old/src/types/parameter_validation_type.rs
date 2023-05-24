use anyhow::{anyhow, bail};
use std::fmt::{Display, Formatter};

#[derive(Debug, Eq, PartialEq)]
pub enum ParameterValidationType {
    NotNull,
    IdNotNull,
    IdNotNone,
    IdSpecified,
    Exists,
    PositionWithinBounds {
        bounds_folder_object: String,
    },
    Is {
        expected_types: Vec<String>,
    },
    ValidPath,
    ValidUtf8,
    NotBlank,
    PathExists,
    PathParentMustExist,
    ValidPropertyValue {
        type_source: String,
        settings_source: String,
    },
    ValidPropertyValueDb {
        property_source: String,
    },
    ValidSettings {
        type_source: String,
    },
    ValidSettingsDb {
        property_source: String,
    },
}

#[derive(Debug, Eq, PartialEq)]
pub enum ValidationPrecedence {
    BeforeTransformation,
    AfterTransformation,
}

impl Display for ParameterValidationType {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            ParameterValidationType::NotNull => f.write_str("NotNull"),
            ParameterValidationType::IdNotNull => f.write_str("IdNotNull"),
            ParameterValidationType::IdNotNone => f.write_str("IdNotNone"),
            ParameterValidationType::IdSpecified => f.write_str("IdSpecified"),
            ParameterValidationType::Exists => f.write_str("Exists"),
            ParameterValidationType::PositionWithinBounds { .. } => {
                f.write_str("PositionWithinBounds")
            }
            ParameterValidationType::Is { .. } => f.write_str("Is"),
            ParameterValidationType::ValidPath => f.write_str("ValidPath"),
            ParameterValidationType::ValidUtf8 => f.write_str("ValidUtf8"),
            ParameterValidationType::NotBlank => f.write_str("NotBlank"),
            ParameterValidationType::PathExists => f.write_str("PathExists"),
            ParameterValidationType::PathParentMustExist => f.write_str("PathParentMustExist"),
            ParameterValidationType::ValidPropertyValue { .. } => f.write_str("ValidPropertyValue"),
            ParameterValidationType::ValidPropertyValueDb { .. } => {
                f.write_str("ValidPropertyValueDb")
            }
            ParameterValidationType::ValidSettings { .. } => f.write_str("ValidSettings"),
            ParameterValidationType::ValidSettingsDb { .. } => f.write_str("ValidSettingsDb"),
        }
    }
}

impl ParameterValidationType {
    pub fn precedence(&self) -> ValidationPrecedence {
        match *self {
            ParameterValidationType::NotNull => ValidationPrecedence::BeforeTransformation,
            ParameterValidationType::IdNotNull => ValidationPrecedence::BeforeTransformation,
            ParameterValidationType::IdNotNone => ValidationPrecedence::BeforeTransformation,
            ParameterValidationType::IdSpecified => ValidationPrecedence::BeforeTransformation,
            ParameterValidationType::Exists => ValidationPrecedence::AfterTransformation,
            ParameterValidationType::PositionWithinBounds { .. } => {
                ValidationPrecedence::AfterTransformation
            }
            ParameterValidationType::Is { .. } => ValidationPrecedence::AfterTransformation,
            ParameterValidationType::ValidPath => ValidationPrecedence::AfterTransformation,
            ParameterValidationType::ValidUtf8 => ValidationPrecedence::AfterTransformation,
            ParameterValidationType::NotBlank => ValidationPrecedence::BeforeTransformation,
            ParameterValidationType::PathExists => ValidationPrecedence::AfterTransformation,
            ParameterValidationType::PathParentMustExist => {
                ValidationPrecedence::BeforeTransformation
            }
            ParameterValidationType::ValidPropertyValue { .. } => {
                ValidationPrecedence::AfterTransformation
            }
            ParameterValidationType::ValidPropertyValueDb { .. } => {
                ValidationPrecedence::AfterTransformation
            }
            ParameterValidationType::ValidSettings { .. } => {
                ValidationPrecedence::AfterTransformation
            }
            ParameterValidationType::ValidSettingsDb { .. } => {
                ValidationPrecedence::AfterTransformation
            }
        }
    }

    pub fn error_type(&self) -> &'static str {
        match *self {
            ParameterValidationType::NotNull => "InvalidArgument",
            ParameterValidationType::IdNotNull => "InvalidArgument",
            ParameterValidationType::IdNotNone => "InvalidArgument",
            ParameterValidationType::IdSpecified => "InvalidArgument",
            ParameterValidationType::Exists => "NotFound",
            ParameterValidationType::PositionWithinBounds { .. } => "InvalidArgument",
            ParameterValidationType::Is { .. } => "InvalidArgument",
            ParameterValidationType::ValidPath => "InvalidArgument",
            ParameterValidationType::ValidUtf8 => "InvalidArgument",
            ParameterValidationType::NotBlank => "InvalidArgument",
            ParameterValidationType::PathExists => "NotFound",
            ParameterValidationType::PathParentMustExist => "NotFound",
            ParameterValidationType::ValidPropertyValue { .. } => "InvalidArgument",
            ParameterValidationType::ValidPropertyValueDb { .. } => "InvalidArgument",
            ParameterValidationType::ValidSettings { .. } => "InvalidArgument",
            ParameterValidationType::ValidSettingsDb { .. } => "InvalidArgument",
        }
    }
    pub fn parse(content: &str) -> anyhow::Result<ParameterValidationType> {
        // yes this is all super
        match &content[..] {
            "NotNull" => return Ok(ParameterValidationType::NotNull),
            "IdNotNull" => return Ok(ParameterValidationType::IdNotNull),
            "IdNotNone" => return Ok(ParameterValidationType::IdNotNone),
            "IdSpecified" => return Ok(ParameterValidationType::IdSpecified),
            "Exists" => return Ok(ParameterValidationType::Exists),
            "ValidPath" => return Ok(ParameterValidationType::ValidPath),
            "ValidUtf8" => return Ok(ParameterValidationType::ValidUtf8),
            "NotBlank" => return Ok(ParameterValidationType::NotBlank),
            "PathExists" => return Ok(ParameterValidationType::PathExists),
            "PathParentMustExist" => return Ok(ParameterValidationType::PathParentMustExist),
            _ => {}
        }

        if let Some((name, options)) = content.split_once('(') {
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
                "ValidPropertyValue" => {
                    let (type_source, settings_source) = options
                            .split_once(", ")
                            .map(|(l, r)| (l.to_string(), r.to_string()))
                        .ok_or_else(|| anyhow!("ValidPropertyValue validation options has invalid characters: {} (perhaps specified more than one)", options))?;

                    return Ok(ParameterValidationType::ValidPropertyValue {
                        type_source,
                        settings_source,
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
                "ValidSettings" => {
                    if options.chars().any(|c| !c.is_alphabetic() && c != '_') {
                        bail!("ValidSettings validation options has invalid characters: {} (perhaps specified more than one)", options);
                    }

                    return Ok(ParameterValidationType::ValidSettings {
                        type_source: options.to_string(),
                    });
                }
                "ValidSettingsDb" => {
                    if options.chars().any(|c| !c.is_alphabetic() && c != '_') {
                        bail!("ValidSettingsDb validation options has invalid characters: {} (perhaps specified more than one)", options);
                    }

                    return Ok(ParameterValidationType::ValidSettingsDb {
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
                content
            );
        }
    }
}
