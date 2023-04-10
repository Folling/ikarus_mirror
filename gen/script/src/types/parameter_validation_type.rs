use anyhow::bail;

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
    PathParentMustExist,
    ValidPropertyValue { type_source: String },
    ValidPropertyValueDb { property_source: String },
}

impl ParameterValidationType {
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
                content
            );
        }
    }
}
