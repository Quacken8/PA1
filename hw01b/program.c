#include <stdio.h>
#include <math.h>

const double relative_epsilon = 2.0 / 1000;
const char *bad_input = "Nespravny vstup.\n";

int are_approx_equal(double x, double y)
{
  return fabs(x - y) < relative_epsilon * (fabs(x) + fabs(y));
}

struct Point
{
  double x;
  double y;
};

// using cross multiplication
int are_in_line(struct Point a, struct Point b, struct Point c)
{
  struct Point v1 = {a.x - b.x, a.y - b.y};
  struct Point v2 = {b.x - c.x, b.y - c.y};
  double v1_x_v2 = v1.x * v2.y - v1.y * v2.x;

  return are_approx_equal(v1_x_v2, 0);
}

struct ReadResult
{
  int success;
  struct Point point;
};

struct ReadResult
read_point()
{
  double x, y;
  char closing_bracket;
  int success = 1;
  // NOLINTNEXTLINE
  int read_vars = scanf(" [%lf, %lf%c ", &x, &y, &closing_bracket);

  if (read_vars != 3 || closing_bracket != ']')
    success = 0;

  struct ReadResult res = {
      success,
      {x, y}};
  return res;
}

int main()
{

  printf("Bod A:\n");
  struct ReadResult res = read_point();
  if (!res.success)
  {
    printf("%s", bad_input);
    return 0;
  }
  struct Point a = res.point;

  printf("Bod B:\n");
  res = read_point();
  if (!res.success)
  {
    printf("%s", bad_input);
    return 0;
  }
  struct Point b = res.point;

  printf("Bod C:\n");
  res = read_point();
  if (!res.success)
  {
    printf("%s", bad_input);
    return 0;
  }
  struct Point c = res.point;

  if (are_in_line(a, b, c))
  {
    printf("Rovnobezniky nelze sestrojit.\n");
    return 0;
  }

  return 0;
}