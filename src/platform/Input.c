#include "Input.h"
#include "error.h"

#include <plank/plank.h>

struct Input
{
  vector(int) *keys;
  vector(int) *downKeys;
  vector(int) *upKeys;
};

struct Input *input;

void _InputInitialize()
{
  input = palloc(struct Input);
  input->keys = vector_new(int);
  input->downKeys = vector_new(int);
  input->upKeys = vector_new(int);
}

void _InputCleanup()
{
  vector_delete(input->downKeys);
  vector_delete(input->upKeys);
  vector_delete(input->keys);
  pfree(input);
}

void _InputSetKey(int key, int state)
{
  size_t i = 0;
  int add = 0;

  if(state == 1)
  {
    for(i = 0; i < vector_size(input->keys); i++)
    {
      if(vector_at(input->keys, i) == key)
      {
        return;
      }
    }

    vector_push_back(input->keys, key);
    vector_push_back(input->downKeys, key);
  }
  else if(state == 0)
  {
    for(i = 0; i < vector_size(input->keys); i++)
    {
      if(vector_at(input->keys, i) == key)
      {
        add = 1;
        vector_erase(input->keys, i);
        i--;
      }
    }

    if(add == 1)
    {
      vector_push_back(input->upKeys, key);
    }
  }
  else
  {
    Error("Invalid keyboard state");
  }
}

int InputKey(int key)
{
  size_t i = 0;

  for(i = 0; i < vector_size(input->keys); i++)
  {
    if(vector_at(input->keys, i) == key)
    {
      return 1;
    }
  }

  return 0;
}