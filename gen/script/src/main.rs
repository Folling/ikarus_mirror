#![feature(iter_intersperse)]

mod types;

use glob::glob;
use std::env;
use std::fs::File;
use std::io::Write;
use std::path::PathBuf;

fn pascalise(str: &String) -> String {
    str.split("_").map(|str|
        // thanks rust for not allowing me to format iterators
        format!("{}{}", str.chars().next().map(|c| c.to_uppercase().to_string()).unwrap_or(String::new()), str.chars().skip(1).collect::<String>())
    ).collect::<String>()
}

fn write_commented<S: AsRef<str>>(file: &mut File, content: S) -> std::io::Result<()> {
    if content.as_ref().chars().all(|c| c.is_whitespace()) {
        return Ok(());
    }

    const MAX_LENGTH: usize = 60;
    let mut curr_length = 0;

    write!(file, "//")?;

    for line in content.as_ref().lines() {
        if line.chars().all(|c| c.is_whitespace()) {
            write!(file, "\n//\n//")?;
            curr_length = 0;
            continue;
        }

        // not perfect, since unicode word boundaries aren't just whitespace, but good enough for us
        for word in line.trim().split_whitespace() {
            if curr_length + word.len() > MAX_LENGTH {
                write!(file, "\n//")?;

                curr_length = 0;
            }

            write!(file, " {}", word)?;
            curr_length += word.len() + 1;
        }
    }

    Ok(())
}

fn main() {
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

    let output = PathBuf::from(output);

    let pattern = format!("{input}/**/*.yaml");
    for path in glob(&pattern).expect("unable to glob path") {
        let path = path.expect("unable to obtain path from glob");

        println!("scanning {}", path.display());

        let content = std::fs::read_to_string(&path).expect("unable to read file content");

        let r#type: types::Type = serde_yaml::from_str(content.as_str())
            .unwrap_or_else(|e| panic!("unable to parse content to Type: {}", e));

        let type_name = &r#type.name;
        let type_name_pascal = pascalise(type_name);

        let output = output.join(
            path.with_extension("h")
                .strip_prefix(input)
                .expect("unable to strip input prefix from file"),
        );

        let mut file = File::create(output).expect("unable to open file for writing");

        writeln!(file, "#pragma once\n").expect("unable to write to file");

        for header in ["cstdbool", "cstddef", "cstdint"] {
            writeln!(file, "#include <{header}>").expect("unable to write to file");
        }

        writeln!(file, "").expect("unable to write to file");

        for dependency in r#type.depends_on {
            writeln!(file, "#include \"{dependency}.h\"").expect("unable to write to file");
        }

        write!(file, "\n").expect("unable to write to file");

        write_commented(&mut file, r#type.description).expect("unable to write to file");

        write!(file, "\n").expect("unable to write to file");

        if r#type.generate_struct {
            writeln!(file, "struct {type_name_pascal};\n").expect("unable to write to file");
        }

        for func in r#type.functions {
            let func_name = &func.name;
            let func_name_pascal = pascalise(func_name);

            for (version, func_version) in func.versions.into_iter().enumerate() {
                let enum_name =
                    format!("Ikarus{type_name_pascal}{func_name_pascal}V{version}Flags");
                let return_type_name =
                    format!("Ikarus{type_name_pascal}{func_name_pascal}V{version}Result");

                let args = func_version
                    .parameters
                    .into_iter()
                    .map(|param| format!("{} {}", param.r#type, param.name))
                    .collect::<Vec<String>>()
                    .join(", ");

                let enum_values = func_version
                    .flags
                    .into_iter()
                    .enumerate()
                    .map(|(i, value)| {
                        format!("\n    {enum_name}_{} = 1 << {i}", pascalise(&value.name))
                    })
                    .collect::<Vec<String>>()
                    .join(",");

                let return_type_members = func_version
                    .return_type
                    .into_iter()
                    .map(|member| format!("\n    {} {};", member.r#type, member.name))
                    .collect::<Vec<String>>()
                    .join("");

                writeln!(
                    file,
                    "\nenum {enum_name} {{\n    {enum_name}_None = 0,{enum_values}\n}};"
                )
                .expect("unable to write to file");

                writeln!(file, "\nstruct {return_type_name} {{{return_type_members}\n    StatusCode status_code;\n}};").expect("unable to write to file");

                writeln!(file, "\n{return_type_name} ikarus_{type_name}_{func_name}_v{version}({args}, {enum_name} flags);").expect("unable to write to file");
            }
        }
    }
}
