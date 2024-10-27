#ifndef __PROGTEST__
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

constexpr unsigned DOW_MON = 0b0000'0001;
constexpr unsigned DOW_TUE = 0b0000'0010;
constexpr unsigned DOW_WED = 0b0000'0100;
constexpr unsigned DOW_THU = 0b0000'1000;
constexpr unsigned DOW_FRI = 0b0001'0000;
constexpr unsigned DOW_SAT = 0b0010'0000;
constexpr unsigned DOW_SUN = 0b0100'0000;
constexpr unsigned DOW_WORKDAYS =
    DOW_MON | DOW_TUE | DOW_WED | DOW_THU | DOW_FRI;
constexpr unsigned DOW_WEEKEND = DOW_SAT | DOW_SUN;
constexpr unsigned DOW_ALL = DOW_WORKDAYS | DOW_WEEKEND;

typedef struct TDate
{
  unsigned m_Year;
  unsigned m_Month;
  unsigned m_Day;
} TDATE;

TDATE makeDate(unsigned y, unsigned m, unsigned d)
{
  TDATE res = {y, m, d};
  return res;
}
#endif /* __PROGTEST__ */

bool isLeapYear(unsigned y)
{
  return (y % 4 == 0) && (y % 100 != 0 || (y % 400 == 0 && y % 4000 != 0));
}

const unsigned THIRTY_ONE_DAYS = 0b0000'1010'1101'0101;

unsigned daysInAMonth(unsigned year, unsigned month)
{
  if (month == 2)
  {
    if (isLeapYear(year))
      return 29;
    return 28;
  }

  bool canHave31 = (1 << (month - 1)) & THIRTY_ONE_DAYS;

  return 30 + canHave31;
}

bool isValidDate(TDATE date)
{
  if (date.m_Month > 12 || date.m_Month == 0 || date.m_Day == 0)
    return false;
  return date.m_Day <= daysInAMonth(date.m_Year, date.m_Month);
}

// total fucking woodoo, courtesy of Cheriton School of Computer Science
int dayOfWeekShift(TDATE date)
{
  int d = date.m_Day;
  int m = date.m_Month - 2 + 12 * (date.m_Month <= 2);
  int capitalY = date.m_Year - (date.m_Month <= 2);
  int c = capitalY / 100;
  int y = capitalY - 100 * c;

  int w = (d + (26 * m - 2) / 10 + y + y / 4 + c / 4 - 2 * c) % 7 - 1;

  if (w < 0)
  {
    w = 7 + w;
  }

  return w % 7;
}

unsigned numOfMultiples(unsigned from, unsigned to, unsigned multiple)
{
  return (to - from) / multiple + (from != to ? (from % multiple == 0 || to % multiple == 0) : 0);
}

unsigned long long intervalLength(TDATE from, TDATE to)
{
  unsigned long long distance = 1;

  unsigned fromD = from.m_Day;

  while ((fromD - 1) % daysInAMonth(from.m_Year, from.m_Month) + 1 != to.m_Day)
  {
    distance++;
    fromD++;
  }

  unsigned fromM = from.m_Month + (fromD > daysInAMonth(from.m_Year, from.m_Month));
  while ((fromM - 1) % 12 + 1 != to.m_Month)
  {
    distance += daysInAMonth(from.m_Year + (fromM > 12), (fromM - 1) % 12 + 1);
    fromM++;
  }

  unsigned fromY = from.m_Year + (fromM > 12);
  distance += 365 * (to.m_Year - fromY);
  distance += numOfMultiples(fromY, to.m_Year, 4);
  distance -= numOfMultiples(fromY, to.m_Year, 100);
  distance += numOfMultiples(fromY, to.m_Year, 400);
  distance -= numOfMultiples(fromY, to.m_Year, 4000);

  return distance;
}

long long fullWeeksBetween(TDATE from, TDATE to)
{
  return intervalLength(from, to) / 7;
}

bool isFirstBeforeSecond(TDATE first, TDATE second)
{
  if (first.m_Year < second.m_Year)
    return true;
  if (first.m_Year > second.m_Year)
    return false;
  if (first.m_Month < second.m_Month)
    return true;
  if (first.m_Month > second.m_Month)
    return false;
  if (first.m_Day < second.m_Day)
    return true;
  return false;
}

