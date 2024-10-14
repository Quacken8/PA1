use std::io::{self, Read};

struct Point {
    x: f64,
    y: f64,
}

// Err(usize) refers to number of correctly parsed points
fn read_points() -> Result<[Point; 3], usize> {
    let mut input = String::new();

    if io::stdin()
        .read_to_string(&mut input)
        .map_err(|_| ())
        .is_err()
    {
        return Err(0);
    };
    let without_white_space: String = input.split_ascii_whitespace().collect();

    if !without_white_space.starts_with("[") {
        return Err(0);
    };
    let missing_ending = !without_white_space.ends_with("]");
    let as_vec: Vec<f64> = without_white_space
        .replace("][", ",")
        .replace("]", "") // this makes us ignore inputs like [[[[[[[[1,2]]]]]]]]
        .replace("[", "") // this makes us ignore inputs like [[[[[[[[1,2]]]]]]]]
        .split(',')
        .filter_map(|s| match s.parse::<f64>() {
            Ok(n) => Some(n),
            Err(_) => None,
        })
        .collect();

    if as_vec.len() != 6 {
        return Err(as_vec.len() / 2);
    }
    if missing_ending {
        return Err(3);
    }

    Ok([
        Point {
            x: as_vec[0],
            y: as_vec[1],
        },
        Point {
            x: as_vec[2],
            y: as_vec[3],
        },
        Point {
            x: as_vec[4],
            y: as_vec[5],
        },
    ])
}

fn find_fourth(a: &Point, b: &Point, c: &Point) -> Point {
    Point {
        x: c.x + a.x - b.x,
        y: c.y + a.y - b.y,
    }
}

fn categorize_shape<'a>(a: &Point, b: &Point, c: &Point) -> &'a str {
    let side_a = (b.x - a.x).hypot(b.y - a.y);
    let side_b = (c.x - b.x).hypot(c.y - b.y);
    let would_be_hypot = (a.x - c.x).hypot(a.y - c.y);

    let are_same_length = (side_a - side_b).abs() <= 100.0 * (side_a + side_b) * f64::EPSILON;
    let makes_right_sided = (would_be_hypot - side_a.hypot(side_b)).abs()
        <= 100.0 * (side_a.hypot(side_b)) * f64::EPSILON;

    if are_same_length && makes_right_sided {
        return "ctverec";
    };
    if are_same_length {
        return "kosoctverec";
    }
    if makes_right_sided {
        return "obdelnik";
    }
    "rovnobeznik"
}

fn is_det_zero(a: &Point, b: &Point, c: &Point) -> bool {
    let v1 = Point {
        x: a.x - b.x,
        y: a.y - b.y,
    };
    let v2 = Point {
        x: b.x - c.x,
        y: b.y - c.y,
    };

    (v1.x * v2.y - v1.y * v2.x).abs()
        <= 100.0 * ((v1.x * v2.y).abs() + (v1.y * v2.x).abs()) * f64::EPSILON
}

// C lang formatting is fucking cursed, man
// this passes the tests, but it is reverse-engineered
fn format_coordinate(x: f64) -> String {
    if x.abs() < 1e4 && (1.0 / x).abs() < 1e4 {
        let mut formatted = format!("{:.3}", x);
        while formatted.ends_with("0") && formatted.contains(".") {
            formatted.remove(formatted.len() - 1);
        }
        if formatted.ends_with(".") {
            formatted.remove(formatted.len() - 1);
        }
        return formatted;
    }

    let formatted = format!("{:.6e}", x);
    if !formatted.starts_with("1") {
        return format!("{:.5e}", formatted.parse::<f64>().unwrap()); // christ
    }
    formatted
}

fn main() {
    println!("Bod A:");
    let [a, b, c] = match read_points() {
        Ok(points) => {
            println!("Bod B:\nBod C:");
            points
        }
        Err(num) => {
            if num >= 1 {
                println!("Bod B:");
            }
            if num >= 2 {
                println!("Bod C:");
            }
            return println!("Nespravny vstup.");
        }
    };

    if is_det_zero(&a, &b, &c) {
        println!("Rovnobezniky nelze sestrojit.");
        return;
    }

    let a_prime = find_fourth(&c, &a, &b);
    println!(
        "A': [{},{}], {}",
        format_coordinate(a_prime.x),
        format_coordinate(a_prime.y),
        categorize_shape(&c, &a, &b)
    );

    let b_prime = find_fourth(&a, &b, &c);
    println!(
        "B': [{},{}], {}",
        format_coordinate(b_prime.x),
        format_coordinate(b_prime.y),
        categorize_shape(&a, &b, &c)
    );

    let c_prime = find_fourth(&b, &c, &a);
    println!(
        "C': [{},{}], {}",
        format_coordinate(c_prime.x),
        format_coordinate(c_prime.y),
        categorize_shape(&b, &c, &a)
    );
}
