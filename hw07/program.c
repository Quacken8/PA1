#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_CAPACITY 64

typedef struct ChArray
{
  size_t len;
  size_t capacity;
  char *data;
} ChArray;

void pushChar(ChArray *arr, char e)
{
  if (arr->capacity == arr->len)
  {
    arr->capacity *= 2;
    arr->data = (char *)realloc(arr->data, arr->capacity * sizeof(char));
  }
  arr->data[arr->len++] = e;
}

ChArray newCharray()
{
  ChArray res = {
      .len = 0,
      .capacity = INITIAL_CAPACITY,
      .data = (char *)malloc(INITIAL_CAPACITY * sizeof(char)),
  };
  return res;
}

typedef struct Slice
{
  size_t leadingZeroes;
  size_t firstIndex;
  size_t lastIndex;
} Slice;

bool isOdd(char c)
{
  unsigned asNumber = c - '0';
  return asNumber % 2;
}

typedef enum ReadRes
{
  Error,
  Eof,
  Num,
  Print,
} ReadRes;

/**
 * Overwrites the input array's data
 */
ReadRes readInput(ChArray *buffer)
{
  buffer->len = 0;
  char definingSybmol = getchar();
  if (definingSybmol == EOF)
    return Eof;
  if (definingSybmol != '?' && definingSybmol != '#')
    return Error;

  // NOLINTNEXTLINE
  scanf(" ");

  while (true)
  {
    char digit = getchar();

    if (digit == '\n')
      break;

    if (!isdigit(digit))
      return Error;

    pushChar(buffer, digit);
    pushChar(buffer, 0);
  }

  if (buffer->len == 0)
    return Error;
  if (definingSybmol == '#')
    return Num;

  return Print;
}

void printSequence(ChArray sequence)
{
  printf("* ");
  for (size_t i = 0; i < sequence.len - 1; i++) // skipping trailing comma in print is easier than handling it in logic
  {
    printf("%c", sequence.data[i]);
  }
  printf("\n");
}

/**
 * performs a-b that resolves to 0 if an underflow were to happen
 */
size_t safeSubtract(size_t a, size_t b)
{
  if (b > a)
    return 0;
  return a - b;
}

bool leftHalfStrictlyBigger(Slice slice, ChArray original)
{
  size_t middleIndex = (slice.lastIndex - slice.firstIndex) / 2;
  for (size_t i = slice.firstIndex, j = middleIndex + 1; i < middleIndex; i += 2, j += 2)
  {
    char a = original.data[i], b = original.data[j];
    if (a < b)
      return false;
    if (a > b)
      return true;
  }
  return false;
}

void lookForCombinations(ChArray original, bool printCombinations)
{
  unsigned long long totalCombinations = 0;

  unsigned long long leadingZeroes = 0;
  for (size_t i = 0; i < original.len; i += 2)
  {
    if (original.data[i] != '0')
      break;
    leadingZeroes++;
  }

  Slice sequence = {
      .leadingZeroes = leadingZeroes,
      .firstIndex = 0,
      .lastIndex = original.len - 1,
  };

  for (size_t i = sequence.firstIndex; i < sequence.lastIndex; i += 2)
  {
    char digit = original.data[i];
    size_t leftLen = safeSubtract(i + 2 - sequence.firstIndex, 2 * sequence.leadingZeroes);
    size_t rightLen = sequence.lastIndex - i - 1; // FIXME explain: Upper bound is enough
    bool valid = isOdd(digit) || (leftLen < rightLen) || (rightLen == 0) ? true : (leftLen > rightLen ? false : !leftHalfStrictlyBigger(sequence, original));

    if (valid)
    {
      totalCombinations++;

      if (printCombinations)
      {
        original.data[i + 1] = ',';
        printSequence(original);
        original.data[i + 1] = 0;
      }
    }
  }

  printf("%lld\n", totalCombinations);
  return;
};

int main()
{
  ChArray sequence = newCharray();
  printf("Posloupnost:\n");
  while (true)
  {
    ReadRes res = readInput(&sequence);

    if (res == Error)
    {
      printf("Nespravny vstup\n");
      break;
    }
    if (res == Eof)
      break;

    bool printCombinations = res == Print;

    lookForCombinations(sequence, printCombinations);
  }

  free(sequence.data);
  return 0;
}