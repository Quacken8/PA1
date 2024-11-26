#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define INITIAL_CAPACITY 32

#define INIT_ARRAY(type, array)        \
  (array).len = 0;                     \
  (array).capacity = INITIAL_CAPACITY; \
  (array).data = (type *)malloc(INITIAL_CAPACITY * sizeof(type));

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
    INIT_ARRAY(elementType, res);                                                         \
    return res;                                                                           \
  }                                                                                       \
  void push##elementType(arrWrapperName *arr, elementType newEl)                          \
  {                                                                                       \
    if (arr->len == arr->capacity)                                                        \
    {                                                                                     \
      arr->capacity *= 2;                                                                 \
      arr->data = (elementType *)realloc(arr->data, arr->capacity * sizeof(elementType)); \
    }                                                                                     \
    arr->data[arr->len++] = newEl;                                                        \
  }

typedef struct Entry
{
  char letter;
  bool marked;
} Entry;

DEFINE_ARR_HELPERS(Entry, PuzzleLine);

typedef struct PuzzleGrid
{
  size_t rowLen;
  PuzzleLine arr;
} PuzzleGrid;

Entry *getAt(PuzzleGrid puzzle, size_t x, size_t y)
{
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
  CoordinateArray *res = (CoordinateArray *)malloc(26 * sizeof(CoordinateArray));

  for (int i = 0; i < 26; i++)
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

LineReadRes readLine(CoordinateArray *letterMap[26], PuzzleGrid *puzzle, bool isFirstLine)
{
  char c;
  bool readAtLeastOneChar = false;

  while (true)
  {
    // NOLINTNEXTLINE
    if (scanf("%c", &c) != 1)
      return Error;

    if (c == '\n')
      break;

    readAtLeastOneChar = true;

    if ((c != '.') && (c < 'a' || c > 'z'))
    {
      return Error;
    }

    if (c != '.')
    {
      Coordinate coor = {
          .x = isFirstLine ? puzzle->arr.len : puzzle->arr.len % puzzle->rowLen,
          .y = isFirstLine ? 0 : puzzle->arr.len / puzzle->rowLen,
      };
      pushCoordinate(letterMap[c - 'a'], coor);
    }
    Entry newLetter = {
        .letter = c,
        .marked = false,
    };
    pushEntry(&puzzle->arr, newLetter);
  }

  if (!readAtLeastOneChar)
    return EndOfPuzzle;
  return Ok;
}

PuzzleReadRes readPuzzle()
{
  PuzzleGrid puzzle = {
      .rowLen = 0,
      .arr = newPuzzleLine(),
  };
  CoordinateArray *letterMap = newLetterMap();
  PuzzleReadRes res = {
      .success = false,
      .puzzle = puzzle,
      .letterMap = letterMap,
  };

  LineReadRes firstRead = readLine(&res.letterMap, &res.puzzle, true);
  if (firstRead != Ok)
  {
    return res; // TODO check how to handle empty input
  }

  res.puzzle.rowLen = res.puzzle.arr.len;

  while (true)
  {
    LineReadRes lineRead = readLine(&res.letterMap, &res.puzzle, false);
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
} QueryType;

DEFINE_ARR_HELPERS(char, ChArray);

typedef struct Query
{
  QueryType type;
  ChArray word;
} Query;

typedef struct QueryReadRes
{
  bool success;
  Query query;
} QueryReadRes;

QueryReadRes readQuery()
{
  char definingSymbol;
  ChArray letters = newChArray();
  Query query;
  QueryReadRes res = {
      .success = false,
      .query = query,
  };

  // NOLINTNEXTLINE
  if (scanf(" %c ", &definingSymbol) != 1)
  {
    free(letters.data);
    return res;
  }
  switch (definingSymbol)
  {
  case '-':
    query.type = CountAndMark;
    break;
  case '#':
    query.type = Count;
    break;
  case '?':
    query.type = Leftover;
    res.query = query;
    res.success = true;
    free(letters.data);
    return res;

  default:
    free(letters.data);
    return res;
  }

  while (true)
  {
    char c = getchar();
    if (c == '\n')
      break;
    if (!islower(c))
    {
      free(letters.data);
      return res;
    }

    pushchar(&letters, c);
  }

  if (letters.len > 1)
    res.success = true;
  query.word = letters;
  res.query = query;

  free(letters.data);
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
}

bool checkAlongDirection(bool mark, ChArray word, PuzzleGrid puzzle, Coordinate coordinate, int xDirection, int yDirection)
{
  // if the word wouldnt fit along the direction no need to check
  if (coordinate.x < -xDirection * word.len || coordinate.x + xDirection * word.len > puzzle.rowLen - 1) // TODO check that the type casting won't fuck you over
    return false;
  size_t colLen = puzzle.arr.len / puzzle.rowLen;
  if (coordinate.y < -yDirection * puzzle.rowLen * word.len || coordinate.y + yDirection * colLen * word.len > colLen - 1) // TODO check that the type casting won't fuck you over
    return false;

  bool match = true;
  for (size_t j = 1; j < word.len; j++)
  {
    char wordLetter = word.data[j];
    char puzzleLetter = getAt(puzzle, coordinate.x + xDirection * j, coordinate.y + yDirection * j)->letter;
    if (puzzleLetter != wordLetter)
    {
      match = false;
      break;
    }
  }

  if (mark && match)
  {
    for (size_t j = 0; j < word.len; j++)
    {
      getAt(puzzle, coordinate.x + xDirection * j, coordinate.y + yDirection * j)->marked = true;
    }
  }
  return match;
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

int main()
{

  printf("Osmismerka:\n");
  PuzzleReadRes puzzleRead = readPuzzle();
  if (!puzzleRead.success)
  {
    printf("Nespravny vstup.\n");
    for (int i = 0; i < 26; i++)
    {
      free(puzzleRead.letterMap[i].data);
    }
    free(puzzleRead.letterMap);
    free(puzzleRead.puzzle.arr.data);
    return 1;
  }

  for (int i = 0; i < 26; i++)
  {
    free(puzzleRead.letterMap[i].data);
  }
  free(puzzleRead.letterMap);
  free(puzzleRead.puzzle.arr.data);
  return 0;
}
