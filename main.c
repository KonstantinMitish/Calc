// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#pragma warning(disable:4996)
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "calc.h"
#include <signal.h>
#define BUF_STEP 512

static void _printEncoded( const char* Str )
{
  while (*Str != 0)
  {
    printf("%c%c", 'a' + (((*Str) & 0xF0) >> 4), 'a' + ((*Str) & 0x0F));
    Str++;
  }
}

static int s_bufSize;
static char* s_buf;

void SignalHandle( int Signal )
{
  if (Signal != SIGABRT)
    return;
  printf(" == ERROR ABORT in ");
  _printEncoded(s_buf);
  printf("\n");
}

// Main programm function
int main( int ArgC, char* ArgV[] )
{
  int c, i = 0;
  int calc = 0, wasslash = 0, comment = 0;
  signal(SIGABRT, SignalHandle);
  s_bufSize = BUF_STEP;
  s_buf = calloc(s_bufSize, sizeof(char));
  if (s_buf == NULL)
  {
    printf("ERROR: Not enough memory\n");
    return 0x2E;
  }
  if (ArgC > 2)
  {
    printf("ERROR: too many arguments\n");
    return 0x3E;
  }
  if (ArgC == 2)
    if (freopen(ArgV[1], "r", stdin) == NULL)
      printf("ERROR: can't open input file\n");
  do
  {
    c = getchar();
    if (c == '\n' || c == EOF)
    {
      if (calc)
      {
        double ans;
        int code;
        if (i == s_bufSize)
        {
          printf(" == ERROR: Not enough memory");
          if (c != EOF)
            printf("\n");
        }
        else
        {
          s_buf[i] = 0;
          if ((code = Calculate(s_buf, &ans)) == 0)
            printf(c == EOF ? " == %g" : " == %g\n", ans);
          else
          {
            printf(" == ERROR %s in ", GetStrError(code));
            _printEncoded(s_buf);
            if (c != EOF)
              printf("\n");
          }
        }
      }
      else if (c != EOF)
        printf("\n");
      i = 0;
      wasslash = 0;
      calc = 0;
      comment = 0;
    }
    else
    {
      if (i < s_bufSize)
        s_buf[i++] = (char)c;
      putchar(c);
      if (calc == 0 && comment == 0)
      {
        if (c == '/')
        {
          if (wasslash == 0)
            wasslash = 1;
          else
            comment = 1;
        }
        else
        {
          if (!isspace((unsigned char)c) || wasslash == 1)
            calc = 1;
          wasslash = 0;
        }
      }
      if (i == s_bufSize)
      {
        char* t;
        s_bufSize += BUF_STEP;
        t = realloc(s_buf, s_bufSize);
        if (t == NULL)
        {
          s_bufSize -= BUF_STEP;
          //printf("ERROR: Not enough memory\n");
          //return 0x1E;
        }
        else
          s_buf = t;
      }
    }
  }
  while (c != EOF);
  free(s_buf);
  return 0;
}                  