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

typedef struct Field
{
  char letter;
  bool marked;
} Field;

DEFINE_ARR_HELPERS(Field, PuzzleRow);

DEFINE_ARR_HELPERS(PuzzleRow, PuzzleGrid);

void freePuzzleGrid(PuzzleGrid *arr)
{
  for (size_t i = 0; i < arr->len; i++)
  {
    free(arr->data[i].data);
  }
  free(arr->data);
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

typedef struct LineReadRes
{
  bool succes;
  PuzzleRow line;
} LineReadRes;

LineReadRes readLine(CoordinateArray *letterMap[26], size_t rowIndex)
{
  char c;
  LineReadRes res = {
      .succes = false,
      .line = newPuzzleRow(),
  };

  while (true)
  {
    // NOLINTNEXTLINE
    if (scanf("%c", &c) != 1)
      return res;

    if (c == '\n')
      break;

    if ((c != '.') && (c < 'a' || c > 'z'))
    {
      return res;
    }

    if (c != '.')
    {
      Coordinate coor = {
          .x = res.line.len,
          .y = rowIndex,
      };
      pushCoordinate(letterMap[c - 'a'], coor);
    }
    Field newLetter = {
        .letter = c,
        .marked = false,
    };
    pushField(&res.line, newLetter);
  }

  res.succes = true;

  return res;
}

PuzzleReadRes readPuzzle()
{
  PuzzleReadRes res = {
      .success = false,
      .puzzle = newPuzzleGrid(),
      .letterMap = newLetterMap(),
  };
  size_t rowIndex = 0;
  LineReadRes firstLine = readLine(&res.letterMap, rowIndex++);
  pushPuzzleRow(&res.puzzle, firstLine.line);

  if (!firstLine.succes || firstLine.line.len == 0)
  {
    return res;
  }

  size_t rowLen = firstLine.line.len;

  while (true)
  {
    LineReadRes readRes = readLine(&res.letterMap, rowIndex++);
    PuzzleRow newLine = readRes.line;

    if (!readRes.succes)
    {
      free(readRes.line.data);
      return res;
    }

    if (newLine.len == 0)
    {
      free(readRes.line.data);
      break;
    }

    if (newLine.len != rowLen)
    {
      free(readRes.line.data);
      return res;
    }
    pushPuzzleRow(&res.puzzle, newLine);
    free(readRes.line.data);
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

int main()
{

  PuzzleReadRes puzl = readPuzzle();

  freePuzzleGrid(&puzl.puzzle);
  for (int i = 0; i < 26; i++)
  {
    free(puzl.letterMap[i].data);
  }
  free(puzl.letterMap);
  return 0;
}
