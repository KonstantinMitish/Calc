// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef STACK_H_INCLUDED__
#define STACK_H_INCLUDED__
#pragma once

typedef struct stack_t stack_t;

stack_t * StackInitSize( int Size );

int StackPushPointer( stack_t *Stack, void *Data );

void * StackTopPointer( stack_t *Stack );

void StackPop( stack_t *Stack );

int StackEmpty( stack_t *Stack );

void StackClear( stack_t *Stack );

void StackDestroy( stack_t *Stack );

void * StackData( stack_t *Stack );

int StackSize( stack_t *Stack );

#define StackTop(Stack, Type) (*((Type *)StackTopPointer(Stack)))

#define StackInit(Type) StackInitSize(sizeof(Type))

#define StackPush(Stack, X) StackPushPointer((Stack), &(X))

#endif /* STACK_H_INCLUDED__ */
