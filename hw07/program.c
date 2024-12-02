#include <stdio.h>
#include <stdlib.h>

#define INITIAL_CAPACITY 64

typedef struct ChArray
{
  size_t len;
  size_t capacity;
  char *data;
} ChArray;

void pushChar(ChArray arr, char e)
{
  if (arr.capacity == arr.len)
  {
    arr.capacity *= 2;
    arr.data = (char *)realloc(arr.data, arr.capacity * sizeof(char));
  }
  arr.data[arr.len++] = e;
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

int main()
{

  return 0;
}