use super::flag::Flag;
use super::parameter::Parameter;
use super::return_type_member::ReturnTypeMember;

use serde_derive::Deserialize;

#[derive(Debug, Deserialize)]
pub struct FunctionVersion {
    pub return_type: Vec<ReturnTypeMember>,
    pub parameters: Vec<Parameter>,
    pub flags: Vec<Flag>,
}
