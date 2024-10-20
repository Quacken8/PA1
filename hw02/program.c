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
  else if (read_vals != 4 || closing_bracket != '>' || upper < lower || lower < 0 || upper < 0 || !(opening_symbol == '#' || opening_symbol == '?'))
  {
    // 77
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

int count_triples(int lower, int upper, int supress_readout)
{
  int count = 0;

  for (int r = 2; r < upper; r += 2)
  {
    for (int s = 1; s <= r / sqrt(2); s++)
    {
      if ((r * r / 2) % s != 0)
        continue;
      int t = r * r / 2 / s;
      int a = r + s;
      int b = r + t;
      int c = r + s + t;
      // c > b > a
      if (c > upper || a < lower)
        continue;

      if (!supress_readout)
        printf("* %d %d %d\n", a, b, c);
      count++;
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