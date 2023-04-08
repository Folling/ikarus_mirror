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

pub fn write_commented<W: Write, S: AsRef<str>>(
    writer: &mut W,
    content: S,
    indent: usize,
) -> std::io::Result<()> {
    if content.as_ref().chars().all(|c| c.is_whitespace()) {
        return Ok(());
    }

    const MAX_LENGTH: usize = 80;
    let mut curr_length = 0;

    write!(writer, "{}//", " ".repeat(indent))?;

    for line in content.as_ref().lines() {
        if line.chars().all(|c| c.is_whitespace()) {
            write!(writer, "\n{}//\n//", " ".repeat(indent))?;
            curr_length = 0;
            continue;
        }

        // not perfect, since unicode word boundaries aren't just whitespace, but good enough for us
        for word in line.trim().split_whitespace() {
            if curr_length + word.len() > MAX_LENGTH {
                write!(writer, "\n{}//", " ".repeat(indent))?;

                curr_length = 0;
            }

            write!(writer, " {}", word)?;
            curr_length += word.len() + 1;
        }
    }

    writeln!(writer, "")?;

    Ok(())
}
