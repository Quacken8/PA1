use std::io;

fn main() {
    let quotes = [
        "noH QapmeH wo' Qaw'lu'chugh yay chavbe'lu' 'ej wo' choqmeH may' DoHlu'chugh lujbe'lu'.",
        "bortaS bIr jablu'DI' reH QaQqu' nay'.",
        "Qu' buSHa'chugh SuvwI', batlhHa' vangchugh, qoj matlhHa'chugh, pagh ghaH SuvwI''e'.",
        "bISeH'eghlaH'be'chugh latlh Dara'laH'be'.",
        "qaStaHvIS wa' ram loS SaD Hugh SIjlaH qetbogh loD.",
        "Suvlu'taHvIS yapbe' HoS neH.",
        "Ha'DIbaH DaSop 'e' DaHechbe'chugh yIHoHQo'.",
        "Heghlu'meH QaQ jajvam.",
        "leghlaHchu'be'chugh mIn lo'laHbe' taj jej.",
    ];

    let query = "ml' nob:";
    let good_input = "Qapla'";
    let out_of_bounds = "Qih mi' ";
    let wrong_input = "Neh mi'";
    // let unexpected_appendix = "bIjatlh 'e' yImev";

    println!("{}", query);

    let mut input = String::new();
    if io::stdin().read_line(&mut input).is_err() {
        println!("{}", wrong_input);
        return;
    }

    let index = input.trim().parse::<i32>();
    if index.is_err() {
        println!("{}", wrong_input);
        return;
    }

    let index = index.unwrap();

    if !(0..=8).contains(&index) {
        println!("{}{}", out_of_bounds, index);
        return;
    }

    println!("{}\n{}", good_input, quotes[index as usize]);
}
