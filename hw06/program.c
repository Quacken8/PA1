#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define INITIAL_CAPACITY 32

/**
 * ## Will crash the program if allocation fails
 *
 * Defines a dynamic array struct and a `new` and a `push`
 * functions that handle memory allocation
 * @param elementType type of the underlying array
 * @param arrWrapperName name for the new struct containing the array
 *
 */
#define DEFINE_ARR_HELPERS(elementType, arrWrapperName)                                   \
  typedef struct arrWrapperName                                                           \
  {                                                                                       \
    size_t len;                                                                           \
    size_t capacity;                                                                      \
    elementType *data;                                                                    \
  } arrWrapperName;                                                                       \
  arrWrapperName new##arrWrapperName()                                                    \
  {                                                                                       \
    arrWrapperName res;                                                                   \
    res.len = 0;                                                                          \
    res.capacity = INITIAL_CAPACITY;                                                      \
    res.data = (elementType *)malloc(INITIAL_CAPACITY * sizeof(elementType));             \
    if (res.data == NULL)                                                                 \
    {                                                                                     \
      printf("Allocation error!!\n");                                                     \
      exit(1);                                                                            \
    }                                                                                     \
    return res;                                                                           \
  }                                                                                       \
  void push##elementType(arrWrapperName *arr, elementType newEl)                          \
  {                                                                                       \
    if (arr->len == arr->capacity)                                                        \
    {                                                                                     \
      arr->capacity *= 2;                                                                 \
      arr->data = (elementType *)realloc(arr->data, arr->capacity * sizeof(elementType)); \
      if (arr->data == NULL)                                                              \
      {                                                                                   \
        printf("Allocation error!!\n");                                                   \
        exit(1);                                                                          \
      }                                                                                   \
    }                                                                                     \
    arr->data[arr->len++] = newEl;                                                        \
  }

size_t min(size_t a, size_t b)
{
  return a < b ? a : b;
}
size_t max(size_t a, size_t b)
{
  return a > b ? a : b;
}

typedef struct Entry
{
  char letter;
  bool marked;
} Entry;

DEFINE_ARR_HELPERS(Entry, EntryArr);

DEFINE_ARR_HELPERS(char, ChArray);

void printCharray(ChArray arr)
{
  for (size_t i = 0; i < arr.len; i++)
  {
    printf("%c", arr.data[i]);
  }
}

/**
 * Insterts `c` at `arr[index]`, appending it at the end if `index > arr.len`
 */
void changeOrPush(ChArray *arr, char c, size_t index)
{
  if (index < arr->len)
    arr->data[index] = c;
  else
    pushchar(arr, c);
}

DEFINE_ARR_HELPERS(bool, BoolArray);

typedef struct PuzzleGrid
{
  size_t rowLen;
  ChArray westEast;
  ChArray northSouth;
  ChArray russiaBrazil;
  ChArray canadaAustralia;
  BoolArray markedWestEast;
} PuzzleGrid;

void freePuzzle(PuzzleGrid puzzle)
{
  free(puzzle.canadaAustralia.data);
  free(puzzle.russiaBrazil.data);
  free(puzzle.markedWestEast.data);
  free(puzzle.northSouth.data);
  free(puzzle.westEast.data);
}

typedef struct OptionChar
{
  char c;
  bool isSome;
} OptionChar;

OptionChar getLetterAt(PuzzleGrid puzzle, size_t x, size_t y, bool buildMode)
{
  OptionChar res;

  bool outOfBounds;
  if (buildMode)
  {
    if (puzzle.rowLen == 0)
      outOfBounds = x > puzzle.westEast.len || y != 0;
    else
    {
      outOfBounds = x >= puzzle.rowLen || y >= (puzzle.westEast.len + puzzle.rowLen - 1) / puzzle.rowLen;
    }
  }
  else
    outOfBounds = x >= puzzle.rowLen || (y >= puzzle.westEast.len / puzzle.rowLen);

  if (outOfBounds)
  {
    res.isSome = false;
    return res;
  }
  res.isSome = true;
  size_t index = y * puzzle.rowLen + x;
  res.c = puzzle.westEast.data[index];
  return res;
}

