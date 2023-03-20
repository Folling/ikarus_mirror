use serde_derive::{Serialize, Deserialize};

#[derive(Serialize, Deserialize)]
pub struct ReturnTypeMember {
    pub name: String,
    pub r#type: String,
}

#[derive(Serialize, Deserialize)]
pub struct Parameter {
    pub name: String,
    pub r#type: String,
}

#[derive(Serialize, Deserialize)]
pub struct FunctionVersion {
    pub return_type: Vec<ReturnTypeMember>,
    pub parameters: Vec<Parameter>,
    pub flags: Vec<String>,
}

#[derive(Serialize, Deserialize)]
pub struct Function {
    pub name: String,
    pub versions: Vec<FunctionVersion>,
}

#[derive(Serialize, Deserialize)]
pub struct Type {
    pub name: String,
    pub generate_struct: bool,
    pub depends_on: Vec<String>,
    pub functions: Vec<Function>,
}
