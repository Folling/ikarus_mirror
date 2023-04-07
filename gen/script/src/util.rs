use std::fs::File;
use std::io::Write;

// pattern isn't stable yet
pub fn split_maybe_once(string: &str, delimiter: char) -> (&str, Option<&str>) {
    if let Some(idx) = string.find(delimiter) {
        unsafe {
            (
                string.get_unchecked(..idx),
                Some(string.get_unchecked(idx..)),
            )
        }
    } else {
        (string, None)
    }
}

pub fn make_pascal_case(str: &String) -> String {
    str.split("_").map(|str|
        // thanks rust for not allowing me to format iterators
        format!("{}{}", str.chars().next().map(|c| c.to_uppercase().to_string()).unwrap_or(String::new()), str.chars().skip(1).collect::<String>())
    ).collect::<String>()
}

pub fn write_commented<S: AsRef<str>>(file: &mut File, content: S) -> std::io::Result<()> {
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