typedef enum
{
  WEST_EAST,
  NORTH_SOUTH,
  RUSSIA_BRAZIL,
  CANADA_AUSTRALIA,
} Direction;

typedef struct LetterPair
{
  size_t xIndex;
  size_t yIndex;
  Direction dir;
  bool backwards;
} LetterPair;

DEFINE_ARR_HELPERS(LetterPair, LetterPairArray);

LetterPairArray *newLetterMap()
{
  size_t len = ('z' - 'a' + 1) * ('z' - 'a' + 1);
  LetterPairArray *res = (LetterPairArray *)malloc(len * sizeof(LetterPairArray));
  if (res == NULL)
  {
    printf("Allocation error!!\n");
    exit(1);
  }

  for (size_t i = 0; i < len; i++)
  {
    res[i] = newLetterPairArray();
  }

  return res;
}

void freeLetterMap(LetterPairArray *letterMap)
{
  for (int i = 0; i < ('z' - 'a' + 1) * ('z' - 'a' + 1); i++)
  {
    free(letterMap[i].data);
  }
  free(letterMap);
}

LetterPairArray *getLetterPairs(char first, char second, LetterPairArray *letterMap)
{
  size_t firstIndex = first - 'a';
  size_t secondIndex = second - 'a';
  size_t numOfLetters = ('z' - 'a' + 1);
  return &letterMap[firstIndex * numOfLetters + secondIndex];
}

/**
 * Given a newly added letter and its position in the puzzle this adds it as a pair with its northwestern neighbors to the letter map
 * @param thisLetter letter you're currently on
 * @param xIndex x position of it in the grid
 * @param yIndex y position of it in the grid
 * @param puzzle the puzzle grid
 * @param letterMap the letter map the new pair gets appended to
 */
void addLetterPairs(char thisLetter, size_t xIndex, size_t yIndex, PuzzleGrid puzzle, LetterPairArray *letterMap)
{
  // WEST
  OptionChar otherLetter = getLetterAt(puzzle, xIndex - 1, yIndex, true);
  if (otherLetter.isSome && otherLetter.c != '.')
  {
    LetterPair newFrontPair = {
        .xIndex = xIndex,
        .yIndex = yIndex,
        .dir = WEST_EAST,
        .backwards = true,
    };
    pushLetterPair(getLetterPairs(thisLetter, otherLetter.c, letterMap), newFrontPair);
    LetterPair newBackPair = {
        .xIndex = xIndex - 1,
        .yIndex = yIndex,
        .dir = WEST_EAST,
        .backwards = false,
    };
    pushLetterPair(getLetterPairs(otherLetter.c, thisLetter, letterMap), newBackPair);
  }
  otherLetter = getLetterAt(puzzle, xIndex - 1, yIndex - 1, true);
  if (otherLetter.isSome && otherLetter.c != '.')
  {
    LetterPair newFrontPair = {
        .xIndex = xIndex,
        .yIndex = yIndex,
        .dir = CANADA_AUSTRALIA,
        .backwards = true,
    };
    pushLetterPair(getLetterPairs(thisLetter, otherLetter.c, letterMap), newFrontPair);
    LetterPair newBackPair = {
        .xIndex = xIndex - 1,
        .yIndex = yIndex - 1,
        .dir = CANADA_AUSTRALIA,
        .backwards = false,
    };
    pushLetterPair(getLetterPairs(otherLetter.c, thisLetter, letterMap), newBackPair);
  }
  otherLetter = getLetterAt(puzzle, xIndex, yIndex - 1, true);
  if (otherLetter.isSome && otherLetter.c != '.')
  {
    LetterPair newFrontPair = {
        .xIndex = xIndex,
        .yIndex = yIndex,
        .dir = NORTH_SOUTH,
        .backwards = true,
    };
    pushLetterPair(getLetterPairs(thisLetter, otherLetter.c, letterMap), newFrontPair);
    LetterPair newBackPair = {
        .xIndex = xIndex,
        .yIndex = yIndex - 1,
        .dir = NORTH_SOUTH,
        .backwards = false,
    };
    pushLetterPair(getLetterPairs(otherLetter.c, thisLetter, letterMap), newBackPair);
  }

  otherLetter = getLetterAt(puzzle, xIndex + 1, yIndex - 1, true);
  if (otherLetter.isSome && otherLetter.c != '.')
  {
    LetterPair newFrontPair = {
        .xIndex = xIndex,
        .yIndex = yIndex,
        .dir = RUSSIA_BRAZIL,
        .backwards = true,
    };
    pushLetterPair(getLetterPairs(thisLetter, otherLetter.c, letterMap), newFrontPair);
    LetterPair newBackPair = {
        .xIndex = xIndex + 1,
        .yIndex = yIndex - 1,
        .dir = RUSSIA_BRAZIL,
        .backwards = false,
    };
    pushLetterPair(getLetterPairs(otherLetter.c, thisLetter, letterMap), newBackPair);
  }
}

