#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Array
{
  unsigned len;
  unsigned capacity;
  unsigned *data;
} Array;

Array array(unsigned initialCapacity)
{
  Array arr = {
      .len = 0,
      .capacity = initialCapacity,
      .data = (unsigned *)malloc(initialCapacity * sizeof(unsigned)),
  };
  return arr;
}

void freeArr(Array *arr)
{
  free(arr->data);
  arr->len = 0;
  arr->capacity = 0;
}

void push(Array *arr, unsigned element)
{
  if (arr->len == arr->capacity)
  {
    arr->capacity *= 2;
    arr->data = (unsigned *)realloc(arr->data, arr->capacity * sizeof(unsigned));
    if (arr->data == NULL)
    {
      printf("Allocation failed!");
      exit(2);
    }
  }

  arr->data[arr->len] = element;
  arr->len++;
}

typedef enum ReadResultType
{
  ERROR,
  EOF_REACHED,
  NEW_ELEMENT,
  RANGE_QUERY,
} ReadResultType;

/**
 * Based on the content of the type only some of the fields have defined values
 *  * ERROR | EOF_REACHED => no fields have defined values
 *  * NEW_ELEMENT => only newEl is defined
 *  * RANGE_QUERY => only queryFrom and queryTo are defined
 */
typedef struct ReadRes
{
  ReadResultType type;
  unsigned newEl;
  unsigned queryFrom;
  unsigned queryTo;
} ReadRes;

ReadRes readInput(unsigned lastDay)
{
  char leadingChar;
  unsigned firstNum;
  ReadRes res;
  // NOLINTNEXTLINE
  int readVars = scanf(" %c %u", &leadingChar, &firstNum);
  if (readVars == EOF)
  {
    res.type = EOF_REACHED;
  }
  else if (readVars == 2 && leadingChar == '+')
  {
    res.newEl = firstNum;
    res.type = NEW_ELEMENT;
  }
  else if (readVars == 2 && leadingChar == '?')
  {
    unsigned secondNum;
    // NOLINTNEXTLINE
    int read = scanf("%u", &secondNum);
    if (read == 1 && secondNum <= lastDay)
    {
      res.queryFrom = firstNum;
      res.queryTo = secondNum;
      res.type = RANGE_QUERY;
    }
    else
    {
      res.type = ERROR;
    }
  }
  else
  {
    res.type = ERROR;
  }

  return res;
}

typedef struct Trade
{
  bool nonAplicable;
  unsigned absoluteValue;
  unsigned buyDay;
  unsigned sellDay;
} Trade;

void printTrade(Trade trade)
{
  if (trade.nonAplicable)
  {
    printf("N/A\n");
  }
  else
  {
    printf("%u (%u - %u)\n", trade.absoluteValue, trade.buyDay, trade.sellDay);
  }
}

typedef struct QueryRes
{
  Trade gain;
  Trade loss;
} QueryRes;

typedef struct PriceOnDay
{
  unsigned day;
  unsigned price;
} PriceOnDay;

QueryRes
findTrades(Array market, unsigned from, unsigned to)
{
  Trade gain = {
      .nonAplicable = true,
      .absoluteValue = 0,
      .buyDay = from,
      .sellDay = from,
  };
  Trade loss = {
      .nonAplicable = true,
      .absoluteValue = 0,
      .buyDay = from,
      .sellDay = from,
  };

  PriceOnDay lowestEver = {
      .day = from,
      .price = market.data[from],
  };
  PriceOnDay highestEver = lowestEver;

  for (unsigned today = from + 1; today <= to; today++)
  {
    unsigned todaysPrice = market.data[today];

    if (todaysPrice >= lowestEver.price)
    {
      unsigned newGain = todaysPrice - lowestEver.price;

      if (newGain > gain.absoluteValue)
      {
        unsigned gainDollers = todaysPrice - lowestEver.price;

        gain.nonAplicable = false;
        gain.absoluteValue = gainDollers;
        gain.buyDay = lowestEver.day;
        gain.sellDay = today;
      }
    }
    else
    {
      lowestEver.day = today;
      lowestEver.price = todaysPrice;
    }

    if (todaysPrice <= highestEver.price)
    {
      unsigned newLoss = highestEver.price - todaysPrice;

      if (newLoss > loss.absoluteValue)
      {
        unsigned lossDollers = highestEver.price - todaysPrice;

        loss.nonAplicable = false;
        loss.absoluteValue = lossDollers;
        loss.buyDay = highestEver.day;
        loss.sellDay = today;
      }
    }
    else
    {
      highestEver.day = today;
      highestEver.price = todaysPrice;
    }
  }

  QueryRes res = {
      .gain = gain,
      .loss = loss,
  };
  return res;
}

int main()
{
  unsigned initialCapacity = 64;
  Array market = array(initialCapacity);

  printf("Ceny, hledani:\n");
  while (true)
  {
    ReadRes input = readInput(market.len - 1);

    QueryRes minmax; // Label followed by a declaration is a C23 extension; not yet sure whether I care about backwards compatibility though
    switch (input.type)
    {
    case EOF_REACHED:
      freeArr(&market);
      return 0;

    case ERROR:
      freeArr(&market);
      printf("Nespravny vstup.\n");
      return 1;

    case NEW_ELEMENT:
      push(&market, input.newEl);
      break;

    case RANGE_QUERY:
      minmax = findTrades(market, input.queryFrom, input.queryTo);
      printf("Nejvyssi zisk: ");
      printTrade(minmax.gain);
      printf("Nejvyssi ztrata: ");
      printTrade(minmax.loss);
      break;
    }
  }
}