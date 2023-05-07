use serde_derive::Deserialize;

use super::parameter_validation::ParameterValidation;

use crate::util::sanitised_string;

#[derive(Debug, Deserialize)]
pub struct Parameter {
    #[serde(deserialize_with = "sanitised_string")]
    pub name: String,
    pub description: String,
    #[serde(default = "Vec::new")]
    pub validation: Vec<ParameterValidation>,
    #[serde(deserialize_with = "sanitised_string")]
    pub r#type: String,
    pub log: bool,
}