TDATE nextDay(TDATE now)
{
  unsigned day = now.m_Day;
  unsigned month = now.m_Month;
  unsigned year = now.m_Year;

  day++;
  if (day > daysInAMonth(year, month))
  {
    day = 1;
    month++;
  }
  if (month > 12)
  {
    month = 1;
    year++;
  }
  return makeDate(year, month, day);
}

TDATE previousDay(TDATE now)
{
  unsigned day = now.m_Day;
  unsigned month = now.m_Month;
  unsigned year = now.m_Year;

  day--;
  if (day == 0)
  {
    month--;
    day = daysInAMonth(year, month);
  }
  if (month == 12)
  {
    month = 12;
    year--;
  }
  return makeDate(year, month, day);
}

bool areEqual(TDATE a, TDATE b)
{
  return a.m_Day == b.m_Day && a.m_Month == b.m_Month && a.m_Year == b.m_Year;
}

unsigned connectionsToday(TDATE today, unsigned perWorkDay, unsigned dowMask)
{
  unsigned shift = dayOfWeekShift(today);
  unsigned fullAmount = (((1 << shift) & dowMask) != 0) * perWorkDay;
  if (shift == 5)
    return fullAmount / 2 + fullAmount % 2;
  if (shift == 6)
    return fullAmount / 3 + (fullAmount % 3 != 0);
  return fullAmount;
}

unsigned bussesPerWeek(unsigned perWorkDay, unsigned dowMask)
{
  unsigned bussesPerWeek = 0;
  for (int i = 0; i <= 4; i++)
  {
    bussesPerWeek += (((1 << i) & dowMask) != 0) * perWorkDay;
  }
  unsigned fullSat = (((1 << 5) & dowMask) != 0) * perWorkDay;
  bussesPerWeek += fullSat / 2 + fullSat % 2;
  unsigned fullSun = (((1 << 6) & dowMask) != 0) * perWorkDay;
  bussesPerWeek += fullSun / 3 + (fullSun % 3 != 0);
  return bussesPerWeek;
}

