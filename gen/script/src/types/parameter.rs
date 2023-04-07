use super::parameter_validation::ParameterValidation;

use serde_derive::Deserialize;

#[derive(Debug, Deserialize)]
pub struct Parameter {
    pub name: String,
    pub description: String,
    #[serde(default = "Vec::new")]
    pub validation: Vec<ParameterValidation>,
    pub r#type: String,
}
