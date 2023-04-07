#![feature(iter_intersperse)]

mod types;
mod util;

use crate::types::Type;
use glob::glob;
use std::env;
use std::error::Error;
use std::fs::{DirBuilder, File};
use std::path::PathBuf;

fn main() -> Result<(), Box<dyn Error>> {
    let args: Vec<_> = env::args().collect();
    let args_str: Vec<_> = args.iter().map(String::as_str).collect();

    let (input, output) = match &args_str[..] {
        [_, "--input", input, "--output", output] => (input, output),
        _ => panic!("must pass --input and --output"),
    };

    if input.trim().len() == 0 {
        panic!("input must not be empty");
    }

    if output.trim().len() == 0 {
        panic!("output must not be empty");
    }

    println!("input: {}\noutput:{}", input, output);

    let output = PathBuf::from(output);

    let pattern = format!("{input}/**/*.yaml");
    for path in glob(&pattern).expect("unable to glob path") {
        let path = path.expect("unable to obtain path from glob");

        println!("scanning {}", path.display());

        let output = output.join(
            path.with_extension("h")
                .strip_prefix(input)
                .expect("unable to strip input prefix from file"),
        );

        println!("writing to {}", output.display());

        let content = std::fs::read_to_string(&path).expect("unable to read file content");

        let r#type: Type = serde_yaml::from_str(content.as_str())
            .unwrap_or_else(|e| panic!("unable to parse content to Type: {}", e));

        DirBuilder::new()
            .recursive(true)
            .create(&output.parent().expect("output file didn't have a parent"))
            .expect("unable to create parent directories for output file");

        let mut file = File::create(output).expect("unable to open file for writing");

        r#type.generate(&mut file)?;
    }

    Ok(())
}
