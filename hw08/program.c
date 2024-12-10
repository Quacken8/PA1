#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>

#define PHONE_DIGITS 10
#define nullptr NULL

typedef struct TNode
{
  char *m_Name;
  struct TNode *m_Child[PHONE_DIGITS];
} TNODE;

typedef struct
{
  TNODE *m_Root;
  int m_Size;
} TPHONEBOOK;

#endif /* __PROGTEST__ */

TNODE *emptyNode()
{
  TNODE *res = (TNODE *)malloc(sizeof(TNODE));
  res->m_Name = nullptr;
  for (int i = 0; i < PHONE_DIGITS; i++)
  {
    res->m_Child[i] = nullptr;
  }

  return res;
}

bool addPhone(TPHONEBOOK *book,
              const char *phone,
              const char *name)
{
  char digit;
  size_t phoneLen = 0;
  size_t buffer[PHONE_DIGITS];
  while ((digit = phone[phoneLen]))
  {
    if (!isdigit(digit) || phoneLen > PHONE_DIGITS)
    {
      return false;
    }
    size_t asNumber = digit - '0';
    buffer[phoneLen++] = asNumber;
  }

  bool anyNew = false;
  TNODE **node = &book->m_Root;

  for (size_t i = 0; i < phoneLen; i++)
  {
    if (*node == nullptr)
    {
      anyNew = true;
      *node = emptyNode();
    }
    node = &(*node)->m_Child[buffer[i]];
  }
  if (*node == nullptr)
  {
    anyNew = true;
    *node = emptyNode();
  }
  if (anyNew)
    book->m_Size++;

  (*node)->m_Name = (char *)realloc((*node)->m_Name, (strlen(name) + 1) * sizeof(char));
  strcpy((*node)->m_Name, name);

  return true;
}

void freeNodeRec(TNODE *node)
{
  if (node == nullptr)
    return;

  for (int i = 0; i < PHONE_DIGITS; i++)
  {
    freeNodeRec(node->m_Child[i]);
  }

  free(node->m_Name);
  free(node);
}

void delBook(TPHONEBOOK *book)
{
  freeNodeRec(book->m_Root);
}

bool isChildless(TNODE *node)
{
  for (size_t i = 0; i < PHONE_DIGITS; i++)
  {
    if (node->m_Child[i] != nullptr)
      return false;
  }

  return true;
}

bool delPhoneRec(TNODE **node, const char *phone, int phoneIndex)
{
  if (phone[phoneIndex] == '\0')
  {
    free((*node)->m_Name);
    (*node)->m_Name = nullptr;
    if (isChildless(*node))
    {
      free(*node);
      *node = nullptr;
    }
    return true;
  }
  char digit = phone[phoneIndex];
  if (!isdigit(digit) || phoneIndex == PHONE_DIGITS)
    return false;

  int asNumber = digit - '0';

  TNODE **child = &(*node)->m_Child[asNumber];
  if (*child == nullptr)
    return false;

  bool smthDeleted = delPhoneRec(child, phone, phoneIndex + 1);

  if (smthDeleted && (*node)->m_Name == nullptr && isChildless(*node))
  {
    free(*node);
    *node = nullptr;
  }

  return smthDeleted;
}

bool delPhone(TPHONEBOOK *book,
              const char *phone)
{
  TNODE **node = &book->m_Root;
  if (*node == nullptr)
    return false;
  int phoneIndex = 0;
  bool deleted = delPhoneRec(node, phone, phoneIndex);
  if (deleted)
    book->m_Size--;
  return deleted;
}

const char *findPhone(TPHONEBOOK *book,
                      const char *phone)
{
  char *res = nullptr;
  TNODE *node = book->m_Root;
  if (node == nullptr)
    return res;

  int phoneIndex = 0;
  char digit;

  while (true)
  {
    digit = phone[phoneIndex++];
    if (digit == 0)
      return res;
    if (!isdigit(digit))
      return nullptr;
    int asNumber = digit - '0';
    node = node->m_Child[asNumber];
    if (node == nullptr)
      return res;
    if (node->m_Name != nullptr)
      res = node->m_Name;
  }

  return res;
}

