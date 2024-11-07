#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#define MAX_CAPACITY 100000
#define DOUBLE_TROUBLE 200001

typedef struct Vehicle
{
  long long availibleFrom;
  long long availibleTo;
  long long capacity;
  long long cost;
} Vehicle;

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

typedef struct CumulativeWork
{
  long long pieces;
  long long cost;
  int day;
} CumulativeWork;

static CumulativeWork piecewise[DOUBLE_TROUBLE];
static CumulativeWork integratedPiecewise[DOUBLE_TROUBLE];

long long min(long long a, long long b)
{
  if (a < b)
    return a;
  return b;
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
int smallerToFirst(const void *elem1, const void *elem2)
{
  Vehicle a = *((Vehicle *)elem1);
  Vehicle b = *((Vehicle *)elem2);
  if (a.availibleTo > b.availibleTo)
    return 1;
  if (a.availibleTo < b.availibleTo)
    return -1;
  return 0;
}

void sortVehicles(Vehicle arr[], int howMany, int (*fun)(const void *, const void *))
{
  qsort(arr, howMany, sizeof(Vehicle), fun);
}

VehicleReadRes readVehicleInfo(Vehicle *byFrom, Vehicle *byTo)
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

    byFrom[i - 1] = info;
    byTo[i - 1] = info;
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

int binaryFindDay(int query, int arrayLen, CumulativeWork array[])
{
  int lower = 0;
  int upper = arrayLen - 1;

  while (lower < upper)
  {
    int mid = (lower + upper) / 2;
    int guess = array[mid].day;
    if (query == guess)
    {
      lower = mid;
      break;
    }
    if (query < guess)
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

CumulativeWork valueOnDay(int day, CumulativeWork *integratedPiecewise, CumulativeWork *piecewise, int len)
{
  int index = binaryFindDay(day, len, integratedPiecewise);
  if (integratedPiecewise[index].day == day || index == 0)
    return integratedPiecewise[index];

  CumulativeWork dayBefore = integratedPiecewise[index - 1];
  CumulativeWork derivative = piecewise[index - 1];
  int interval = (day - dayBefore.day);
  CumulativeWork res = {
      .pieces = dayBefore.pieces + derivative.pieces * interval,
      .cost = dayBefore.cost + derivative.cost * interval,
      .day = day,
  };

  return res;
}

CumulativeWork getCumWithPieces(long long pieces, int arrayLen, CumulativeWork integratedPiecewise[], CumulativeWork piecewise[])
{

  int lower = integratedPiecewise[0].day;
  int upper = integratedPiecewise[arrayLen - 1].day;

  while (lower < upper)
  {
    int mid = (lower + upper) / 2;
    CumulativeWork guess = valueOnDay(mid, integratedPiecewise, piecewise, arrayLen);

    if (pieces < guess.pieces)
    {
      upper = mid;
    }
    else if (pieces > guess.pieces)
    {
      lower = mid + 1;
    }
    else if (lower != upper)
    {
      upper = mid;
    }
  }

  return valueOnDay(lower, integratedPiecewise, piecewise, arrayLen);
}

void evaluateDays(int startDay, long long leftoverPieces, CumulativeWork *piecewise, CumulativeWork *integratedPiecewise, int piecewiseLen)
{
  CumulativeWork beforeFirstDay = valueOnDay(startDay - 1, integratedPiecewise, piecewise, piecewiseLen);

  CumulativeWork lastDay = getCumWithPieces(beforeFirstDay.pieces + leftoverPieces, piecewiseLen, integratedPiecewise, piecewise);

  if ((lastDay.pieces - beforeFirstDay.pieces) < leftoverPieces)
  {
    printf("Prilis velky naklad, nelze odvezt.\n");
  }
  else
  {

    printf("Konec: %d, cena: %lld\n", lastDay.day, lastDay.cost - beforeFirstDay.cost);
  }
};

void solveProblem(Problem problem, int lastDay, int firstDay, CumulativeWork *piecewise, int piecewiseLen)
{
  long long leftover = problem.pieces;
  int day = problem.day < firstDay ? firstDay : problem.day;

  if (day > lastDay)
  {
    printf("Prilis velky naklad, nelze odvezt.\n");
    return;
  }
  evaluateDays(day, leftover, piecewise, integratedPiecewise, piecewiseLen);

  return;
};

int setUpPiecewiseArr(Vehicle *vehiclesByFrom, Vehicle *vehiclesByTo, int vehiclesLen)
{
  CumulativeWork zeroethKnee = {
      .pieces = 0,
      .cost = 0,
      .day = -1,
  };
  piecewise[0] = zeroethKnee;
  int fromsIndex = 0;
  int tosIndex = 0;
  int piecewiseIndex = 1;
  Vehicle fromV = vehiclesByFrom[0], toV = vehiclesByTo[0];
  CumulativeWork carriedOver = {
      .pieces = -1,
      .cost = -1,
      .day = -1,
  };
  CumulativeWork yesterdaysRes = piecewise[0];
  while (fromsIndex < vehiclesLen || tosIndex < vehiclesLen)
  {
    long long cost = yesterdaysRes.cost;
    long long pieces = yesterdaysRes.pieces;

    if (carriedOver.day != -1)
    {
      tosIndex++;
      pieces -= carriedOver.pieces;
      cost -= carriedOver.cost;
    }

    if (fromsIndex < vehiclesLen)
    {
      fromV = vehiclesByFrom[fromsIndex];
    }
    else
    {
      fromV.availibleFrom = LLONG_MAX;
    }
    if (tosIndex < vehiclesLen)
    {
      toV = vehiclesByTo[tosIndex];
    }
    else
    {
      toV.availibleTo = LLONG_MAX;
    }

    int today;
    if (carriedOver.day != -1)
    {
      today = carriedOver.day;
    }
    else
      today = min(fromV.availibleFrom, toV.availibleTo);

    if (fromV.availibleFrom == today)
    {
      fromsIndex++;
      cost += fromV.cost;
      pieces += fromV.capacity;
    }
    if (toV.availibleTo == today)
    {
      CumulativeWork carry = {
          .pieces = toV.capacity,
          .cost = toV.cost,
          .day = today + 1,
      };
      carriedOver = carry;
    }

    if (carriedOver.day == today || fromV.availibleFrom == today)
    {
      if (carriedOver.day == today)
        carriedOver.day = -1;

      CumulativeWork todayRes = {
          .pieces = pieces,
          .cost = cost,
          .day = today,
      };
      yesterdaysRes = todayRes;
      piecewise[piecewiseIndex] = todayRes;
      piecewiseIndex++;
    }
  }

  return piecewiseIndex;
}

void integratePieceWise(int piecewiseLen)
{
  long long lastCost = 0;
  long long lastPieces = 0;
  long long currCost = lastCost;
  long long currPieces = lastPieces;

  for (int i = 1; i < piecewiseLen; i++)
  {
    int intervalLen = piecewise[i].day - piecewise[i - 1].day;

    currCost += (intervalLen - 1) * lastCost + piecewise[i].cost;
    currPieces += (intervalLen - 1) * lastPieces + piecewise[i].pieces;
    lastCost = piecewise[i].cost;
    lastPieces = piecewise[i].pieces;

    integratedPiecewise[i].cost = currCost;
    integratedPiecewise[i].pieces = currPieces;
    integratedPiecewise[i].day = piecewise[i].day;
  }
}

int main()
{
  static Vehicle vehiclesByFrom[MAX_CAPACITY];
  static Vehicle vehiclesByTo[MAX_CAPACITY];
  VehicleReadRes readVehicles = readVehicleInfo(vehiclesByFrom, vehiclesByTo);
  if (!readVehicles.success)
  {
    printf("Nespravny vstup.\n");
    return 1;
  }
  sortVehicles(vehiclesByFrom, readVehicles.totalNumber, smallerFromFirst);
  sortVehicles(vehiclesByTo, readVehicles.totalNumber, smallerToFirst);

  int pieceWiseLen = setUpPiecewiseArr(vehiclesByFrom, vehiclesByTo, readVehicles.totalNumber);
  integratePieceWise(pieceWiseLen);

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
    solveProblem(read.problem, readVehicles.largestTo, readVehicles.smallestFrom, piecewise, pieceWiseLen);
  }

  return 0;
}