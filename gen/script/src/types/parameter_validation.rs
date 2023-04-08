use serde::de::Error;
use serde::{Deserialize, Deserializer};

use crate::util::split_maybe_once;

use super::parameter_validation_type::ParameterValidationType;

#[derive(Debug)]
pub struct FlagCondition {
    pub inverted: bool,
    pub flag: String,
}

#[derive(Debug)]
pub struct ParameterValidation {
    pub validation: ParameterValidationType,
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
                let mut flag_chars = flag.chars().peekable();

                let inverted = if flag_chars.peek() == Some(&'!') {
                    flag_chars.next();
                    true
                } else {
                    false
                };

                Ok(ParameterValidation {
                    validation: ParameterValidationType::parse(name.trim())
                        .map_err(D::Error::custom)?,
                    flag_condition: Some(FlagCondition {
                        inverted,
                        flag: flag_chars.collect::<String>(),
                    }),
                })
            }
            (name, None) => Ok(ParameterValidation {
                validation: ParameterValidationType::parse(name.trim())
                    .map_err(D::Error::custom)?,
                flag_condition: None,
            }),
        };
    }
}
