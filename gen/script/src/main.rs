#![feature(iter_intersperse)]

use anyhow::{anyhow, bail};
use std::env;
use std::fs::{DirBuilder, File};
use std::path::PathBuf;

use glob::glob;

use crate::types::Type;

mod types;
mod util;

fn main() -> anyhow::Result<()> {
    let args: Vec<_> = env::args().collect();
    let args_str: Vec<_> = args.iter().map(String::as_str).collect();

    let (input, include_output, source_output, impl_header_output) = match &args_str[..] {
        [_, "--input", input, "--include-output", include_output, "--source-output", source_output, "--impl-header-output", impl_haeader_output] => {
            (input, include_output, source_output, impl_haeader_output)
        }
        _ => bail!("must pass --input and --output"),
    };

    if input.trim().len() == 0 {
        bail!("input must not be empty");
    }

    if include_output.trim().len() == 0 {
        bail!("include-output must not be empty");
    }

    if source_output.trim().len() == 0 {
        bail!("source-output must not be empty");
    }

    if impl_header_output.trim().len() == 0 {
        bail!("impl-header-output must not be empty");
    }

    println!("input: {}\noutput:{}", input, include_output);

    let include_output = PathBuf::from(include_output);
    let source_output = PathBuf::from(source_output);
    let impl_header_output = PathBuf::from(impl_header_output);

    let pattern = format!("{input}/**/*.yaml");
    for path in glob(&pattern).expect("unable to glob path") {
        let path = path.expect("unable to obtain path from glob");

        println!("scanning {}", path.display());

        let content = std::fs::read_to_string(&path).expect("unable to read file content");

        let r#type: Type = serde_yaml::from_str(content.as_str())
            .unwrap_or_else(|e| panic!("unable to parse content to Type: {}", e));

        let subst_path_include = path.with_extension("h").strip_prefix(input)?.to_path_buf();
        let subst_path_source = path
            .with_extension("cpp")
            .strip_prefix(input)?
            .to_path_buf();
        let subst_path_impl_header = path
            .with_extension("hpp")
            .strip_prefix(input)?
            .to_path_buf();

        let generation_steps: [(
            &PathBuf,
            &PathBuf,
            fn(&Type, &mut File, &PathBuf) -> anyhow::Result<()>,
        ); 3] = [
            (
                &include_output,
                &subst_path_include,
                Type::generate_include_header,
            ),
            (
                &source_output,
                &subst_path_source,
                Type::generate_include_source,
            ),
            (
                &impl_header_output,
                &subst_path_impl_header,
                Type::generate_impl_header,
            ),
        ];

        for (path, sub, func) in &generation_steps {
            let output = path.join(&sub);

            DirBuilder::new().recursive(true).create(
                &output
                    .parent()
                    .ok_or_else(|| anyhow!("unable to fetch parent for output file"))?,
            )?;

            let mut file = File::create(&output)?;

            func(&r#type, &mut file, &sub)?;
        }
    }

    Ok(())
}
