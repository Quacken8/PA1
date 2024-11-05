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
  int smallestFrom;
  int largestTo;
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

int biggerToFirst(const void *elem1, const void *elem2)
{
  Vehicle a = *((Vehicle *)elem1);
  Vehicle b = *((Vehicle *)elem2);
  if (a.availibleTo > b.availibleTo)
    return -1;
  if (a.availibleTo < b.availibleTo)
    return 1;
  return 0;
}
int smallerFromFirst(const void *elem1, const void *elem2)
{
  Vehicle a = *((Vehicle *)elem1);
  Vehicle b = *((Vehicle *)elem2);
  if (a.availibleFrom > b.availibleFrom)
    return 1;
  if (a.availibleFrom < b.availibleFrom)
    return -1;
  return 0;
}

void sortVehicles(Vehicle arr[], int howMany, int (*fun)(const void *, const void *))
{
  qsort(arr, howMany, sizeof(Vehicle), fun);
}

VehicleReadRes readVehicleInfo()
{
  VehicleReadRes res = {
      .success = false,
      .totalNumber = 0,
      .smallestFrom = INT_MAX,
      .largestTo = INT_MIN,
  };
  printf("Moznosti dopravy:\n");
  char openingBrace;
  // NOLINTNEXTLINE
  if (scanf(" %c", &openingBrace) != 1 || openingBrace != '{')
  {
    return res;
  }

  int from, to, capacity, cost;
  int smallestFrom = INT_MAX;
  int largestTo = INT_MIN;
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

    if (to > largestTo)
      largestTo = to;
    if (from < smallestFrom)
      smallestFrom = from;

    vehicles[i - 1] = info;
    if (endingChar == '}')
      break;
  };

  res.success = true;
  res.totalNumber = i;
  res.largestTo = largestTo;
  res.smallestFrom = smallestFrom;

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