long long countConnections(TDATE from, TDATE to, unsigned perWorkDay, unsigned dowMask)
{
  if (isFirstBeforeSecond(to, from) || !isValidDate(from) || !isValidDate(to))
    return -1;

  long long count = 0;

  TDATE newFrom = from;
  do
  {
    count += connectionsToday(newFrom, perWorkDay, dowMask);
    if (areEqual(newFrom, to))
      return count;
    newFrom = nextDay(newFrom);
  } while (dayOfWeekShift(newFrom) != dayOfWeekShift(to) + 1);

  count += bussesPerWeek(perWorkDay, dowMask) * fullWeeksBetween(from, to);
  return count;
}
TDATE endDate(TDATE from, long long connections, unsigned perWorkDay, unsigned dowMask)
{

  if (!isValidDate(from) || dowMask == 0 || perWorkDay == 0 || connections < connectionsToday(from, perWorkDay, dowMask))
  {
    return makeDate(0, 0, 0);
  }

  TDATE to = from;
  unsigned estimatedYears = connections / bussesPerWeek(perWorkDay, dowMask) / 54;
  while (estimatedYears > 0)
  {
    to = makeDate(to.m_Year + estimatedYears, to.m_Month, to.m_Day);
    long long usedUp = countConnections(from, to, perWorkDay, dowMask);

    connections -= usedUp;
    estimatedYears = connections / bussesPerWeek(perWorkDay, dowMask) / 54;
    from = to;
  }

  while (connections >= connectionsToday(to, perWorkDay, dowMask))
  {
    connections -= connectionsToday(to, perWorkDay, dowMask);
    to = nextDay(to);
  }

  return previousDay(to);
}
#ifndef __PROGTEST__
int main()
{
  TDATE d;

  assert(isValidDate(makeDate(2024, 8, 26)));
  assert(isValidDate(makeDate(2024, 2, 29)));
  assert(!isValidDate(makeDate(2024, 8, 32)));

  assert(dayOfWeekShift(makeDate(2024, 10, 26)) == 5);
  assert(dayOfWeekShift(makeDate(2024, 10, 29)) == 1);

  assert(intervalLength(makeDate(2024, 8, 26), makeDate(2024, 8, 29)) == 4);
  assert(intervalLength(makeDate(2024, 12, 31), makeDate(2025, 2, 1)) == 33);
  assert(intervalLength(makeDate(2024, 5, 22), makeDate(2025, 3, 5)) == 288);

  assert(countConnections(makeDate(2024, 10, 1), makeDate(2024, 10, 31), 1,
                          DOW_ALL) == 31);
  assert(countConnections(makeDate(2024, 10, 1), makeDate(2024, 10, 31), 10,
                          DOW_ALL) == 266);
  assert(countConnections(makeDate(2024, 10, 1), makeDate(2024, 10, 31), 1,
                          DOW_WED) == 5);
  assert(countConnections(makeDate(2024, 10, 2), makeDate(2024, 10, 30), 1,
                          DOW_WED) == 5);
  assert(countConnections(makeDate(2024, 10, 1), makeDate(2024, 10, 1), 10,
                          DOW_TUE) == 10);
  assert(countConnections(makeDate(2024, 10, 1), makeDate(2024, 10, 1), 10,
                          DOW_WED) == 0);
  assert(countConnections(makeDate(2024, 1, 1), makeDate(2034, 12, 31), 5,
                          DOW_MON | DOW_FRI | DOW_SAT) == 7462);
  assert(countConnections(makeDate(2024, 1, 1), makeDate(2034, 12, 31), 0,
                          DOW_MON | DOW_FRI | DOW_SAT) == 0);
  assert(countConnections(makeDate(2024, 1, 1), makeDate(2034, 12, 31), 100,
                          0) == 0);
  assert(countConnections(makeDate(2024, 10, 10), makeDate(2024, 10, 9), 1,
                          DOW_MON) == -1);
  assert(countConnections(makeDate(2024, 2, 29), makeDate(2024, 2, 29), 1,
                          DOW_ALL) == 1);
  assert(countConnections(makeDate(2023, 2, 29), makeDate(2023, 2, 29), 1,
                          DOW_ALL) == -1);
  assert(countConnections(makeDate(2100, 2, 29), makeDate(2100, 2, 29), 1,
                          DOW_ALL) == -1);
  assert(countConnections(makeDate(2400, 2, 29), makeDate(2400, 2, 29), 1,
                          DOW_ALL) == 1);
  assert(countConnections(makeDate(4000, 2, 29), makeDate(4000, 2, 29), 1,
                          DOW_ALL) == -1);
  d = endDate(makeDate(2024, 10, 1), 100, 1, DOW_ALL);
  assert(d.m_Year == 2025 && d.m_Month == 1 && d.m_Day == 8);
  d = endDate(makeDate(2024, 10, 1), 100, 6, DOW_ALL);
  assert(d.m_Year == 2024 && d.m_Month == 10 && d.m_Day == 20);
  d = endDate(makeDate(2024, 10, 1), 100, 1, DOW_WORKDAYS);
  assert(d.m_Year == 2025 && d.m_Month == 2 && d.m_Day == 17);
  d = endDate(makeDate(2024, 10, 1), 100, 4, DOW_WORKDAYS);
  assert(d.m_Year == 2024 && d.m_Month == 11 && d.m_Day == 4);
  d = endDate(makeDate(2024, 10, 1), 100, 1, DOW_THU);
  assert(d.m_Year == 2026 && d.m_Month == 9 && d.m_Day == 2);
  d = endDate(makeDate(2024, 10, 1), 100, 2, DOW_THU);
  assert(d.m_Year == 2025 && d.m_Month == 9 && d.m_Day == 17);
  d = endDate(makeDate(2024, 10, 1), 100, 0, DOW_THU);
  assert(d.m_Year == 0 && d.m_Month == 0 && d.m_Day == 0);
  d = endDate(makeDate(2024, 10, 1), 100, 1, 0);
  assert(d.m_Year == 0 && d.m_Month == 0 && d.m_Day == 0);
  d = endDate(makeDate(2024, 10, 27), 5112099, 1, DOW_THU);
  assert(d.m_Year == 100000 && d.m_Month == 1 && d.m_Day == 5);
  d = endDate(makeDate(4001, 1, 11), 5, 6, DOW_SAT);
  assert(d.m_Year == 4001 && d.m_Month == 1 && d.m_Day == 20);

  //  = > r = {4001, 1, 20}, s = {4001, 1, 19};

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
