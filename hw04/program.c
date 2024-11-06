#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#define MAX_CAPACITY 100000

typedef struct Vehicle
{
  long long availibleFrom;
  long long availibleTo;
  long long capacity;
  long long cost;
} Vehicle;

static Vehicle vehicles[MAX_CAPACITY];

typedef struct VehicleReadRes
{
  bool success;
  long long totalNumber;
  long long smallestFrom;
  long long largestTo;
} VehicleReadRes;

typedef struct Problem
{
  long long day;
  long long pieces;
} Problem;

typedef struct ProblemReadRes
{
  Problem problem;
  bool eof;
  bool success;
} ProblemReadRes;

long long clamp(long long lo, long long x, long long hi)
{
  if (x > hi)
    return hi;
  if (x < lo)
    return lo;
  return x;
}

int largerToFirst(const void *elem1, const void *elem2)
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

void sortVehicles(Vehicle arr[], long long howMany, int (*fun)(const void *, const void *))
{
  qsort(arr, howMany, sizeof(Vehicle), fun);
}

VehicleReadRes readVehicleInfo(Vehicle *arr)
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

  long long from, to, capacity, cost;
  long long smallestFrom = INT_MAX;
  long long largestTo = INT_MIN;
  char endingChar;
  long long i = 1;
  for (; i <= MAX_CAPACITY + 1; i++)
  {
    // NOLINTNEXTLINE
    if (scanf(" [ %lld - %lld , %lld , %lld ] %c", &from, &to, &capacity, &cost, &endingChar) != 5 || from < 0 || to < 0 || from > to || capacity <= 0 || cost <= 0 || i > MAX_CAPACITY || (endingChar != '}' && endingChar != ','))
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

    arr[i - 1] = info;
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
  long long startingDay, pieces;
  // NOLINTNEXTLINE
  long long readVals = scanf(" %lld %lld", &startingDay, &pieces);
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

typedef struct CumulativeWork
{
  long long pieces;
  long long cost;
  long long day;
} CumulativeWork;

long long binaryFindIndex(long long wantedLowestTo, long long arrayLen, CumulativeWork array[])
{
  long long lower = 0;
  long long upper = arrayLen - 1;

  while (lower < upper)
  {
    long long mid = (lower + upper) / 2;
    long long guess = array[mid].day;
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
  long long thisCenter;
  long long fromLen;
  long long toLen;
  struct TreeNode *smallerNeighbour;
  struct TreeNode *biggerNeighbour;
  CumulativeWork *sortedByFrom;
  CumulativeWork *sortedByTo;
} TreeNode;

void freeTree(TreeNode *tree)
{
  if (tree == NULL)
    return;
  if (tree->hasLarger)
    freeTree(tree->biggerNeighbour);
  if (tree->hasSmaller)
    freeTree(tree->smallerNeighbour);

  free(tree->sortedByTo);
  free(tree->sortedByFrom);
  free(tree);
}

TreeNode *constructTree(Vehicle vehicles[], long long arrLen, long long lower, long long upper)
{
  Vehicle *smallerArr = (Vehicle *)malloc(arrLen * sizeof(Vehicle));
  Vehicle *largerArr = (Vehicle *)malloc(arrLen * sizeof(Vehicle));
  Vehicle *thisArr = (Vehicle *)malloc(arrLen * sizeof(Vehicle));
  CumulativeWork *thisArrByFrom = (CumulativeWork *)malloc(arrLen * sizeof(CumulativeWork));
  CumulativeWork *thisArrByTo = (CumulativeWork *)malloc(arrLen * sizeof(CumulativeWork));
  TreeNode *res = (TreeNode *)malloc(sizeof(TreeNode));

  if (res == NULL || smallerArr == NULL || largerArr == NULL || thisArrByFrom == NULL || thisArrByTo == NULL)
  {
    free(res);
    free(thisArr);
    free(smallerArr);
    free(largerArr);
    free(thisArrByFrom);
    free(thisArrByTo);
    return NULL;
  }

  if (arrLen == 1)
  {
    Vehicle v = vehicles[0];
    CumulativeWork fromWork = {
        .pieces = v.capacity,
        .cost = v.cost,
        .day = v.availibleFrom,
    };
    CumulativeWork toWork = fromWork;
    toWork.day = v.availibleTo;
    res->hasSmaller = false;
    res->hasLarger = false;
    res->thisCenter = (v.availibleTo + v.availibleFrom) / 2;
    res->fromLen = 1;
    res->toLen = 1;
    res->sortedByFrom = thisArrByFrom;
    res->sortedByTo = thisArrByTo;
    res->sortedByFrom[0] = fromWork;
    res->sortedByTo[0] = toWork;
    free(smallerArr);
    free(largerArr);
    free(thisArr);

    return res;
  }
  long long centerDay = (lower + upper) / 2;
  long long smallerLen = 0;
  long long largerLen = 0;
  long long thisLen = 0;
  long long smallersUpper = INT_MIN;
  long long largersLower = INT_MAX;

  for (long long i = 0; i < arrLen; i++)
  {
    Vehicle v = vehicles[i];
    long long from = v.availibleFrom;
    long long to = v.availibleTo;
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
      thisArr[thisLen] = v;
      thisLen++;
    }
  }

  long long thisFromLen = 0;
  long long thisToLen = 0;

  if (thisLen != 0)
  {
    sortVehicles(thisArr, thisLen, smallerFromFirst);
    CumulativeWork first = {
        .pieces = thisArr[0].capacity,
        .cost = thisArr[0].cost,
        .day = thisArr[0].availibleFrom,
    };
    thisArrByFrom[0] = first;
    long long j = 0;
    long long cumCost = thisArrByFrom[0].cost;
    long long cumPieces = thisArrByFrom[0].pieces;
    for (long long i = 1; i < thisLen; i++)
    {
      Vehicle v = thisArr[i];
      cumCost += v.cost;
      cumPieces += v.capacity;
      if (thisArrByFrom[j].day == v.availibleFrom)
      {
        thisArrByFrom[j].cost = cumCost;
        thisArrByFrom[j].pieces = cumPieces;
      }
      else
      {
        j++;
        CumulativeWork next = {
            .pieces = cumPieces,
            .cost = cumCost,
            .day = v.availibleFrom,
        };
        thisArrByFrom[j] = next;
      }
    }
    thisFromLen = j + 1;

    sortVehicles(thisArr, thisLen, largerToFirst);
    CumulativeWork last = {
        .pieces = thisArr[0].capacity,
        .cost = thisArr[0].cost,
        .day = thisArr[0].availibleTo,
    };
    thisArrByTo[0] = last;
    j = 0;
    cumCost = thisArrByTo[0].cost;
    cumPieces = thisArrByTo[0].pieces;
    for (long long i = 1; i < thisLen; i++)
    {
      Vehicle v = thisArr[i];
      cumCost += v.cost;
      cumPieces += v.capacity;
      if (thisArrByTo[j].day == v.availibleTo)
      {
        thisArrByTo[j].cost = cumCost;
        thisArrByTo[j].pieces = cumPieces;
      }
      else
      {
        j++;
        CumulativeWork next = {
            .pieces = cumPieces,
            .cost = cumCost,
            .day = v.availibleTo,
        };
        thisArrByTo[j] = next;
      }
    }
    thisToLen = j + 1;
    for (int i = 0; i < thisToLen / 2; i++)
    {
      CumulativeWork thisEl = thisArrByTo[i];
      CumulativeWork thatEl = thisArrByTo[thisToLen - 1 - i];
      thisArrByTo[i] = thatEl;
      thisArrByTo[thisToLen - 1 - i] = thisEl;
    }
  }

  bool hasSmaller = smallerLen != 0;
  TreeNode *smallerNeighbor = NULL;
  if (hasSmaller)
    smallerNeighbor = constructTree(smallerArr, smallerLen, lower, smallersUpper);

  bool hasLarger = largerLen != 0;
  TreeNode *largerNeighbor = NULL;
  if (hasLarger)
    largerNeighbor = constructTree(largerArr, largerLen, largersLower, upper);

  res->hasSmaller = (smallerLen != 0);
  res->hasLarger = (largerLen != 0);
  res->thisCenter = centerDay;
  res->fromLen = thisFromLen;
  res->toLen = thisToLen;
  res->smallerNeighbour = smallerNeighbor;
  res->biggerNeighbour = largerNeighbor;
  res->sortedByFrom = thisArrByFrom;
  res->sortedByTo = thisArrByTo;

  free(smallerArr);
  free(largerArr);
  free(thisArr);
  return res;
}

typedef struct OneDaysWork
{
  long long cost;
  long long transportedPieces;
} OneDaysWork;

OneDaysWork evaluateDay(long long day, long long leftoverPieces, TreeNode *tree)
{
  OneDaysWork res = {
      0,
      0,
  };

  if (tree->thisCenter == day && tree->fromLen != 0)
  {
    res.cost = tree->sortedByFrom[tree->fromLen - 1].cost;
    res.transportedPieces = tree->sortedByFrom[tree->fromLen - 1].pieces;
    return res;
  }

  if (day < tree->thisCenter)
  {
    if (tree->fromLen != 0 && day >= tree->sortedByFrom[0].day)
    {
      long long index = binaryFindIndex(day, tree->fromLen, tree->sortedByFrom);

      if (tree->sortedByFrom[index].day > day)
        index--;

      index = clamp(0, index, tree->fromLen);
      res.cost = tree->sortedByFrom[index].cost;
      res.transportedPieces = tree->sortedByFrom[index].pieces;
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
    if (tree->toLen != 0 && day <= tree->sortedByTo[tree->toLen - 1].day)
    {
      long long index = binaryFindIndex(day, tree->toLen, tree->sortedByTo);
      res.cost = tree->sortedByTo[index].cost;
      res.transportedPieces = tree->sortedByTo[index].pieces;
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

void solveProblem(Problem problem, TreeNode *tree, long long lastDay, long long firstDay)
{
  long long leftover = problem.pieces;
  long long day = problem.day < firstDay ? firstDay : problem.day;
  long long cost = 0;

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
  printf("Konec: %lld, cena: %lld\n", day, cost);
  return;
};

int main()
{

  VehicleReadRes readVehicles = readVehicleInfo(vehicles);
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
      freeTree(tree);
      printf("Nespravny vstup.\n");
      return 2;
    }
    solveProblem(read.problem, tree, readVehicles.largestTo, readVehicles.smallestFrom);
  }

  freeTree(tree);

  return 0;
}