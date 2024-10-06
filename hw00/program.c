#include <stdio.h>

const char *prompt = "ml' nob:";
const char *read_error = "Neh mi'";
const char *out_of_bounds_error = "Qih mi'";
const char *output = "Qapla'";
const char *unexpected_appendix_error = "bIjatlh 'e' yImev";

const char *
    quotes[] = {
        "noH QapmeH wo' Qaw'lu'chugh yay chavbe'lu' 'ej wo' choqmeH may' DoHlu'chugh lujbe'lu'.",
        "bortaS bIr jablu'DI' reH QaQqu' nay'.",
        "Qu' buSHa'chugh SuvwI', batlhHa' vangchugh, qoj matlhHa'chugh, pagh ghaH SuvwI''e'.",
        "bISeH'eghlaH'be'chugh latlh Dara'laH'be'.",
        "qaStaHvIS wa' ram loS SaD Hugh SIjlaH qetbogh loD.",
        "Suvlu'taHvIS yapbe' HoS neH.",
        "Ha'DIbaH DaSop 'e' DaHechbe'chugh yIHoHQo'.",
        "Heghlu'meH QaQ jajvam.",
        "leghlaHchu'be'chugh mIn lo'laHbe' taj jej."};

int main()
{
  printf("%s\n", prompt);
  short i;
  char unexpected_char;
  // NOLINTNEXTLINE
  short read = scanf("%hd %c", &i, &unexpected_char);
  if (read <= 0)
  {
    printf("%s\n", read_error);
    return 1;
  }
  if (read == 2)
  {
    printf("%s\n", unexpected_appendix_error);
    return 1;
  }

  if (i > 8 || i < 0)
  {
    printf("%s %hd\n", out_of_bounds_error, i);
    return 1;
  }

  printf("%s\n%s\n", output, quotes[i]);
  return 0;
}