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

typedef struct BigNum
{
  char *underlyingData;
  size_t biggestDigit;
  size_t smallestDigit;
  size_t leadingZeroes;
} BigNum;

size_t totalDigits(BigNum num)
{
  return num.smallestDigit - num.biggestDigit + 1;
}

size_t digitToUnderlyingIndex(size_t digit, BigNum num)
{
  return 2 * (num.biggestDigit + digit);
}

size_t min(size_t a, size_t b)
{
  return a < b ? a : b;
}

size_t max(size_t a, size_t b)
{
  return a > b ? a : b;
}

char nthLargestDigit(BigNum num, size_t n)
{
  return num.underlyingData[digitToUnderlyingIndex(n, num)];
}

void splitAfterDigit(BigNum num, size_t digit, BigNum *left, BigNum *right)
{

  left->underlyingData = num.underlyingData;
  left->biggestDigit = num.biggestDigit;
  left->smallestDigit = num.biggestDigit + digit;
  left->leadingZeroes = min(num.leadingZeroes, digit + 1);

  right->underlyingData = num.underlyingData;
  right->biggestDigit = num.biggestDigit + digit + 1;
  right->smallestDigit = num.smallestDigit;
  right->leadingZeroes = num.leadingZeroes - left->leadingZeroes;

  // check the leading zeroes of right tho
  if (right->leadingZeroes == 0)
  {
    for (size_t i = 0; i < totalDigits(*right); i++)
    {
      if (nthLargestDigit(*right, i) != '0')
        break;
      right->leadingZeroes++;
    }
  }
}

/**
 * returns a ≤ b
 */
bool isLEq(BigNum a, BigNum b)
{
  size_t aSignificantDigits = totalDigits(a) - a.leadingZeroes;
  size_t bSignificantDigits = totalDigits(b) - b.leadingZeroes;

  if (aSignificantDigits != bSignificantDigits)
    return aSignificantDigits < bSignificantDigits;

  for (size_t i = a.leadingZeroes, j = b.leadingZeroes; i < totalDigits(a); i++, j++)
  {
    char aDigit = nthLargestDigit(a, i);
    char bDigit = nthLargestDigit(b, j);
    if (aDigit != bDigit)
      return aDigit < bDigit;
  }
  return true;
}

bool isOdd(char c)
{
  unsigned asNumber = c - '0';
  return asNumber % 2;
}

bool isValidSplit(BigNum left, BigNum right)
{
  if (isOdd(nthLargestDigit(left, totalDigits(left) - 1)))
    return true;

  return isLEq(left, right);
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
    pushChar(buffer, '|');
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
    if (sequence.data[i] != '|')
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

unsigned long long countSubcombinations(BigNum slice, BigNum prevLeft, bool printCombinations, ChArray original)
{
  unsigned long long res = 0;
  for (size_t i = 0; i < totalDigits(slice) - 1; i++)
  {
    BigNum left;
    BigNum right;
    splitAfterDigit(slice, i, &left, &right);
    if (!isValidSplit(prevLeft, left) || !isValidSplit(left, right))
      continue;

    res++;

    if (printCombinations)
    {
      original.data[digitToUnderlyingIndex(i, slice) + 1] = ',';
      printSequence(original);
    }
    if (totalDigits(right) > 1)
      res += countSubcombinations(right, left, printCombinations, original);

    if (printCombinations)
    {
      original.data[digitToUnderlyingIndex(i, slice) + 1] = '|';
    }
  }

  return res;
}

void lookForCombinations(ChArray original, bool printCombinations)
{

  unsigned long long leadingZeroes = 0;
  for (size_t i = 0; i < original.len; i += 2)
  {
    if (original.data[i] != '0')
      break;
    leadingZeroes++;
  }

  BigNum initialSequence;

  initialSequence.biggestDigit = 0;
  initialSequence.smallestDigit = (original.len - 1) / 2;
  initialSequence.leadingZeroes = leadingZeroes;
  initialSequence.underlyingData = original.data;

  char dummy[] = "0";
  BigNum prev;
  prev.leadingZeroes = 1;
  prev.biggestDigit = 0;
  prev.smallestDigit = 0;
  prev.underlyingData = dummy;

  if (printCombinations)
    printSequence(original);
  unsigned long long totalCombinations = countSubcombinations(initialSequence, prev, printCombinations, original) + 1;

  printf("Celkem: %lld\n", totalCombinations);
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
      printf("Nespravny vstup.\n");
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