typedef struct PuzzleReadRes
{
  bool success;
  PuzzleGrid puzzle;
  LetterPairArray *letterMap;
} PuzzleReadRes;

typedef enum LineReadRes
{
  EndOfPuzzle,
  Error,
  Ok,
} LineReadRes;

/**
 * Reads one puzzle line from stdin. Only legal chars are lowercase letters, `.` and a newline iff
 * it will result in a nonempty rectangular puzzle grid
 * @param letterMap a `CoordinateArray` that has exactly enough space for all lowercase letters.
 * Each read letter's coordinates in the puzzle will be recorded
 * @param puzzle a `PuzzleGrid` to which to save the read chars
 * @param isFirstLine whether this is the first line being read (i.e. whether we don't yet know what
 * the length of a row will be)
 *
 * @returns
 *  -  `EndOfPuzzle` - read an empty line
 *
 *  -  `Error` - read an illegal symbol or the resulting puzzle will not be an unempty rectangular grid
 *
 *  -  `Ok` - line was read correctly
 */
LineReadRes readLine(LetterPairArray letterMap[], PuzzleGrid *puzzle, bool isFirstLine)
{
  bool readAtLeastOneChar = false;
  size_t lenBefore = puzzle->westEast.len;
  while (true)
  {
    char c = getchar();

    if (c == '\n')
      break;

    readAtLeastOneChar = true;

    if ((c != '.') && !islower(c))
    {
      return Error;
    }

    if (c != '.')
    {
      size_t xIndex = isFirstLine ? puzzle->westEast.len : puzzle->westEast.len % puzzle->rowLen;
      size_t yIndex = isFirstLine ? 0 : puzzle->westEast.len / puzzle->rowLen;
      addLetterPairs(c, xIndex, yIndex, *puzzle, letterMap);
    }

    pushchar(&puzzle->westEast, c);
    pushbool(&puzzle->markedWestEast, false);
  }

  if (!readAtLeastOneChar)
  {
    if (isFirstLine)
    {
      return Error;
    }
    else
    {
      return EndOfPuzzle;
    }
  }

  if (!isFirstLine && puzzle->westEast.len - lenBefore != puzzle->rowLen)
    return Error;

  return Ok;
}

typedef struct Coordinate
{
  size_t x;
  size_t y;
} Coordinate;

