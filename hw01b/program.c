#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <math.h>

const char *bad_input = "Nespravny vstup.\n";

struct Point
{
  double x;
  double y;
};

int is_any_invalid(struct Point p1, struct Point p2, struct Point p3)
{
  return isnan(p1.x) || isinf(p1.x) || isnan(p1.y) || isinf(p1.y) || isnan(p2.x) || isinf(p2.x) || isnan(p2.y) || isinf(p2.y) || isnan(p3.x) || isinf(p3.x) || isnan(p3.y) || isinf(p3.y);
}

int is_det_zero(struct Point p1, struct Point p2, struct Point p3)
{
  struct Point v1 = {p1.x - p2.x, p1.y - p2.y};
  struct Point v2 = {p2.x - p3.x, p2.y - p3.y};
  double xy = v1.x * v2.y;
  double yx = v1.y * v2.x;
  double largest = fabs(xy) > fabs(yx) ? fabs(xy) : fabs(yx);
  return fabs(xy - yx) <= largest * 100 * DBL_EPSILON;
}

int make_right_triangle(struct Point p1, struct Point p2, struct Point p3)
{
  double side_a = hypot(p1.x - p2.x, p1.y - p2.y);
  double side_b = hypot(p2.x - p3.x, p2.y - p3.y);
  double hypo = hypot(p3.x - p1.x, p3.y - p1.y);
  double larger = hypot(side_a, side_b) > hypo ? hypot(side_a, side_b) : hypo;

  return fabs(hypot(side_a, side_b) - hypo) <= larger * 100 * DBL_EPSILON;
}

int are_equidistant(struct Point p1, struct Point p2, struct Point p3)
{
  double side_a = hypot(p1.x - p2.x, p1.y - p2.y);
  double side_b = hypot(p2.x - p3.x, p2.y - p3.y);
  double largest = side_a > side_b ? side_a : side_b;

  return fabs(side_a - side_b) <= largest * 100 * DBL_EPSILON;
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

  int right_angle = make_right_triangle(a, b, c);
  int same_length = are_equidistant(a, b, c);

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

  struct Point a_prime = find_fourth(c, a, b);
  struct Point b_prime = find_fourth(a, b, c);
  struct Point c_prime = find_fourth(b, c, a);

  if (is_det_zero(a, b, c) || is_any_invalid(a, b, c) || is_any_invalid(a_prime, b_prime, c_prime))
  {
    printf("Rovnobezniky nelze sestrojit.\n");
    return 0;
  }

  printf("A': [%.8g,%.8g], %s\n", a_prime.x, a_prime.y, categorize_shape(c, a, b));

  printf("B': [%.8g,%.8g], %s\n", b_prime.x, b_prime.y, categorize_shape(a, b, c));

  printf("C': [%.8g,%.8g], %s\n", c_prime.x, c_prime.y, categorize_shape(b, c, a));

  return 0;
}