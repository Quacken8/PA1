#include <stdio.h>
#include <math.h>

struct ReadResult
{
  int success;
  int eof;
  int supress_readout;
  int lower;
  int upper;
};

struct ReadResult readInput()
{

  char closing_bracket, opening_symbol;
  int lower, upper;
  int success = 1;
  int eof = 0;
  // NOLINTNEXTLINE
  int read_vals = scanf(" %c < %d ; %d %c", &opening_symbol, &lower, &upper, &closing_bracket);
  if (read_vals == EOF)
  {
    eof = 1;
  }
  else if (read_vals != 4 || closing_bracket != '>' || upper < lower || lower <= 0 || upper <= 0 || !(opening_symbol == '#' || opening_symbol == '?'))
  {
    success = 0;
  }

  struct ReadResult res =
      {
          success,
          eof,
          opening_symbol == '#',
          lower,
          upper};

  return res;
}

int gcd(int m, int n)
{
  int larger = m > n ? m : n;
  int smaller = m < n ? m : n;
  if (smaller == 0)
    return larger;
  return gcd(larger % smaller, smaller);
}

int count_triples(int lower, int upper, int supress_readout)
{
  int count = 0;

  for (int m = 2; m < sqrt(upper); m++)
  {
    for (int n = 1 + (m % 2); n < m; n += 1 + (m % 2))
    {
      if (gcd(m, n) != 1)
        continue;
      int a_kless = m * m - n * n;
      int b_kless = 2 * m * n;
      int c_kless = m * m + n * n;

      int start_k = lower / (a_kless > b_kless ? b_kless : a_kless);
      int end_k = upper / c_kless + 1;
      for (int k = start_k; k <= end_k; k++)
      {
        int a = k * a_kless;
        int b = k * b_kless;
        int c = k * c_kless;
        if (c > upper)
          break;
        if (a < lower || b < lower)
          continue;
        count++;
        if (!supress_readout)
          printf("* %d %d %d\n", a, b, c);
      }
    }
  }

  return count;
}

int main()
{
  printf("Problemy:\n");
  while (1)
  {
    struct ReadResult res = readInput();
    if (!res.success)
    {
      printf("Nespravny vstup.\n");
      break;
    }
    if (res.eof)
    {
      break;
    }

    int count = count_triples(res.lower, res.upper, res.supress_readout);
    printf("Celkem: %d\n", count);
  }

  return 0;
}