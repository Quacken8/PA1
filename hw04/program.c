#include <stdio.h>
#include <stdbool.h>

typedef struct VehicleInfo
{
  int availibleFrom;
  int availibleTo;
  int capacity;
  int cost;
} VehicleInfo;

typedef struct VehicleReadRes
{
  VehicleInfo *vehicles;
  bool success;
} VehicleReadRes;

typedef struct Problem
{
  int startingDate;
  int pieces;
} Problem;

typedef struct ProblemReadRes
{
  Problem *problems;
  bool success;
} ProblemReadRes;

VehicleReadRes readVehicleInfo()
{
  VehicleReadRes res;
  res.success = false;
  char openingBrace;
  // NOLINTNEXTLINE
  if (scanf(" %c", &openingBrace) != 1 || openingBrace != '{')
  {
    return res;
  }

  int from, to, capacity, cost;
  char endingChar;
  for (int i = 1; i <= 100000 + 1; i++)
  {
    // NOLINTNEXTLINE
    int readVals = scanf(" [ %d - %d , %d , %d ] %c", &from, &to, &capacity, &cost, &endingChar);
    if (readVals == EOF)
      break;

    if (readVals != 5 || from < 0 || to < 0 || from > to || capacity <= 0 || cost <= 0 || i == 100000 || (endingChar != '}' && endingChar != ','))
    {
      return res;
    }

    VehicleInfo info = {
        .availibleFrom = from,
        .availibleTo = to,
        .capacity = capacity,
        .cost = cost,
    };
  }
  res.success = true;

  return res;
}

ProblemReadRes readProblems()
{
  ProblemReadRes res;
  res.success = false;
  while (true)
  {
    int startingDay, pieces;
    // NOLINTNEXTLINE
    int readVals = scanf(" %d %d", &startingDay, &pieces);
    if (readVals == EOF)
      break;
    if (readVals != 2 || startingDay < 0 || pieces <= 0)
    {
      return res;
    }

    Problem problem = {
        .pieces = pieces,
        .startingDate = startingDay,
    };
  }
  res.success = true;

  return res;
}

int main()
{

  return 0;
}