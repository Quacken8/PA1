use std::{
    cmp::{max, min},
    io::{self},
};

struct ReadRes {
    range: Range,
    suppress_readout: bool,
}
struct Range {
    lower: usize,
    upper: usize,
}

enum ReadErrors {
    BadInput,
    Eof,
}

fn read_range() -> Result<ReadRes, ReadErrors> {
    let mut input = String::new();

    match io::stdin().read_line(&mut input) {
        Ok(n) => {
            if n == 0 {
                return Err(ReadErrors::Eof);
            }
        }
        Err(_) => return Err(ReadErrors::BadInput),
    };
    input = input.split_ascii_whitespace().collect();
    let mut chars = input.chars();
    let defining_symbol = match chars.next() {
        Some(s) => s,
        None => return Err(ReadErrors::BadInput),
    };
    let one_string = chars.collect::<String>();
    let range_strings: Vec<&str> = one_string.split(";").collect();
    if range_strings.len() != 2
        || !range_strings[1].ends_with('>')
        || !range_strings[0].starts_with('<')
    {
        return Err(ReadErrors::BadInput);
    }

    let lower: usize = match range_strings[0].replace("<", "").parse() {
        Ok(f) => f,
        Err(_) => return Err(ReadErrors::BadInput),
    };
    let upper: usize = match range_strings[1].replace(">", "").parse() {
        Ok(f) => f,
        Err(_) => return Err(ReadErrors::BadInput),
    };

    let suppress_readout = if defining_symbol == '#' {
        true
    } else if defining_symbol == '?' {
        false
    } else {
        return Err(ReadErrors::BadInput);
    };

    if lower > upper || lower == 0 || upper == 0 {
        return Err(ReadErrors::BadInput);
    }

    Ok(ReadRes {
        range: Range { lower, upper },
        suppress_readout,
    })
}

fn gcd(m: usize, n: usize) -> usize {
    let smaller = min(m, n);
    let larger = max(m, n);
    if smaller == 0 {
        return larger;
    };

    gcd(larger % smaller, smaller)
}

fn count_triples(range: Range, suppress_readout: bool) -> usize {
    let mut count: usize = 0;

    for m in 2..((range.upper as f64).sqrt().ceil() as usize) {
        for n in ((1 + (m % 2))..m).step_by(m % 2 + 1) {
            if gcd(m, n) != 1 {
                continue;
            }

            let a_kless = m * m - n * n;
            let b_kless = 2 * m * n;
            let c_kless = m * m + n * n;

            let k_lower = range.lower / min(a_kless, b_kless);
            let k_upper = range.upper / c_kless + 1;
            for k in k_lower..k_upper {
                let a = a_kless * k;
                let b = b_kless * k;
                let c = c_kless * k;
                if c > range.upper {
                    break;
                }
                if a < range.lower || b < range.lower {
                    continue;
                }
                count += 1;
                if !suppress_readout {
                    println!("* {a} {b} {c}")
                }
            }
        }
    }

    count
}

fn main() {
    println!("Problemy:");
    loop {
        let read_res = match read_range() {
            Ok(r) => r,
            Err(e) => match e {
                ReadErrors::Eof => break,
                ReadErrors::BadInput => {
                    println!("Nespravny vstup.");
                    break;
                }
            },
        };

        println!(
            "Celkem {}",
            count_triples(read_res.range, read_res.suppress_readout)
        );
    }
}
