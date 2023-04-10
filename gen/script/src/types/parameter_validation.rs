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