#ifndef __PROGTEST__
int main()
{
  TPHONEBOOK b = {nullptr, 0};
  assert(!delPhone(&b, "420"));
  char tmpStr[100];
  const char *name;
  assert(addPhone(&b, "420", "Czech Republic"));
  assert(addPhone(&b, "42022435", "Czech Republic CVUT"));
  assert(addPhone(&b, "421", "Slovak Republic"));
  assert(addPhone(&b, "44", "Great Britain"));
  strncpy(tmpStr, "USA", sizeof(tmpStr) - 1);
  assert(addPhone(&b, "1", tmpStr));
  strncpy(tmpStr, "Guam", sizeof(tmpStr) - 1);
  assert(addPhone(&b, "1671", tmpStr));
  assert(addPhone(&b, "44", "United Kingdom"));
  assert(b.m_Size == 6);
  assert(!b.m_Root->m_Name);
  assert(!b.m_Root->m_Child[0]);
  assert(!strcmp(b.m_Root->m_Child[1]->m_Name, "USA"));
  assert(!b.m_Root->m_Child[1]->m_Child[0]);
  assert(!b.m_Root->m_Child[1]->m_Child[1]);
  assert(!b.m_Root->m_Child[1]->m_Child[2]);
  assert(!b.m_Root->m_Child[1]->m_Child[3]);
  assert(!b.m_Root->m_Child[1]->m_Child[4]);
  assert(!b.m_Root->m_Child[1]->m_Child[5]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Name);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[0]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[1]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[2]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[3]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[4]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[5]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[6]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Name);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[0]);
  assert(!strcmp(b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[1]->m_Name, "Guam"));
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[1]->m_Child[0]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[1]->m_Child[1]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[1]->m_Child[2]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[1]->m_Child[3]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[1]->m_Child[4]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[1]->m_Child[5]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[1]->m_Child[6]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[1]->m_Child[7]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[1]->m_Child[8]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[1]->m_Child[9]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[2]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[3]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[4]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[5]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[6]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[7]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[8]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[7]->m_Child[9]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[8]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]->m_Child[9]);
  assert(!b.m_Root->m_Child[1]->m_Child[7]);
  assert(!b.m_Root->m_Child[1]->m_Child[8]);
  assert(!b.m_Root->m_Child[1]->m_Child[9]);
  assert(!b.m_Root->m_Child[2]);
  assert(!b.m_Root->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Name);
  assert(!b.m_Root->m_Child[4]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Name);
  assert(!strcmp(b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Name, "Czech Republic"));
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Name);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Name);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[2]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Name);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[2]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Name);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[2]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[4]);
  assert(!strcmp(b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Name, "Czech Republic CVUT"));
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[2]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[4]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[4]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[4]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[4]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[9]);
  assert(!strcmp(b.m_Root->m_Child[4]->m_Child[2]->m_Child[1]->m_Name, "Slovak Republic"));
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[1]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[1]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[1]->m_Child[2]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[1]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[1]->m_Child[4]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[1]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[1]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[1]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[1]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[1]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[2]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[4]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[3]);
  assert(!strcmp(b.m_Root->m_Child[4]->m_Child[4]->m_Name, "United Kingdom"));
  assert(!b.m_Root->m_Child[4]->m_Child[4]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[4]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[4]->m_Child[2]);
  assert(!b.m_Root->m_Child[4]->m_Child[4]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[4]->m_Child[4]);
  assert(!b.m_Root->m_Child[4]->m_Child[4]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[4]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[4]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[4]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[4]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[9]);
  assert(!b.m_Root->m_Child[5]);
  assert(!b.m_Root->m_Child[6]);
  assert(!b.m_Root->m_Child[7]);
  assert(!b.m_Root->m_Child[8]);
  assert(!b.m_Root->m_Child[9]);
  name = findPhone(&b, "420800123456");
  assert(!strcmp(name, "Czech Republic"));
  name = findPhone(&b, "420224351111");
  assert(!strcmp(name, "Czech Republic CVUT"));
  name = findPhone(&b, "42022435");
  assert(!strcmp(name, "Czech Republic CVUT"));
  name = findPhone(&b, "4202243");
  assert(!strcmp(name, "Czech Republic"));
  name = findPhone(&b, "420224343258985224");
  assert(!strcmp(name, "Czech Republic"));
  name = findPhone(&b, "42");
  assert(!name);
  name = findPhone(&b, "422");
  assert(!name);
  name = findPhone(&b, "4422");
  assert(!strcmp(name, "United Kingdom"));
  name = findPhone(&b, "16713425245763");
  assert(!strcmp(name, "Guam"));
  name = findPhone(&b, "123456789123456789");
  assert(!strcmp(name, "USA"));
  assert(delPhone(&b, "420"));
  assert(delPhone(&b, "421"));
  assert(delPhone(&b, "44"));
  assert(delPhone(&b, "1671"));
  assert(!delPhone(&b, "1672"));
  assert(!delPhone(&b, "1671"));
  name = findPhone(&b, "16713425245763");
  assert(!strcmp(name, "USA"));
  name = findPhone(&b, "4422");
  assert(!name);
  name = findPhone(&b, "420800123456");
  assert(!name);
  name = findPhone(&b, "420224351111");
  assert(!strcmp(name, "Czech Republic CVUT"));
  assert(b.m_Size == 2);
  assert(!b.m_Root->m_Name);
  assert(!b.m_Root->m_Child[0]);
  assert(!strcmp(b.m_Root->m_Child[1]->m_Name, "USA"));
  assert(!b.m_Root->m_Child[1]->m_Child[0]);
  assert(!b.m_Root->m_Child[1]->m_Child[1]);
  assert(!b.m_Root->m_Child[1]->m_Child[2]);
  assert(!b.m_Root->m_Child[1]->m_Child[3]);
  assert(!b.m_Root->m_Child[1]->m_Child[4]);
  assert(!b.m_Root->m_Child[1]->m_Child[5]);
  assert(!b.m_Root->m_Child[1]->m_Child[6]);
  assert(!b.m_Root->m_Child[1]->m_Child[7]);
  assert(!b.m_Root->m_Child[1]->m_Child[8]);
  assert(!b.m_Root->m_Child[1]->m_Child[9]);
  assert(!b.m_Root->m_Child[2]);
  assert(!b.m_Root->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Name);
  assert(!b.m_Root->m_Child[4]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Name);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Name);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Name);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Name);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[2]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Name);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[2]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Name);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[2]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[4]);
  assert(!strcmp(b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Name, "Czech Republic CVUT"));
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[0]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[2]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[4]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[5]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[3]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[4]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[4]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[2]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[4]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[2]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[4]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[0]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[1]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[2]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[4]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[2]->m_Child[9]);
  assert(!b.m_Root->m_Child[4]->m_Child[3]);
  assert(!b.m_Root->m_Child[4]->m_Child[4]);
  assert(!b.m_Root->m_Child[4]->m_Child[5]);
  assert(!b.m_Root->m_Child[4]->m_Child[6]);
  assert(!b.m_Root->m_Child[4]->m_Child[7]);
  assert(!b.m_Root->m_Child[4]->m_Child[8]);
  assert(!b.m_Root->m_Child[4]->m_Child[9]);
  assert(!b.m_Root->m_Child[5]);
  assert(!b.m_Root->m_Child[6]);
  assert(!b.m_Root->m_Child[7]);
  assert(!b.m_Root->m_Child[8]);
  assert(!b.m_Root->m_Child[9]);
  assert(delPhone(&b, "1"));
  assert(delPhone(&b, "42022435"));
  assert(!addPhone(&b, "12345XYZ", "test"));
  assert(b.m_Size == 0);
  assert(!b.m_Root);

  delBook(&b);
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