size_t getIndexOfRotated(Direction rotation, size_t rowLen, size_t colLen, size_t x, size_t y)
{
  size_t diagonal;

  switch (rotation)
  {
  case WEST_EAST:
    return x + rowLen * y;
  case NORTH_SOUTH:
    return y + colLen * x;
  case RUSSIA_BRAZIL:
    diagonal = x + y;
    break;
  case CANADA_AUSTRALIA:
    diagonal = (rowLen - x - 1) + y;
    break;
  default:
    diagonal = 0;
  }
  size_t bigDimension = max(rowLen, colLen);
  size_t smallDimension = min(rowLen, colLen);

  // ⎛ \ \ \ \ ⎞
  // ⎜ . \ \ \ ⎟
  // ⎜ . . \ \ ⎟
  // ⎜ . . . \ ⎟
  // ⎜ . . . . ⎟
  // ⎜ . . . . ⎟
  // ⎝ . . . . ⎠
  size_t index = min(diagonal * (diagonal + 1), (smallDimension * (smallDimension + 1))) / 2;

  // ⎛ . . . . ⎞
  // ⎜ \ . . . ⎟
  // ⎜ \ \ . . ⎟
  // ⎜ \ \ \ . ⎟
  // ⎜ . \ \ \ ⎟
  // ⎜ . . \ \ ⎟
  // ⎝ . . . \ ⎠

  if (diagonal > smallDimension)
    index += min(diagonal - smallDimension, bigDimension - smallDimension) * smallDimension;

  // ⎛ . . . . ⎞
  // ⎜ . . . . ⎟
  // ⎜ . . . . ⎟
  // ⎜ . . . . ⎟
  // ⎜ \ . . . ⎟
  // ⎜ \ \ . . ⎟
  // ⎝ \ \ \ . ⎠

  if (diagonal > bigDimension)
    index += (smallDimension * (smallDimension - 1) - (bigDimension + smallDimension - diagonal) * (bigDimension + smallDimension - diagonal - 1)) / 2;

  // ⎛ . . . . ⎞
  // ⎜ . . . . ⎟
  // ⎜ . . . . ⎟
  // ⎜ \ . . . ⎟
  // ⎜ . \ . . ⎟
  // ⎜ . . \ . ⎟
  // ⎝ . . . . ⎠
  if (rotation == CANADA_AUSTRALIA)
    index += min(x, y);
  else
    index += min(rowLen - x - 1, y);

  return index;
}

/**
 * To a finished west-east array in a puzzle adds the three remaining rotations
 */
void addRotatedGrids(PuzzleGrid *puzzle)
{
  if (puzzle->westEast.len == 0 || puzzle->rowLen == 0)
    return;
  ChArray northSouth = {
      .len = puzzle->westEast.len,
      .capacity = puzzle->westEast.len,
      .data = (char *)malloc(puzzle->westEast.len * sizeof(char)),
  };

  size_t colLen = puzzle->westEast.len / puzzle->rowLen;
  size_t rowLen = puzzle->rowLen;

  ChArray russiaBrazil = {
      .len = puzzle->westEast.len,
      .capacity = puzzle->westEast.len,
      .data = (char *)malloc(puzzle->westEast.len * sizeof(char)),
  };
  ChArray canadaAustralia = {
      .len = puzzle->westEast.len,
      .capacity = puzzle->westEast.len,
      .data = (char *)malloc(puzzle->westEast.len * sizeof(char)),
  };

  for (size_t i = 0; i < puzzle->westEast.len; i++)
  {
    char c = puzzle->westEast.data[i];
    size_t x = i % rowLen;
    size_t y = i / rowLen;
    northSouth.data[getIndexOfRotated(NORTH_SOUTH, rowLen, colLen, x, y)] = c;
    russiaBrazil.data[getIndexOfRotated(RUSSIA_BRAZIL, rowLen, colLen, x, y)] = c;
    canadaAustralia.data[getIndexOfRotated(CANADA_AUSTRALIA, rowLen, colLen, x, y)] = c;
  }

  puzzle->canadaAustralia = canadaAustralia;
  puzzle->russiaBrazil = russiaBrazil;
  puzzle->northSouth = northSouth;
}

