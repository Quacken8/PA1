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

/**
 * @brief Pushes character at the end of the array, reallocating it if neccessary
 */
void pushChar(ChArray *arr, char e)
{
  if (arr->capacity == arr->len)
  {
    arr->capacity *= 2;
    arr->data = (char *)realloc(arr->data, arr->capacity * sizeof(char));
  }
  arr->data[arr->len++] = e;
}

/**
 * @brief Creates an empty array
 */
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

/**
 * @returns the index of the underlying ChArray at which the digit is
 */
size_t digitToUnderlyingIndex(size_t digit, BigNum num)
{
  return 2 * (num.biggestDigit + digit);
}

/**
 * @returns the smaller of the arguments
 */
size_t min(size_t a, size_t b)
{
  return a < b ? a : b;
}

/**
 * @returns nth largest (i.e. leftmost) digit
 */
char nthLargestDigit(BigNum num, size_t n)
{
  return num.underlyingData[digitToUnderlyingIndex(n, num)];
}

/**
 * @brief splits input into two BigNums
 * @param num original number to be split
 * @param digit digit (counted from the left) after which the split should happen
 * @param left output param, will be overwritten with the left side of the original number after the split
 * @param right output param, will be overwritten with the right side of the original number after the split
 * @warning doesn't check whether `digit <= totalDigits(num)`
 */
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
 * @returns a <= b
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

bool isOddChar(char c)
{
  int asNumber = c - '0';
  return asNumber % 2;
}

bool isOdd(BigNum num)
{
  return isOddChar(nthLargestDigit(num, totalDigits(num) - 1));
}

/**
 * @returns true iff such a splitting follows the vault rules
 * @warning only makes sense for `left` and `right` numbers to be neighbouring in a larger sequence
 */
bool isValidSplit(BigNum left, BigNum right)
{
  if (isOdd(left))
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
 * @brief Reads a query symbol (? or #) and sequence of numbers from stdin, adding space for separators in
 * @returns `ReadRes` enum that characterizes the input:
 *
 * - `Error` -- wrong input
 *
 * - `Eof` -- end of input reached
 *
 * - `Num` -- query included '#'; only number of combinations is required
 *
 * - `Print` -- query included '?'; each found valid combination is to be printed
 *
 * @warning overwrites data in the input buffer
 */
ReadRes readInput(ChArray *buffer)
{
  buffer->len = 0;
  char definingSybmol = getchar();
  if (definingSybmol == EOF)
    return Eof;
  if (definingSybmol != '?' && definingSybmol != '#')
    return Error;

  // NOLINTNEXTLINE -- skips all empty chars. The linter warning is irrelevant, because no data is saved
  scanf(" ");

  while (true)
  {
    char digit = getchar();

    if (digit == '\n')
      break;

    if (!isdigit(digit))
      return Error;

    pushChar(buffer, digit);
    pushChar(buffer, ' ');
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
    if (sequence.data[i] != ' ')
      printf("%c", sequence.data[i]);
  }
  printf("\n");
}

/**
 * @brief recursively finds (and optionally prints) all valid splittings of `slice` based on it's left neighbor `prevLeft`
 * @param slice the number to look for combinations in
 * @param prevLeft `slice`'s neighbour to the left
 * @param printCombinations whether to print each found combination to stdout.
 * Prints the whole sequence, not just that related to input `slice`
 * @param original ChArray containing the entire underlying string of numbers and separators
 * @returns number of found valid combinations
 */
unsigned long long countSubcombinations(BigNum slice, BigNum prevLeft, bool printCombinations, ChArray original)
{
  unsigned long long res = 0;
  size_t startingDigit = 0;
  if (!isOdd(prevLeft))
  {
    // skip all splittings that would end up having an even number neccessarily followed by a smaller number
    size_t prevSignificantDigits = totalDigits(prevLeft) - prevLeft.leadingZeroes;
    if (prevSignificantDigits > 0)
      startingDigit = slice.leadingZeroes + prevSignificantDigits - 1;
  }

  for (size_t i = startingDigit; i < totalDigits(slice) - 1; i++)
  {
    BigNum left;
    BigNum right;
    splitAfterDigit(slice, i, &left, &right);
    if (!isValidSplit(prevLeft, left) || !isValidSplit(left, right))
      continue;

    res++;

    if (printCombinations)
    {
      original.data[digitToUnderlyingIndex(i, slice) + 1] = ','; // +1 to target char *after* the digit (i.e. the separator)
      printSequence(original);
    }
    if (totalDigits(right) > 1)
      res += countSubcombinations(right, left, printCombinations, original);

    if (printCombinations)
    {
      original.data[digitToUnderlyingIndex(i, slice) + 1] = ' ';
    }
  }

  return res;
}

/**
 * @brief initiates recursive search for combinations
 * @param original the underlying data including digits and spaces for separators
 * @param printCombinations whether to print out all found combinations and not just their total number
 * @returns void, all output goes directly to stdout
 */
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

  // initiate the sequence with smallest possible neighbor to the left
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