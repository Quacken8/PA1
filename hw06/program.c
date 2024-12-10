#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define INITIAL_CAPACITY 32

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
 * @returns pointer at an entry in the puzzle or NULL if the coordinates are out of bounds
 */
Entry *getAt(PuzzleGrid puzzle, size_t x, size_t y)
{
  if (x >= puzzle.rowLen || y >= puzzle.arr.len / puzzle.rowLen)
  {
    return NULL;
  }
  size_t index = y * puzzle.rowLen + x;
  return &puzzle.arr.data[index];
}

typedef struct Coordinate
{
  size_t x;
  size_t y;
} Coordinate;

DEFINE_ARR_HELPERS(Coordinate, CoordinateArray);

CoordinateArray *newLetterMap()
{
  CoordinateArray *res = (CoordinateArray *)malloc(('z' - 'a' + 1) * sizeof(CoordinateArray));
  if (res == NULL)
  {
    printf("Allocation error!!\n");
    exit(1);
  }

  for (int i = 0; i < ('z' - 'a' + 1); i++)
  {
    res[i] = newCoordinateArray();
  }

  return res;
}

typedef struct PuzzleReadRes
{
  bool success;
  PuzzleGrid puzzle;
  CoordinateArray *letterMap;
} PuzzleReadRes;

typedef enum LineReadRes
{
  EndOfPuzzle,
  Error,
  Ok,
} LineReadRes;

LineReadRes readLine(CoordinateArray letterMap[26], PuzzleGrid *puzzle, bool isFirstLine)
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
      Coordinate coor = {
          .x = isFirstLine ? puzzle->arr.len : puzzle->arr.len % puzzle->rowLen,
          .y = isFirstLine ? 0 : puzzle->arr.len / puzzle->rowLen,
      };
      pushCoordinate(&letterMap[c - 'a'], coor);
    }
    Entry newLetter = {
        .letter = c,
        .marked = false,
    };
    pushEntry(&puzzle->arr, newLetter);
  }
  if (!readAtLeastOneChar)
    return EndOfPuzzle;
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
  CoordinateArray *letterMap = newLetterMap();
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

typedef enum QueryType
{
  Count,
  CountAndMark,
  Leftover,
  ReadError,
  Eof,
} QueryType;

DEFINE_ARR_HELPERS(char, ChArray);

void printCharray(ChArray arr)
{
  for (size_t i = 0; i < arr.len; i++)
  {
    printf("%c", arr.data[i]);
  }
}

void pushcharAt(ChArray *arr, char c, size_t index)
{
  if (index < arr->len)
    arr->data[index] = c;
  else
    pushchar(arr, c);
}

QueryType readQuery(ChArray *word)
{
  QueryType res = ReadError;

  char definingSymbol = getchar();
  if (definingSymbol == EOF)
  {
    res = Eof;
    return res;
  }

  switch (definingSymbol)
  {
  case '-':
    res = CountAndMark;
    scanf(" ");
    break;
  case '#':
    res = Count;
    scanf(" ");
    break;
  case '?':
    definingSymbol = getchar();
    if (definingSymbol == '\n')
      res = Leftover;
    else
      res = ReadError;
    return res;
  default:
    res = ReadError;
    return res;
  }

  word->len = 0;
  size_t index = 0;
  while (true)
  {
    char c = getchar();
    if (c == '\n')
      break;
    if (!islower(c))
    {
      res = ReadError;
      return res;
    }

    pushcharAt(word, c, index++);
  }

  if (word->len <= 1)
    res = ReadError;
  return res;
}

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

bool checkAlongDirection(bool mark, ChArray word, PuzzleGrid puzzle, Coordinate coordinate, int xDirection, int yDirection)
{
  //  if the word wouldnt fit along the direction no need to check
  Entry *lastLetter = getAt(puzzle, coordinate.x + xDirection * (word.len - 1), coordinate.y + yDirection * (word.len - 1));
  if (lastLetter == NULL || lastLetter->letter != word.data[word.len - 1])
    return false;

  for (size_t j = 1; j < word.len - 1; j++)
  {
    char wordLetter = word.data[j];
    Entry *e = getAt(puzzle, coordinate.x + xDirection * j, coordinate.y + yDirection * j);
    if (e == NULL || e->letter != wordLetter)
    {
      return false;
    }
  }

  if (mark)
  {
    for (size_t j = 0; j < word.len; j++)
    {
      getAt(puzzle, coordinate.x + xDirection * j, coordinate.y + yDirection * j)->marked = true;
    }
  }
  return true;
}

unsigned findWords(ChArray word, bool markFound, PuzzleGrid puzzle, CoordinateArray letterMap[])
{
  char firstLetter = word.data[0];
  CoordinateArray possibleStarts = letterMap[firstLetter - 'a'];

  unsigned res = 0;

  for (size_t i = 0; i < possibleStarts.len; i++)
  {
    Coordinate coordinate = possibleStarts.data[i];
    res += checkAlongDirection(markFound, word, puzzle, coordinate, 1, 0);
    res += checkAlongDirection(markFound, word, puzzle, coordinate, 1, 1);
    res += checkAlongDirection(markFound, word, puzzle, coordinate, 0, 1);
    res += checkAlongDirection(markFound, word, puzzle, coordinate, -1, 1);
    res += checkAlongDirection(markFound, word, puzzle, coordinate, -1, 0);
    res += checkAlongDirection(markFound, word, puzzle, coordinate, -1, -1);
    res += checkAlongDirection(markFound, word, puzzle, coordinate, 0, -1);
    res += checkAlongDirection(markFound, word, puzzle, coordinate, 1, -1);
  }

  return res;
}

int error(PuzzleReadRes readPuzzle)
{
  printf("Nespravny vstup.\n");
  for (int i = 0; i < 26; i++)
  {
    free(readPuzzle.letterMap[i].data);
  }
  free(readPuzzle.letterMap);
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
      for (int i = 0; i < 26; i++)
      {
        free(puzzleRead.letterMap[i].data);
      }
      free(puzzleRead.letterMap);
      free(puzzleRead.puzzle.arr.data);
      free(word.data);
      return 0;
    case ReadError:
      free(word.data);
      return error(puzzleRead);
    }
  }
}