ChArray getRotatedGrid(PuzzleGrid puzzle, Direction rotation)
{
  switch (rotation)
  {
  case WEST_EAST:
    return puzzle.westEast;
  case NORTH_SOUTH:
    return puzzle.northSouth;
  case RUSSIA_BRAZIL:
    return puzzle.russiaBrazil;
  case CANADA_AUSTRALIA:
    return puzzle.canadaAustralia;
  default:
    // cant happen
    printf("Got unexpected enum value!\n");
    exit(1);
  }
}

PuzzleReadRes readPuzzle()
{
  PuzzleGrid puzzle;
  puzzle.rowLen = 0;
  puzzle.westEast = newChArray();
  puzzle.markedWestEast = newBoolArray();

  LetterPairArray *letterMap = newLetterMap();
  PuzzleReadRes res = {
      .success = false,
      .puzzle = puzzle,
      .letterMap = letterMap,
  };

  LineReadRes firstRead = readLine(res.letterMap, &res.puzzle, true);
  if (firstRead != Ok)
  {
    return res;
  }

  res.puzzle.rowLen = res.puzzle.westEast.len;

  while (true)
  {
    LineReadRes lineRead = readLine(res.letterMap, &res.puzzle, false);
    if (lineRead == Error)
    {
      return res;
    }
    if (lineRead == EndOfPuzzle)
      break;
  }

  res.success = true;
  return res;
}

typedef enum QueryType
{
  Count,
  CountAndMark,
  Leftover,
  ReadError,
  Eof,
} QueryType;

/**
 * Reads query from stdin and saves searched word
 *
 * @param word a `ChArray` to write the word in. The old data isn't purged, only `word.len` is
 * set to 0 and new data is written from the beginning
 *
 * @returns
 *  - `Count` - `#` was read; user wants to only find and count all occurances of a word
 *
 *  - `CountAndMark` - `-` was read; user wants to both count and mark all occurances of a word
 *
 *  - `Leftover` - `?` was read; user wants to print all unmarked letters
 *
 *  - `ReadError` - an illegal symbol was read
 *
 *  - `Eof` - end of input was reached
 */
QueryType readQuery(ChArray *word)
{

  char definingSymbol = getchar();
  if (definingSymbol == EOF)
  {
    return Eof;
  }
  QueryType res = ReadError;
  switch (definingSymbol)
  {
  case '-':
    res = CountAndMark;
    break;
  case '#':
    res = Count;
    break;
  case '?':
    definingSymbol = getchar();
    if (definingSymbol == '\n')
      return Leftover;
    return ReadError;
  default:
    return ReadError;
  }

  // skips all whitespaces
  scanf(" ");

  word->len = 0;
  size_t index = 0;
  while (true)
  {
    char c = getchar();
    if (c == '\n')
      break;
    if (!islower(c))
    {
      return ReadError;
    }

    changeOrPush(word, c, index++);
  }

  if (word->len <= 1)
    return ReadError;
  return res;
}

/**
 * Prints out all unmarked letters in `puzzle`
 */
void printLeftovers(PuzzleGrid puzzle)
{
  unsigned const LETTERS_PER_LINE = 60;

  unsigned printedLetters = 0;

  for (size_t i = 0; i < puzzle.westEast.len; i++)
  {
    bool marked = puzzle.markedWestEast.data[i];
    char c = puzzle.westEast.data[i];
    if (!marked && c != '.')
    {
      printf("%c", c);
      printedLetters++;
      if (printedLetters == LETTERS_PER_LINE)
      {
        printf("\n");
        printedLetters = 0;
      }
    }
  }
  if (printedLetters != 0)
    printf("\n");
}

/**
 * Checks whether a word is in a puzzle at a specific coordinate
 *
 * @param mark whether to mark the word if it is found
 * @param word a `ChArray` of the word you're looking for
 * @param puzzle the relevant `PuzzleGrid` in which to search. If `mark` is `true` and the word was found
 * the elements of this will be mutated (their `entry.marked` will be set to true)
 * @param letterPair matching letter pair of start of the word. It is expeced these two letters match
 *
 * @returns `true` if the word was found, `false` otherwise
 */
