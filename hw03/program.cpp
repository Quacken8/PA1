#ifndef __PROGTEST__
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

  bool canHave31 = ((1u << (month - 1)) & THIRTY_ONE_DAYS) != 0;

  return 30 + canHave31;
}

bool isValidDate(TDATE date)
{
  if (date.m_Month > 12 || date.m_Month == 0 || date.m_Day == 0)
    return false;
  return date.m_Day <= daysInAMonth(date.m_Year, date.m_Month);
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
  if (month == 0)
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

unsigned leapsSinceJesus(unsigned year)
{
  return year / 4 - year / 100 + year / 400 - year / 4000;
}

unsigned long long daysSinceJesus(TDATE date)
{
  unsigned long long res = date.m_Day + 365 * (date.m_Year - 2000) + leapsSinceJesus(date.m_Year - 1) - leapsSinceJesus(1999);
  for (int i = date.m_Month - 1; i > 0; i--)
  {
    res += daysInAMonth(date.m_Year, i);
  }
  return res;
}

TDATE jesusToDate(unsigned long long days)
{
  unsigned long long allDays = days;
  TDATE res = makeDate(2000, 1, 1);
  do
  {
    unsigned long long years = days / 366;
    res = makeDate(res.m_Year + years, 1, 1);
    days = allDays - daysSinceJesus(res);
  } while (days > 366);

  for (; days > 0; days--)
  {
    res = nextDay(res);
  }

  return res;
}

int dayOfWeekShift(TDATE date)
{

  unsigned shiftedMonth = (date.m_Month + 9) % 12 + 3;
  unsigned Y = date.m_Year - (date.m_Month <= 2);
  unsigned h = (date.m_Day + (13 * shiftedMonth + 13) / 5 + Y + leapsSinceJesus(Y)) % 7;

  return (h + 5) % 7;
}

unsigned long long intervalLength(TDATE from, TDATE to)
{
  return daysSinceJesus(to) - daysSinceJesus(from) + 1;
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

unsigned connectionsToday(TDATE today, unsigned perWorkDay, unsigned dowMask)
{
  unsigned shift = dayOfWeekShift(today);
  unsigned fullAmount = (((1 << shift) & dowMask) != 0) * perWorkDay;
  if (shift == 5)
    return (fullAmount + 1) / 2;
  if (shift == 6)
    return (fullAmount + 2) / 3;
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
  while (dayOfWeekShift(newFrom) != (dayOfWeekShift(to) + 1) % 7)
  {
    count += connectionsToday(newFrom, perWorkDay, dowMask);
    if (areEqual(newFrom, to))
      return count;
    newFrom = nextDay(newFrom);
  }

  count += bussesPerWeek(perWorkDay, dowMask) * fullWeeksBetween(newFrom, to);
  return count;
}
TDATE endDate(TDATE from, long long connections, unsigned perWorkDay, unsigned dowMask)
{

  if (!isValidDate(from) || dowMask == 0 || perWorkDay == 0 || connections < connectionsToday(from, perWorkDay, dowMask))
  {
    return makeDate(0, 0, 0);
  }

  unsigned long long weeks = connections / bussesPerWeek(perWorkDay, dowMask);
  TDATE guessDate = from;
  unsigned long long leftover = 0;
  do
  {
    guessDate = makeDate(guessDate.m_Year + weeks / 53, guessDate.m_Month, guessDate.m_Day);
    if (!isValidDate(guessDate))
      guessDate.m_Day = 28;
    leftover = connections - countConnections(from, guessDate, perWorkDay, dowMask);
    weeks = leftover / bussesPerWeek(perWorkDay, dowMask);
  } while (weeks >= 53);

  while (leftover >= connectionsToday(nextDay(guessDate), perWorkDay, dowMask))
  {
    guessDate = nextDay(guessDate);
    leftover -= connectionsToday(guessDate, perWorkDay, dowMask);
  }

  return guessDate;
}

#ifndef __PROGTEST__

void randomTrials()
{
  const long long MAX_YEAR = 100000000;
  const unsigned TRIALS = 1000;
  const unsigned MAX_PER_DAY = 1000;
  srand(time(NULL));
  for (unsigned i = 0; i < TRIALS; i++)
  {
    unsigned long long d = rand();
    assert(daysSinceJesus(jesusToDate(d)) == d);
  }
  unsigned errors = 0;
  for (unsigned i = 0; i < TRIALS; i++)
  {
    unsigned aYear = rand() % (MAX_YEAR - 2000) + 2000;
    unsigned aMonth = rand() % 12 + 1;
    unsigned aDay = rand() % daysInAMonth(aYear, aMonth) + 1;
    TDATE a = makeDate(aYear, aMonth, aDay);

    unsigned cYear = rand() % (MAX_YEAR - 2000) + 2000;
    unsigned cMonth = rand() % 12 + 1;
    unsigned cDay = rand() % daysInAMonth(aYear, aMonth) + 1;
    TDATE c = makeDate(cYear, cMonth, cDay);
    unsigned perWorkDay = rand() % MAX_PER_DAY;
    unsigned dowMask = DOW_WORKDAYS; // rand() % (DOW_ALL + 1);

    long long whole = countConnections(a, c, perWorkDay, dowMask);
    TDATE tryC = endDate(a, whole, perWorkDay, dowMask);
    long long leftover = whole - countConnections(a, tryC, perWorkDay, dowMask);
    TDATE actualC = c;
    while (connectionsToday(nextDay(actualC), perWorkDay, dowMask) <= leftover && dowMask != 0 && perWorkDay != 0)
    {
      actualC = nextDay(actualC);
    }

    if (!areEqual(actualC, tryC) && whole != -1 && dowMask != 0 && perWorkDay != 0)
    {
      errors++;
      printf("Parameters:\n");
      printf("a: %u-%u-%u\n", a.m_Year, a.m_Month, a.m_Day);
      printf("c: %u-%u-%u\n", c.m_Year, c.m_Month, c.m_Day);
      printf("perWorkDay: %d\n", perWorkDay);
      printf("dowMask: %u\n", dowMask);
      printf("-------------------\n");
      printf("Output:\n");
      printf("whole:\t%lld\n", whole);
      printf("c:\t%u-%u-%u\n", actualC.m_Year, actualC.m_Month, actualC.m_Day);
      printf("tryC:\t%u-%u-%u\n", tryC.m_Year, tryC.m_Month, tryC.m_Day);

      printf("==================\n");
    }
  }
  printf("found %d errors, error rate of %.2lf %%", errors, 100 * (double)errors / (double)TRIALS);
}

int main()
{
  TDATE d;

  assert(isValidDate(makeDate(2024, 8, 26)));
  assert(isValidDate(makeDate(2024, 2, 29)));
  assert(!isValidDate(makeDate(2024, 8, 32)));

  assert(dayOfWeekShift(makeDate(2024, 10, 26)) == 5);
  assert(dayOfWeekShift(makeDate(2024, 10, 29)) == 1);

  assert(daysSinceJesus(jesusToDate(100000)) == 100000);

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
  d = endDate(makeDate(4001, 1, 11), 5, 6, DOW_SAT);
  assert(d.m_Year == 4001 && d.m_Month == 1 && d.m_Day == 20);
  // d = endDate(makeDate(2024, 10, 27), 5112099, 1, DOW_THU);
  // assert(d.m_Year == 100000 && d.m_Month == 1 && d.m_Day == 5);

  assert(countConnections(makeDate(2024, 1, 1), makeDate(2024, 12, 31), 1, DOW_MON) == 53);
  assert(countConnections(makeDate(2024, 1, 1), makeDate(2024, 12, 31), 1, DOW_MON | DOW_TUE) == 106);
  assert(countConnections(makeDate(2024, 1, 1), makeDate(2024, 12, 31), 1, DOW_MON | DOW_TUE | DOW_WED) == 158);
  assert(countConnections(makeDate(2024, 1, 1), makeDate(2024, 12, 31), 1, DOW_MON | DOW_TUE | DOW_WED | DOW_THU) == 210);
  assert(countConnections(makeDate(2024, 1, 1), makeDate(2024, 12, 31), 1, DOW_MON | DOW_TUE | DOW_WED | DOW_THU | DOW_FRI) == 262);
  assert(countConnections(makeDate(2024, 1, 1), makeDate(2024, 12, 31), 1, DOW_MON | DOW_TUE | DOW_WED | DOW_THU | DOW_FRI | DOW_SAT) == 314);
  assert(countConnections(makeDate(2024, 1, 1), makeDate(2024, 12, 31), 1, DOW_ALL) == 366);
  d = endDate(makeDate(2024, 1, 1), 52, 1, DOW_MON);
  assert(d.m_Year == 2024 && d.m_Month == 12 && d.m_Day == 29);

  d = endDate(makeDate(2024, 1, 1), 104, 1, DOW_MON | DOW_TUE);
  assert(d.m_Year == 2024 && d.m_Month == 12 && d.m_Day == 29);

  d = endDate(makeDate(2024, 1, 1), 156, 1, DOW_MON | DOW_TUE | DOW_WED);
  assert(d.m_Year == 2024 && d.m_Month == 12 && d.m_Day == 29);

  d = endDate(makeDate(2024, 1, 1), 208, 1, DOW_MON | DOW_TUE | DOW_WED | DOW_THU);
  assert(d.m_Year == 2024 && d.m_Month == 12 && d.m_Day == 29);

  d = endDate(makeDate(2024, 1, 1), 260, 1, DOW_MON | DOW_TUE | DOW_WED | DOW_THU | DOW_FRI);
  assert(d.m_Year == 2024 && d.m_Month == 12 && d.m_Day == 29);

  assert(countConnections(makeDate(2033, 8, 25), makeDate(2493, 2, 21), 15, DOW_MON | DOW_TUE | DOW_THU | DOW_FRI | DOW_SUN) == 1558405);

  endDate(makeDate(2609745, 7, 26), 62958900104, 52, 124);

  endDate(makeDate(2000, 12, 15), 519, 45, 126);

  randomTrials();

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
