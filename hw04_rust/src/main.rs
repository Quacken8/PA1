use std::io;

#[derive(Clone)]
struct Vehicle {
    availible_from: i128,
    availible_to: i128,
    cost: u64,
    capacity: u64,
}

static MAX_CAPACITY: usize = 100000;

fn read_vehicles(vehicles: &mut Vec<Vehicle>) -> Result<(), ()> {
    let mut line = String::new();
    let mut first_line = true;
    let mut last_line = false;
    loop {
        match io::stdin().read_line(&mut line) {
            Ok(0) => break,
            Err(_) => return Err(()),
            _ => {
                //noop
            }
        };
        line = line.split_ascii_whitespace().collect();
        if first_line {
            if !line.starts_with('{') {
                return Err(());
            }
            line.remove(0);
            first_line = false;
        }

        if line.ends_with('}') {
            if last_line {
                return Err(());
            }
            line.remove(line.len() - 1);
            last_line = true;
        }

        if line.remove(0) != '[' || line.remove(line.len() - 1) != ']' {
            return Err(());
        }
        for s in line.split("],[") {
            let v = parse_a_single_vehicle(s)?;
            if vehicles.len() == MAX_CAPACITY {
                return Err(());
            }
            vehicles.push(v);
        }
    }

    if line.ends_with('}') {
        return Ok(());
    }
    Err(())
}

fn parse_a_single_vehicle(s: &str) -> Result<Vehicle, ()> {
    let mut three_parts = s.split(',');
    let mut availibility_vec = three_parts
        .next()
        .ok_or(())?
        .split("-")
        .map(|s| s.parse().ok());

    let [Some(availible_from), Some(avalible_to), None] =
        std::array::from_fn(|_| availibility_vec.next().flatten())
    else {
        return Err(());
    };

    let mut three_parts = three_parts.map(|s| s.parse().ok());
    let [Some(cost), Some(capacity), None] = std::array::from_fn(|_| three_parts.next().flatten())
    else {
        return Err(());
    };

    Ok(Vehicle {
        availible_from,
        availible_to: avalible_to,
        capacity,
        cost,
    })
}

struct Problem {
    starting_day: u64,
    pieces: u64,
}

struct Solution {
    end_day: u64,
    total_cost: u64,
}

enum ProblemReadErr {
    Eof,
    SomethingStinky,
}

fn read_problem() -> Result<Problem, ProblemReadErr> {
    let mut line = String::new();
    match io::stdin().read_line(&mut line) {
        Ok(0) => return Err(ProblemReadErr::Eof),
        Err(_) => return Err(ProblemReadErr::SomethingStinky),
        _ => {
            // noop
        }
    };

    let mut iter = line.split_ascii_whitespace().map(|s| s.parse::<u64>().ok());

    let [Some(starting_day), Some(pieces), None] = std::array::from_fn(|_| iter.next().flatten())
    else {
        return Err(ProblemReadErr::SomethingStinky);
    };

    Ok(Problem {
        starting_day,
        pieces,
    })
}

fn solve_problem(problem: Problem) -> Solution {
    todo!();
}

#[derive(Clone, Copy)]
struct CumulativeWork {
    day: i128, // a little dumb to have the whole range halved just for that one case, but the code may be cleaner?
    pieces: u128,
    cost: u128,
}

/// Sets up a piecewise constant function that represents total cost and
/// carrying capacity of all vehicles availible on and after given day
/// up until the next day in the piecewise vec
///
///     p[1]  p[2]     p[3]    p[4] p[5]
///    |
///  c |       ×--------o
///  o |                ×-------o
///  s | ×-----o
///  t |                        ×----o
///    +-----------------------------·-----
///                  day
fn create_piecewise(vehicles: &mut [Vehicle]) -> Vec<CumulativeWork> {
    let mut piecewise = Vec::with_capacity(vehicles.len() * 2 + 1);
    let mut todays_work = CumulativeWork {
        day: -1,
        pieces: 0,
        cost: 0,
    };
    piecewise[0] = todays_work;
    let mut vehicles_clone = vehicles.to_owned();
    vehicles_clone.sort_by_key(|v| v.availible_from);
    let mut by_from = vehicles_clone.iter();
    vehicles.sort_by_key(|v| v.availible_to);
    let mut by_to = vehicles.iter();

    let mut to = by_to.next();
    let mut from = by_from.next();
    while to.is_some() || from.is_some() {
        match (from, to) {
            (Some(f), None) => {
                todays_work.day = f.availible_from; // when the start of availibility is hit, capacity/cost increases
                todays_work.cost += f.cost as u128;
                todays_work.pieces += f.capacity as u128;
                for f in by_from.by_ref() {
                    // drain out all "froms" with the same from day
                    from = Some(f);
                    if f.availible_from != todays_work.day {
                        break;
                    }
                    todays_work.cost += f.cost as u128;
                    todays_work.pieces += f.capacity as u128;
                }
            }
            (None, Some(t)) => {
                todays_work.day = t.availible_to + 1; // one day after the end of availibility is hit, capacity/cost decreasees
                todays_work.cost -= t.cost as u128;
                todays_work.pieces -= t.capacity as u128;
                for t in by_to.by_ref() {
                    to = Some(t);
                    if t.availible_to + 1 != todays_work.day {
                        break;
                    }
                    todays_work.cost -= t.cost as u128;
                    todays_work.pieces -= t.capacity as u128;
                }
            }
            (Some(f), Some(t)) => {
                todays_work.day = f.availible_from.min(t.availible_to + 1);

                if f.availible_from == todays_work.day {
                    todays_work.cost += f.cost as u128;
                    todays_work.pieces += f.capacity as u128;
                    for f in by_from.by_ref() {
                        // drain out all "froms" with the same from day
                        from = Some(f);
                        if f.availible_from != todays_work.day {
                            break;
                        }
                        todays_work.cost += f.cost as u128;
                        todays_work.pieces += f.capacity as u128;
                    }
                }

                if t.availible_to + 1 == todays_work.day {
                    todays_work.cost -= t.cost as u128;
                    todays_work.pieces -= t.capacity as u128;
                    for t in by_to.by_ref() {
                        to = Some(t);
                        if t.availible_to + 1 != todays_work.day {
                            break;
                        }
                        todays_work.cost -= t.cost as u128;
                        todays_work.pieces -= t.capacity as u128;
                    }
                }
            }
            _ => {
                panic!("This should never happen");
            }
        }
        piecewise.push(todays_work);
    }

    piecewise
}

fn integrate_piecewise(piecewise: &Vec<CumulativeWork>) -> Vec<CumulativeWork> {
    todo!()
}

fn main() {
    println!("Moznosti dopravy:\n");
    let mut vehicles = Vec::<Vehicle>::with_capacity(MAX_CAPACITY);
    if read_vehicles(&mut vehicles).is_err() {
        println!("Nespravny vstup.\n");
        return;
    }
    println!("Naklad:\n");
    loop {
        let problem = match read_problem() {
            Ok(p) => p,
            Err(ProblemReadErr::Eof) => return,
            _ => {
                println!("Nespravny vstup.\n");
                return;
            }
        };

        let solution = solve_problem(problem);
        println!("Konec: {}, cena: {}", solution.end_day, solution.total_cost);
    }
}
