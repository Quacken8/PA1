#include <stdio.h>

const char *bad_input = "Nespravny vstup.\n";
const double relative_eps = 1.0 / 1000;

unsigned int approximately_equal(double x1, double x2)
{
  double larger = x1 > x2 ? x1 : x2;
  double smaller = x1 + x2 - larger;
  double error = larger * relative_eps;

  if (larger - error < smaller)
    return 1;

  return 0;
}

int main()
{

  double a1, b1;
  double a2, b2;

  printf("Obdelnik #1:\n");
  // NOLINTNEXTLINE
  short loaded = scanf(" %lf %lf", &a1, &b1);
  if (loaded != 2 || a1 <= 0 || b1 <= 0)
  {
    printf("%s", bad_input);
    return 0;
  }

  printf("Obdelnik #2:\n");
  // NOLINTNEXTLINE
  loaded = scanf(" %lf %lf", &a2, &b2);
  if (loaded != 2 || a2 <= 0 || b2 <= 0)
  {
    printf("%s", bad_input);
    return 0;
  }

  double s1 = a1 * b1;
  double s2 = a2 * b2;

  if (approximately_equal(s1, s2))
  {
    printf("Obdelniky maji stejny obsah.\n");
  }
  else if (s1 > s2)
  {
    printf("Obdelnik #2 ma mensi obsah.\n");
  }
  else
  {
    printf("Obdelnik #1 ma mensi obsah.\n");
  }

  return 0;
}