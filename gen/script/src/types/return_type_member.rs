use serde_derive::Deserialize;

#[derive(Debug, Deserialize)]
pub struct ReturnTypeMember {
    pub name: String,
    pub description: String,
    pub r#type: String,
}
