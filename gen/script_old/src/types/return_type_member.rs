use serde_derive::Deserialize;

use crate::util;
use util::sanitised_string;

#[derive(Debug, Deserialize)]
pub struct ReturnTypeMember {
    #[serde(deserialize_with = "sanitised_string")]
    pub name: String,
    pub description: String,
    #[serde(deserialize_with = "sanitised_string")]
    pub r#type: String,
}