int binaryFindFirstFromIndex(int wantedLowestTo, int arrayLen, Vehicle array[])
{
  int lower = 0;
  int upper = arrayLen - 1;

  while (lower < upper)
  {
    int mid = (lower + upper) / 2;
    int guess = array[mid].availibleFrom;
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

typedef struct TreeNode
{
  bool hasSmaller;
  bool hasLarger;
  int thisCenter;
  int thisLen;
  struct TreeNode *smallerNeighbour;
  struct TreeNode *biggerNeighbour;
  Vehicle *sortedByStart;
  Vehicle *sortedByEnd;
} TreeNode;

TreeNode *constructTree(Vehicle vehicles[], int arrLen, int lower, int upper)
{
  if (arrLen == 1)
  {
    Vehicle v = vehicles[0];
    TreeNode *res = (TreeNode *)malloc(sizeof(TreeNode));
    res->hasSmaller = false;
    res->hasLarger = false;
    res->thisCenter = (v.availibleTo + v.availibleFrom) / 2;
    res->thisLen = 1;
    res->sortedByStart = vehicles;
    res->sortedByEnd = vehicles;
    return res;
  }
  int centerDay = (lower + upper) / 2;
  int smallerLen = 0;
  int largerLen = 0;
  int thisLen = 0;
  int smallersUpper = INT_MIN;
  int largersLower = INT_MAX;

  Vehicle *smallerArr = (Vehicle *)malloc(arrLen * sizeof(Vehicle));
  Vehicle *largerArr = (Vehicle *)malloc(arrLen * sizeof(Vehicle));
  Vehicle *thisArrByFrom = (Vehicle *)malloc(arrLen * sizeof(Vehicle));
  Vehicle *thisArrByTo = (Vehicle *)malloc(arrLen * sizeof(Vehicle));

  for (int i = 0; i < arrLen; i++)
  {
    Vehicle v = vehicles[i];
    int from = v.availibleFrom;
    int to = v.availibleTo;
    if (to < centerDay)
    {
      if (to > smallersUpper)
        smallersUpper = to;
      smallerArr[smallerLen] = v;
      smallerLen++;
    }
    else if (from > centerDay)
    {
      if (from < largersLower)
        largersLower = from;
      largerArr[largerLen] = v;
      largerLen++;
    }
    else
    {
      thisArrByFrom[thisLen] = v;
      thisArrByTo[thisLen] = v;
      thisLen++;
    }
  }

  sortVehicles(thisArrByFrom, thisLen, smallerFromFirst);
  sortVehicles(thisArrByTo, thisLen, biggerToFirst);

  bool hasSmaller = smallerLen != 0;
  TreeNode *smallerNeighbor = (TreeNode *)malloc(sizeof(TreeNode));
  if (hasSmaller)
    smallerNeighbor = constructTree(smallerArr, smallerLen, lower, smallersUpper);

  bool hasLarger = largerLen != 0;
  TreeNode *largerNeighbor = (TreeNode *)malloc(sizeof(TreeNode));
  if (hasLarger)
    largerNeighbor = constructTree(largerArr, largerLen, largersLower, upper);

  TreeNode *res = (TreeNode *)malloc(sizeof(TreeNode));
  res->hasSmaller = (smallerLen != 0);
  res->hasLarger = (largerLen != 0);
  res->thisCenter = centerDay;
  res->thisLen = thisLen;
  res->smallerNeighbour = smallerNeighbor;
  res->biggerNeighbour = largerNeighbor;
  res->sortedByStart = (Vehicle *)malloc(thisLen * sizeof(Vehicle));
  res->sortedByEnd = (Vehicle *)malloc(thisLen * sizeof(Vehicle));
  for (int i = 0; i < thisLen; i++)
  {
    res->sortedByStart[i] = thisArrByFrom[i];
    res->sortedByEnd[i] = thisArrByTo[i];
  }

  return res;
}

typedef struct OneDaysWork
{
  int cost;
  int transportedPieces;
} OneDaysWork;

OneDaysWork evaluateDay(int day, int leftoverPieces, TreeNode *tree)
{
  OneDaysWork res = {
      0,
      0,
  };
  if (leftoverPieces <= 0)
    return res;
  if (tree->thisCenter == day)
  {
    for (int i = 0; i < tree->thisLen; i++)
    {
      Vehicle thisVehicle = tree->sortedByEnd[i];
      res.cost += thisVehicle.cost;
      res.transportedPieces += thisVehicle.capacity;

      if (res.transportedPieces > leftoverPieces)
        break;
    }
    return res;
  }

  if (day < tree->thisCenter)
  {
    for (int i = 0; i < tree->thisLen; i++)
    {
      Vehicle thisVehicle = tree->sortedByStart[i];
      if (thisVehicle.availibleFrom > day)
        break;
      res.cost += thisVehicle.cost;
      res.transportedPieces += thisVehicle.capacity;

      if (res.transportedPieces >= leftoverPieces)
        break;
    }
    if (tree->hasSmaller)
    {
      OneDaysWork smallersRes = evaluateDay(day, leftoverPieces - res.transportedPieces, tree->smallerNeighbour);
      res.cost += smallersRes.cost;
      res.transportedPieces += smallersRes.transportedPieces;
    }
    return res;
  }

  if (day > tree->thisCenter)
  {
    for (int i = 0; i < tree->thisLen; i++)
    {
      Vehicle thisVehicle = tree->sortedByEnd[i];
      if (thisVehicle.availibleTo < day)
        break;
      res.cost += thisVehicle.cost;
      res.transportedPieces += thisVehicle.capacity;

      if (res.transportedPieces >= leftoverPieces)
        break;
    }

    if (tree->hasLarger)
    {
      OneDaysWork largersRes = evaluateDay(day, leftoverPieces - res.transportedPieces, tree->biggerNeighbour);
      res.cost += largersRes.cost;
      res.transportedPieces += largersRes.transportedPieces;
    }
    return res;
  }

  return res;
}

void solveProblem(Problem problem, TreeNode *tree, int lastDay)
{
  int leftover = problem.pieces;
  int day = problem.day;
  int cost = 0;

  while (leftover > 0)
  {
    if (day > lastDay)
    {
      printf("Prilis velky naklad, nelze odvezt.\n");
      return;
    }
    OneDaysWork work = evaluateDay(day, leftover, tree);
    cost += work.cost;
    leftover -= work.transportedPieces;
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
  sortVehicles(vehicles, readVehicles.totalNumber, smallerFromFirst);
  TreeNode *tree = constructTree(vehicles, readVehicles.totalNumber, readVehicles.smallestFrom, readVehicles.largestTo);

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
    solveProblem(read.problem, tree, readVehicles.largestTo);
  }

  // shit frees itself

  return 0;
}