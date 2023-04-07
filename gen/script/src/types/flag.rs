use serde_derive::Deserialize;

#[derive(Debug, Deserialize)]
pub struct Flag {
    pub name: String,
    pub description: String,
}
