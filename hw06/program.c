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

typedef struct Entry
{
  char letter;
  bool marked;
} Entry;

DEFINE_ARR_HELPERS(Entry, EntryArr);

typedef struct PuzzleGrid
{
  size_t rowLen;
  EntryArr arr;
} PuzzleGrid;

/**
 * @param buildMode if true it is assumed that you may be accessing elements of a yet unfinished line of the puzzle
 * @returns a pointer at an entry in the puzzle or NULL if the coordinates are out of bounds
 */
Entry *getAt(PuzzleGrid puzzle, size_t x, size_t y, bool buildMode)
{

  bool outOfBounds;
  if (buildMode)
  {
    if (puzzle.rowLen == 0)
      outOfBounds = x > puzzle.arr.len || y != 0;
    else
    {
      outOfBounds = x >= puzzle.rowLen || y >= (puzzle.arr.len + puzzle.rowLen - 1) / puzzle.rowLen;
    }
  }
  else
    outOfBounds = x >= puzzle.rowLen || (y >= puzzle.arr.len / puzzle.rowLen);

  if (outOfBounds)
  {
    return NULL;
  }
  size_t index = y * puzzle.rowLen + x;
  return &puzzle.arr.data[index];
}

typedef enum
{
  ONE = 1,
  ZERO = 0,
  M_ONE = -1,
} DirComponent;

typedef struct LetterPair
{
  size_t xIndex;
  size_t yIndex;
  DirComponent xDir;
  DirComponent yDir;
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

void addLetterPairs(char thisLetter, size_t xIndex, size_t yIndex, PuzzleGrid puzzle, LetterPairArray *letterMap)
{

  // WEST
  Entry *otherLetter = getAt(puzzle, xIndex - 1, yIndex, true);
  if (otherLetter != NULL && otherLetter->letter != '.')
  {
    LetterPair newFrontPair = {
        .xIndex = xIndex,
        .yIndex = yIndex,
        .xDir = M_ONE,
        .yDir = ZERO,
    };
    pushLetterPair(getLetterPairs(thisLetter, otherLetter->letter, letterMap), newFrontPair);
    LetterPair newBackPair = {
        .xIndex = xIndex - 1,
        .yIndex = yIndex,
        .xDir = ONE,
        .yDir = ZERO,
    };
    pushLetterPair(getLetterPairs(otherLetter->letter, thisLetter, letterMap), newBackPair);
  }
  // NORTH WEST
  otherLetter = getAt(puzzle, xIndex - 1, yIndex - 1, true);
  if (otherLetter != NULL && otherLetter->letter != '.')
  {
    LetterPair newFrontPair = {
        .xIndex = xIndex,
        .yIndex = yIndex,
        .xDir = M_ONE,
        .yDir = M_ONE,
    };
    pushLetterPair(getLetterPairs(thisLetter, otherLetter->letter, letterMap), newFrontPair);
    LetterPair newBackPair = {
        .xIndex = xIndex - 1,
        .yIndex = yIndex - 1,
        .xDir = ONE,
        .yDir = ONE,
    };
    pushLetterPair(getLetterPairs(otherLetter->letter, thisLetter, letterMap), newBackPair);
  }

  // NORTH
  otherLetter = getAt(puzzle, xIndex, yIndex - 1, true);
  if (otherLetter != NULL && otherLetter->letter != '.')
  {
    LetterPair newFrontPair = {
        .xIndex = xIndex,
        .yIndex = yIndex,
        .xDir = ZERO,
        .yDir = M_ONE,
    };
    pushLetterPair(getLetterPairs(thisLetter, otherLetter->letter, letterMap), newFrontPair);
    LetterPair newBackPair = {
        .xIndex = xIndex,
        .yIndex = yIndex - 1,
        .xDir = ZERO,
        .yDir = ONE,
    };
    pushLetterPair(getLetterPairs(otherLetter->letter, thisLetter, letterMap), newBackPair);
  }

  // NORTH EAST
  otherLetter = getAt(puzzle, xIndex + 1, yIndex - 1, true);
  if (otherLetter != NULL && otherLetter->letter != '.')
  {
    LetterPair newFrontPair = {
        .xIndex = xIndex,
        .yIndex = yIndex,
        .xDir = ONE,
        .yDir = M_ONE,
    };
    pushLetterPair(getLetterPairs(thisLetter, otherLetter->letter, letterMap), newFrontPair);
    LetterPair newBackPair = {
        .xIndex = xIndex + 1,
        .yIndex = yIndex - 1,
        .xDir = M_ONE,
        .yDir = ONE,
    };
    pushLetterPair(getLetterPairs(otherLetter->letter, thisLetter, letterMap), newBackPair);
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
  size_t lenBefore = puzzle->arr.len;
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
      size_t xIndex = isFirstLine ? puzzle->arr.len : puzzle->arr.len % puzzle->rowLen;
      size_t yIndex = isFirstLine ? 0 : puzzle->arr.len / puzzle->rowLen;
      addLetterPairs(c, xIndex, yIndex, *puzzle, letterMap);
    }

    Entry newLetter = {
        .letter = c,
        .marked = false,
    };
    pushEntry(&puzzle->arr, newLetter);
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

  if (!isFirstLine && puzzle->arr.len - lenBefore != puzzle->rowLen)
    return Error;

  return Ok;
}

PuzzleReadRes readPuzzle()
{
  PuzzleGrid puzzle = {
      .rowLen = 0,
      .arr = newEntryArr(),
  };
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

  res.puzzle.rowLen = res.puzzle.arr.len;

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

  for (size_t i = 0; i < puzzle.arr.len; i++)
  {
    Entry e = puzzle.arr.data[i];
    if (!e.marked && e.letter != '.')
    {
      printf("%c", e.letter);
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
  int xDirection = letterPair.xDir;
  int yDirection = letterPair.yDir;

  //  if the word wouldnt fit along the direction no need to check
  Entry *lastLetter = getAt(puzzle, xIndex + xDirection * (word.len - 1), yIndex + yDirection * (word.len - 1), false);
  if (lastLetter == NULL || lastLetter->letter != word.data[word.len - 1])
    return false;

  for (size_t j = 2; j < word.len - 1; j++)
  {
    char wordLetter = word.data[j];
    Entry *e = getAt(puzzle, xIndex + xDirection * j, yIndex + yDirection * j, false);
    if (e == NULL || e->letter != wordLetter)
    {
      return false;
    }
  }

  if (mark)
  {
    for (size_t j = 0; j < word.len; j++)
    {
      getAt(puzzle, xIndex + xDirection * j, yIndex + yDirection * j, false)->marked = true;
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
  free(readPuzzle.puzzle.arr.data);
  return 1;
}

int main()
{

  printf("Osmismerka:\n");
  PuzzleReadRes puzzleRead = readPuzzle();
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
      free(puzzleRead.puzzle.arr.data);
      free(word.data);
      return 0;
    case ReadError:
      free(word.data);
      return error(puzzleRead);
    }
  }
}
