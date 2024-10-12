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

int are_in_line(struct Point p1, struct Point p2, struct Point p3)
{
  double side_a = hypot(p1.x - p2.x, p1.y - p2.y);
  double side_b = hypot(p2.x - p3.x, p2.y - p3.y);
  double hypo = hypot(p3.x - p1.x, p3.y - p1.y);

  double largest = side_a > side_b ? side_a : side_b;
  largest = largest > hypo ? largest : hypo;
  double smallest = side_a < side_b ? side_a : side_b;
  smallest = smallest < hypo ? largest : hypo;
  double mid = side_a + side_b + hypo - largest - smallest;

  return fabs(smallest + mid - largest) < largest * DBL_EPSILON;
}

int make_right_triangle(struct Point p1, struct Point p2, struct Point p3)
{
  double side_a = hypot(p1.x - p2.x, p1.y - p2.y);
  double side_b = hypot(p2.x - p3.x, p2.y - p3.y);
  double hypo = hypot(p3.x - p1.x, p3.y - p1.y);
  double larger = hypot(side_a, side_b) > hypo ? hypot(side_a, side_b) : hypo;

  return fabs(hypot(side_a, side_b) - hypo) < larger * 100 * DBL_EPSILON;
}

int are_equidistant(struct Point p1, struct Point p2, struct Point p3)
{
  double side_a = hypot(p1.x - p2.x, p1.y - p2.y);
  double side_b = hypot(p2.x - p3.x, p2.y - p3.y);
  double largest = side_a > side_b ? side_a : side_b;

  return fabs(side_a - side_b) < largest * 100 * DBL_EPSILON;
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