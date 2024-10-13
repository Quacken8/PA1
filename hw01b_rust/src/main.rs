use regex::Regex;
use std::io::{self, Read};

struct Point {
    x: f64,
    y: f64,
}

fn read_point() -> Result<Point, ()> {
    let mut input = String::new();

    io::stdin().read_to_string(&mut input).map_err(|_| ())?;

    let input: String = input
        .split_whitespace()
        .fold(String::new(), |mut prev, curr| {
            prev.push_str(curr);
            prev
        });

    let re = Regex::new(r"^\[(\-?\d+\.?\d*),(\-?\d+\.?\d*)\]$").unwrap();

    if let Some(caps) = re.captures(&input) {
        let x: f64 = caps[1].parse().map_err(|_| ())?;
        let y: f64 = caps[2].parse().map_err(|_| ())?;
        Ok(Point { x, y })
    } else {
        Err(())
    }
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

    let are_same_length = (side_a - side_b).abs() <= 100.0 * (side_a + side_b) * f64::EPSILON;
    let makes_right_sided = side_a.hypot(side_b) <= 100.0 * (side_a + side_b) * f64::EPSILON;

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

fn main() {
    println!("A:");
    let a = match read_point() {
        Ok(p) => p,
        Err(_) => {
            println!("Nespravny vstup.");
            return;
        }
    };
    println!("B:");
    let b = match read_point() {
        Ok(p) => p,
        Err(_) => {
            println!("Nespravny vstup.");
            return;
        }
    };
    println!("C:");
    let c = match read_point() {
        Ok(p) => p,
        Err(_) => {
            println!("Nespravny vstup.");
            return;
        }
    };

    if is_det_zero(&a, &b, &c) {
        println!("Rovnobezniky nelze sestrojit.");
        return;
    }

    let a_prime = find_fourth(&c, &a, &b);
    println!(
        "A': [{},{}], {}",
        a_prime.x,
        a_prime.y,
        categorize_shape(&c, &a, &b)
    );

    let b_prime = find_fourth(&a, &b, &c);
    println!(
        "B': [{},{}], {}",
        b_prime.x,
        b_prime.y,
        categorize_shape(&a, &b, &c)
    );

    let c_prime = find_fourth(&b, &c, &a);
    println!(
        "C': [{},{}], {}",
        c_prime.x,
        c_prime.y,
        categorize_shape(&b, &c, &a)
    );
}
