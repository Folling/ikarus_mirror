use serde_derive::Deserialize;

#[derive(Deserialize, Debug, Ord, PartialOrd, Eq, PartialEq)]
pub enum DbAccess {
    #[serde(rename = "none")]
    None,
    #[serde(rename = "readonly")]
    ReadOnly,
    #[serde(rename = "readwrite")]
    ReadWrite,
}
