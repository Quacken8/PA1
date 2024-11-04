#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#define MAX_CAPACITY 100000

typedef struct Vehicle
{
  int availibleFrom;
  int availibleTo;
  int capacity;
  int cost;
} Vehicle;

typedef struct VehicleReadRes
{
  bool success;
  int totalNumber;
} VehicleReadRes;

typedef struct Problem
{
  int day;
  int pieces;
} Problem;

typedef struct ProblemReadRes
{
  Problem problem;
  bool eof;
  bool success;
} ProblemReadRes;

static Vehicle vehicles[MAX_CAPACITY];
static int lastPossibleDay = 0;

int compare(const void *elem1, const void *elem2)
{
  Vehicle a = *((Vehicle *)elem1);
  Vehicle b = *((Vehicle *)elem2);
  if (a.availibleTo > b.availibleTo)
    return 1;
  if (a.availibleTo < b.availibleTo)
    return -1;
  return 0;
}

void sortVehicles(Vehicle arr[], int howMany)
{
  qsort(arr, howMany, sizeof(Vehicle), compare);
}

VehicleReadRes readVehicleInfo()
{
  VehicleReadRes res = {
      .success = false,
      .totalNumber = 0,
  };
  printf("Moznosti dopravy:\n");
  char openingBrace;
  // NOLINTNEXTLINE
  if (scanf(" %c", &openingBrace) != 1 || openingBrace != '{')
  {
    return res;
  }

  int from, to, capacity, cost;
  char endingChar;
  int i = 1;
  for (; i <= MAX_CAPACITY + 1; i++)
  {
    // NOLINTNEXTLINE
    if (scanf(" [ %d - %d , %d , %d ] %c", &from, &to, &capacity, &cost, &endingChar) != 5 || from < 0 || to < 0 || from > to || capacity <= 0 || cost <= 0 || i > MAX_CAPACITY || (endingChar != '}' && endingChar != ','))
    {
      return res;
    }

    Vehicle info = {
        .availibleFrom = from,
        .availibleTo = to,
        .capacity = capacity,
        .cost = cost,
    };

    if (to > lastPossibleDay)
      lastPossibleDay = to;

    vehicles[i - 1] = info;
    if (endingChar == '}')
      break;
  };

  res.success = true;
  res.totalNumber = i;

  return res;
}

ProblemReadRes readProblem()
{
  ProblemReadRes res;
  res.success = false;
  res.eof = false;
  int startingDay, pieces;
  // NOLINTNEXTLINE
  int readVals = scanf(" %d %d", &startingDay, &pieces);
  if (readVals == EOF)
  {
    res.eof = true;
    return res;
  }
  if (readVals != 2 || startingDay < 0 || pieces <= 0)
  {
    return res;
  }

  Problem problem = {
      .day = startingDay,
      .pieces = pieces,
  };
  res.problem = problem;
  res.success = true;
  return res;
}

int binaryFindFirstToIndex(int wantedLowestTo, int arrayLen, Vehicle array[])
{
  int lower = 0;
  int upper = arrayLen - 1;

  while (lower < upper)
  {
    int mid = (lower + upper) / 2;
    int guess = array[mid].availibleTo;
    if (wantedLowestTo <= guess)
    {
      upper = mid;
    }
    else
    {
      lower = mid + 1;
    }
  }
  return lower;
}

void solveProblem(Problem problem, int lastPossibleDay, int arrLen, Vehicle array[])
{
  int leftover = problem.pieces;
  int day = problem.day;
  int cost = 0;

  while (leftover > 0)
  {
    int firstToIndex = binaryFindFirstToIndex(day, arrLen, array);
    if (day > lastPossibleDay)
    {
      printf("Prilis velky naklad, nelze odvezt.\n");
      return;
    }
    for (int i = firstToIndex; i < arrLen; i++)
    {
      Vehicle tryVehicle = vehicles[i];
      if (tryVehicle.availibleFrom <= day)
      {
        cost += tryVehicle.cost;
        leftover -= tryVehicle.capacity;
      }
    }
    day++;
  }

  day--;
  printf("Konec: %d, cena: %d\n", day, cost);
  return;
};

int main()
{
  VehicleReadRes readVehicles = readVehicleInfo();
  if (!readVehicles.success)
  {
    printf("Nespravny vstup.\n");
    return 1;
  }

  sortVehicles(vehicles, readVehicles.totalNumber);

  printf("Naklad:\n");
  while (true)
  {
    ProblemReadRes read = readProblem();
    if (read.eof)
    {
      break;
    }
    if (!read.success)
    {
      printf("Nespravny vstup.\n");
      return 2;
    }
    solveProblem(read.problem, lastPossibleDay, readVehicles.totalNumber, vehicles);
  }

  return 0;
}