// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <stdlib.h>
#include "stack.h"
#include <string.h>

struct stack_t
{
  int TypeSize;
  void *Memory;
  int SizeMax;
  int Size;
};

#define STEP 512

stack_t * StackInitSize( int Size )
{
  stack_t *stack = calloc(1, sizeof(stack_t));
  if (stack == NULL)
    return NULL;
  stack->SizeMax = STEP;
  stack->TypeSize = Size;
  stack->Memory = calloc(stack->SizeMax, Size);
  if (stack->Memory == NULL)
  {
    free(stack);
    return NULL;
  }
  return stack;
}

int StackPushPointer( stack_t *Stack, void *Data )
{
  if (Stack->Size == Stack->SizeMax)
  {
    void *t;
    Stack->SizeMax += STEP;
    t = realloc(Stack->Memory, Stack->TypeSize * Stack->SizeMax);
    if (t == NULL)
      return 0;
    Stack->Memory = t;
  }
  memcpy((char *)Stack->Memory + Stack->Size * Stack->TypeSize, Data, Stack->TypeSize);
  Stack->Size++;
  return 1;
}

void * StackTopPointer( stack_t *Stack )
{
  if (Stack->Size == 0)
    return NULL;
  return (char *)Stack->Memory + Stack->TypeSize * (Stack->Size - 1);
}

void StackPop( stack_t *Stack )
{
  if (Stack->Size != 0)
    Stack->Size--;
}

int StackEmpty( stack_t *Stack )
{
  return Stack->Size == 0;
}

void StackClear( stack_t *Stack )
{
  Stack->Size = 0;
}

void StackDestroy( stack_t *Stack )
{
  free(Stack->Memory);
  free(Stack);
}

void * StackData( stack_t *Stack )
{
  return Stack->Memory;
}

int StackSize( stack_t *Stack )
{
  return Stack->Size;
}