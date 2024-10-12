#include <stdio.h>
#include <math.h>

const double relative_epsilon = 1e-3;
const double absolute_epsilon = 1e-6;
const char *bad_input = "Nespravny vstup.\n";

int are_approx_equal(double x, double y)
{
  if (fabs(x) < absolute_epsilon && fabs(y) < absolute_epsilon)
    return 1;
  return fabs(x - y) <= relative_epsilon * (fabs(x) + fabs(y));
}

struct Point
{
  double x;
  double y;
};

int are_in_line(struct Point a, struct Point b, struct Point c)
{
  struct Point v1 = {a.x - b.x, a.y - b.y};
  struct Point v2 = {b.x - c.x, b.y - c.y};
  double angle = fabs(atan2(v2.y, v2.x) - atan2(v1.y, v1.x));
  return are_approx_equal(angle, 0) || are_approx_equal(angle, 4.0 * atan(1));
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
  int read_vars = scanf(" [ %lf , %lf %c ", &x, &y, &closing_bracket);

  if (read_vars != 3 || closing_bracket != ']')
    success = 0;
  struct ReadResult res = {
      success,
      {x, y}};

  return res;
}

// It is expected the corners are all adjacent
const char *categorize_shape(struct Point a, struct Point b, struct Point c)
{
  struct Point side_a = {a.x - b.x, a.y - b.y};
  struct Point side_b = {b.x - c.x, b.y - c.y};
  double angle = fabs(atan2(side_a.y, side_a.x) - atan2(side_b.y, side_b.x));
  int right_angle = are_approx_equal(angle, 2.0 * atan(1));
  int same_length = are_approx_equal(hypot(side_a.x, side_a.y), hypot(side_b.x, side_b.y));

  if (right_angle && same_length)
    return "ctverec";
  if (right_angle)
    return "obdelnik";
  if (same_length)
    return "kosoctverec";

  return "rovnobeznik";
}

struct Point find_fourth(struct Point a, struct Point b, struct Point c)
{
  struct Point fourth = {
      c.x + a.x - b.x,
      c.y + a.y - b.y};
  return fourth;
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

  struct Point fourth = find_fourth(c, a, b);
  printf("A': [%.8g,%.8g], %s\n", fourth.x, fourth.y, categorize_shape(c, a, b));

  fourth = find_fourth(a, b, c);
  printf("B': [%.8g,%.8g], %s\n", fourth.x, fourth.y, categorize_shape(a, b, c));

  fourth = find_fourth(b, c, a);
  printf("C': [%.8g,%.8g], %s\n", fourth.x, fourth.y, categorize_shape(b, c, a));

  return 0;
}