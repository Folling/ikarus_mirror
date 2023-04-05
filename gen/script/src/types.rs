use std::collections::HashMap;
use serde::{Deserialize, Deserializer};
use serde::de::Error;
use serde_derive::{Debug, Deserialize};

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

pub struct FlagCondition {
    pub inverted: bool,
    pub flag: String
}

#[derive(Debug)]
pub struct ParameterValidation {
    pub name: String,
    pub flag_condition: Option<FlagCondition>,
}

impl<'a> Deserialize<'a> for ParameterValidation {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'a>,
    {
        let str: &str = Deserializer::deserialize(deserializer)?;

        if str.chars().all(|c| c.is_whitespace()) {
            return Err(D::Error::custom("empty or blank parameter validation"));
        }

        return match str.split("@") {
            [name, flag] => {
                let flag_chars = flag.chars().peekable();

                let inverted = if (flag_chars.peek() == Some('!')) {
                    flag_chars.next();
                    true
                } else {
                    false
                };

                Ok(ParameterValidation {
                    name,
                    flag_condition: Some(FlagCondition {
                        flag: flag_chars.collect::<String>(),
                        inverted
                    })
                })
            },
            [name] => {
                Ok(ParameterValidation {
                    name,
                    flag_condition: None
                })
            },
            _ => {
                Err(D::Error::custom("invalid flag format"))
            }
        };
    }
}

#[derive(Debug, Deserialize)]
pub struct Parameter {
    pub name: String,
    pub description: String,
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