bool checkAlongDirection(bool mark, ChArray word, PuzzleGrid puzzle, LetterPair letterPair)
{

  size_t xIndex = letterPair.xIndex;
  size_t yIndex = letterPair.yIndex;
  Direction direction = letterPair.dir;
  ChArray arr = getRotatedGrid(puzzle, direction);
  size_t startingIndex = getIndexOfRotated(direction, puzzle.rowLen, puzzle.westEast.len / puzzle.rowLen, xIndex, yIndex);
  bool backwards = letterPair.backwards;

  int xDirection = direction == RUSSIA_BRAZIL ? -1 : direction == NORTH_SOUTH ? 0
                                                                              : 1;

  int yDirection = direction != WEST_EAST ? 1 : 0;

  if (backwards)
  {
    xDirection *= -1;
    yDirection *= -1;
  }

  //  if the word wouldnt fit along the direction no need to check
  OptionChar lastLetter = getLetterAt(puzzle, xIndex + xDirection * (word.len - 1), yIndex + yDirection * (word.len - 1), false);
  if (!lastLetter.isSome || lastLetter.c != word.data[word.len - 1])
    return false;

  for (size_t j = 2; j < word.len - 1; j++)
  {
    char wordLetter = word.data[j];
    char puzzleLetter = arr.data[startingIndex + (backwards ? -j : j)];
    if (puzzleLetter != wordLetter)
    {
      return false;
    }
  }

  if (mark)
  {

    for (size_t j = 0; j < word.len; j++)
    {
      puzzle.markedWestEast.data[xIndex + xDirection * j + (yIndex + yDirection * j) * puzzle.rowLen] = true;
    }
  }
  return true;
}

/**
 * Looks for all occurances of a word in a puzzle
 *
 * @param word the word that you look for
 * @param markFound whether to mark the word if found
 * @param puzzle the puzzle to look for the word in. Will be mutated if
 * the word is found and `markFound` is `true`
 * @param letterMap an array with one `CoordinateArray` for each lowercase
 * letter (sorted alphabetically) that contains ALL occurances of that letter in the puzzle
 *
 * @returns number of found occurances
 */
unsigned findWords(ChArray word, bool markFound, PuzzleGrid puzzle, LetterPairArray letterMap[])
{
  char firstLetter = word.data[0];
  char secondLetter = word.data[1];
  LetterPairArray *possibleStarts = getLetterPairs(firstLetter, secondLetter, letterMap);

  unsigned res = 0;

  for (size_t i = 0; i < possibleStarts->len; i++)
  {
    LetterPair start = possibleStarts->data[i];
    res += checkAlongDirection(markFound, word, puzzle, start);
  }

  return res;
}

/**
 * Frees data in input, prints error message and returns 1
 */
int error(PuzzleReadRes readPuzzle)
{
  printf("Nespravny vstup.\n");
  freeLetterMap(readPuzzle.letterMap);
  freePuzzle(readPuzzle.puzzle);
  return 1;
}

int main()
{

  printf("Osmismerka:\n");
  PuzzleReadRes puzzleRead = readPuzzle();
  addRotatedGrids(&puzzleRead.puzzle);
  if (!puzzleRead.success)
  {
    return error(puzzleRead);
  }

  ChArray word = newChArray();
  while (true)
  {
    QueryType query = readQuery(&word);
    bool mark = false;
    unsigned occurances;
    switch (query)
    {
    case CountAndMark:
      mark = true;
      // fall through
    case Count:
      occurances = findWords(word, mark, puzzleRead.puzzle, puzzleRead.letterMap);
      printCharray(word);
      printf(": %dx\n", occurances);
      break;
    case Leftover:
      printf("Tajenka:\n");
      printLeftovers(puzzleRead.puzzle);
      break;
    case Eof:
      freeLetterMap(puzzleRead.letterMap);
      freePuzzle(puzzleRead.puzzle);
      free(word.data);
      return 0;
    case ReadError:
      free(word.data);
      return error(puzzleRead);
    }
  }
}
