// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#pragma warning(disable:4996)
#include "stack.h"
#include <ctype.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "calc_func.h"
#include <string.h>
#include <stdlib.h>
/* 
 * Error codes handle
 */
#pragma region ERROR CODES

typedef enum error_code_t
{
  SUCCESS,
  ERR_MEMORY,
  ERR_UNEX_NUM,
  ERR_UNK_WORD,
  ERR_UNEX_OPER,
  ERR_NO_CLOSE_BRACKET,
  ERR_UNK,
  ERR_CLOSE_BRACKET,
  ERR_NE_ARGS,
  ERR_FUNCION,
  ERR_EMPTY,
  ERR_NUM
} error_code_t;

char *GetStrError( error_code_t Code )
{
  static char err[][59] = {
    "No error",
    "Not enough memory",
    "Unexepted number",
    "Unknown word",
    "Unexepted operator",
    "Close bracket not found",
    "Unkown error",
    "Unexepted close bracket",
    "Not enough argumetns",
    "Function calculation error",
    "Empty expression",
    "Number parsing error"
  };

  return err[Code];
}

#pragma endregion 

/*
 * Operator types handle
 */
#pragma region OPERTATOR TYPES

typedef enum
{
  NUMBER         = 0x01, /* 0000 0001 */
  OPEN_SKOBKA    = 0x02, /* 0000 0010 */
  CLOSE_SKOBKA   = 0x04, /* 0000 0100 */
  FUNCTION       = 0x08, /* 0000 1000 */
  BINARY_OPERAND = 0x10, /* 0001 0000 */
  UNARY_OPERAND  = 0x20  /* 0010 0000 */
} operand_type_t;

static int _isAllowed( operand_type_t Last, operand_type_t Current )
{
  static operand_type_t allow_after[][2] = {
    {NUMBER, CLOSE_SKOBKA | BINARY_OPERAND},
    {OPEN_SKOBKA, NUMBER | OPEN_SKOBKA | FUNCTION | UNARY_OPERAND},
    {CLOSE_SKOBKA, CLOSE_SKOBKA | BINARY_OPERAND},
    {FUNCTION, OPEN_SKOBKA},
    {BINARY_OPERAND, NUMBER | FUNCTION | OPEN_SKOBKA | UNARY_OPERAND},
    {UNARY_OPERAND, NUMBER | FUNCTION | OPEN_SKOBKA | UNARY_OPERAND},
  };
  int i;

  for (i = 0; i < sizeof(allow_after) / sizeof(allow_after[0]); i++)
  {
    if (Last == allow_after[i][0])
    {
      if (Current & allow_after[i][1])
        return 1;
      return 0;
    }
  }
  return 0;
}

#pragma endregion 

/*
 * Operators and functions handle
 */
#pragma region OPERATORS
static struct
{
  operand_type_t Type;
  char Code;
  int Priority;
  int Associativity;
  char FunctionName[7];
  int FunctionNameLen;
  int (*FunctionExecute)(double *, double *);
  int NumberOfArguments;
} s_operands[] = 
{
    {FUNCTION,       'k',  3,  1,   "sqrt", 4,       FuncSqrt, 1},
    {FUNCTION,       's',  3,  1,    "sin", 3,        FuncSin, 1},
    {FUNCTION,       'c',  3,  1,    "cos", 3,        FuncCos, 1},
    {FUNCTION,       't',  3,  1,     "tg", 2,         FuncTg, 1},
    {FUNCTION,       'q',  3,  1,    "ctg", 3,        FuncCtg, 1},
    {FUNCTION,      -'s',  3,  1, "arcsin", 6,     FuncArcsin, 1},
    {FUNCTION,      -'c',  3,  1, "arccos", 6,     FuncArccos, 1},
    {FUNCTION,      -'t',  3,  1,  "arctg", 5,      FuncArctg, 1},
    {FUNCTION,       'l',  3,  1,     "ln", 2,         FuncLn, 1},
    {FUNCTION,      -'l',  3,  1,    "log", 3,        FuncLog, 2},
    {FUNCTION,       'f',  3,  1,  "floor", 5,      FuncFloor, 1},
    {FUNCTION,      -'f',  3,  1,   "ceil", 4,       FuncCeil, 1},
    {BINARY_OPERAND, '^',  1,  1,      {0}, 0,      FuncPower, 2},
    {UNARY_OPERAND, -'-',  1,  1,      {0}, 0, FuncUnaryMinus, 1},
    {BINARY_OPERAND, '*',  0, -1,      {0}, 0,   FuncMultiply, 2},
    {BINARY_OPERAND, '/',  0, -1,      {0}, 0,     FuncDevide, 2},
    {BINARY_OPERAND, '+', -1, -1,      {0}, 0,       FuncPlus, 2},
    {BINARY_OPERAND, '-', -1, -1,      {0}, 0,      FuncMinus, 2},
    {BINARY_OPERAND, '=', -2,  1,      {0}, 0,      FuncDummy, 2}
};

static int _getArgNum( char Code )
{
  int i;
  for (i = 0; i < sizeof(s_operands) / sizeof(s_operands[0]); i++)
    if (s_operands[i].Code == Code)
      return s_operands[i].NumberOfArguments;
  return 0;
}

static int (* _getFunction( char Code ))( double *Arg, double *Ans )
{
  int i;
  for (i = 0; i < sizeof(s_operands) / sizeof(s_operands[0]); i++)
    if (s_operands[i].Code == Code)
      return s_operands[i].FunctionExecute;
  return FuncDummy;
}

static int _getPriority( char Operand )
{
  int i;

  for (i = 0; i < sizeof(s_operands) / sizeof(s_operands[0]); i++)
    if (s_operands[i].Code == Operand)
      return s_operands[i].Priority;
  return -239;
}

static int _isRightAssotiative( char Operand )
{
  int i;

  for (i = 0; i < sizeof(s_operands) / sizeof(s_operands[0]); i++)
    if (s_operands[i].Code == Operand)
    {
      if (s_operands[i].Associativity == 1)
        return 1;
      return 0;
    }
  return 0;
}

static int _isFunction( char Operand )
{
  int i;

  for (i = 0; i < sizeof(s_operands) / sizeof(s_operands[0]); i++)
    if (s_operands[i].Code == Operand && s_operands[i].Type == FUNCTION)
      return 1;
  return 0;
}

static int _isOperand( char Operand )
{
  int i;

  for (i = 0; i < sizeof(s_operands) / sizeof(s_operands[0]); i++)
    if (s_operands[i].Code == Operand &&
      (s_operands[i].Type == BINARY_OPERAND || s_operands[i].Type == UNARY_OPERAND))
      return 1;
  return 0;
}

#pragma endregion 


/*
 * Variables handle
 */
#pragma region VARIABLES
typedef struct
{
  char Name[10];
  double Value;
} variable_t;

variable_t *s_vars = NULL;
int s_noofVars = 0;

static void _varsFree( void )
{
  if (s_vars != NULL)
  {
    free(s_vars);
    s_vars = NULL;
    s_noofVars = 0;
  }
}

static int _varsInit( void )
{
  if ((s_vars = calloc(2, sizeof(variable_t)))== NULL)
    return 0;
  s_noofVars = 2;
  strcpy(s_vars[0].Name, "pi");
  s_vars[0].Value = M_PI;
  strcpy(s_vars[1].Name, "e");
  s_vars[1].Value = M_E;
  return 1;
}

static int _regVar( const char *Name, double Value )
{
  variable_t *t;
  if ((t = realloc(s_vars, ++s_noofVars * sizeof(variable_t)))== NULL)
    return -1;
  s_vars = t;
  strncpy(s_vars[s_noofVars - 1].Name, Name, min(strlen(Name) + 1, sizeof(s_vars[0].Name)));
  s_vars[s_noofVars - 1].Value = Value;
  return s_noofVars - 1;
}

///static int _varsRegVar( variable_array_t *Vars, const char *Name, double Value )
///{
///  variable_t *t;
///  if ((t = realloc(Vars->Vars, ++Vars->Size * sizeof(variable_t)))== NULL)
///  {
///    Vars->Size--;
///    return ERR_MEMORY;
///  }
///  Vars->Vars = t;
///  strncpy(Vars->Vars[Vars->Size - 1].Name, Name, min(strlen(Name) + 1, sizeof(Vars->Vars[0].Name)));
///  Vars->Vars[Vars->Size - 1].Value = Value;
///  return SUCCESS;
///}
#pragma endregion 

/*
 * One function for both funcionts and variables
 */
static int _getCode( char **Func, int *Code )
{
  // determinate if it function, var, or incorrect
  int len = -1;
  char *a = *Func;

  while (isalpha(*a))
  {
    len++;
    a++;
  }
  // find what next
  while (isspace(*a))
    a++;
  if (isalpha(*a))
  {       
    *Func += len;
    return -1; 
  }
  if (_isOperand(*a) || *a == ')' || *a == 0)// variable
  {
    int i;
    for (i = 0; i < s_noofVars; i++)
      if (strncmp(s_vars[i].Name, *Func, len + 1) == 0)
      {  
       *Func += len;
        *Code = i;
        return 0;
      } 
    *Func += len;
    return -1; // error
  }
  if (*a == '(') // function
  {
    int i;
    for (i = 0; i < sizeof(s_operands) / sizeof(s_operands[0]); i++)
    {
      if (s_operands[i].Type == FUNCTION && strncmp(*Func, s_operands[i].FunctionName, s_operands[i].FunctionNameLen) == 0)
      {
        *Code = s_operands[i].Code;
        *Func += len;
        return 1;
      }
    }
    *Func += len;
    return -1;
  }
  *Func += len;
  return -1; // error
}

/*
 * Skobki checker
 */
static int _simpleCheck( char *Text )
{
  int cnt = 0;

  while (*Text != 0)
  {
    if (*Text == '(')
      cnt++;
    else if (*Text == ')')
    {
      cnt--;
      if (cnt < 0)
        return 0;
    }
    Text++;
  }
  return cnt == 0;
}

/*
 * Polska kurwa calculator handle
 */
#pragma region KURWA
typedef struct
{
  int IsNumber;
  int IsVar;
  union
  {
    double Number;
    char Operand;
  } As;
} train_t;

typedef struct
{
  stack_t *Texas;
  operand_type_t Last;
  stack_t *Kalofornia;
  ///variable_array_t Variables;
  char *NewYork;
  int I, Unary;
} calc_t;

static error_code_t _calcInit( calc_t *Calc, char *Expression )
{ 
  static train_t null = {0};
  Calc->Last = OPEN_SKOBKA;
  Calc->NewYork = Expression;
  Calc->Unary = 1;
  //if (_varsInit(&Calc->Variables) != SUCCESS)
  if ((Calc->Texas = StackInit(train_t)) == NULL)
  {
    _varsFree();
    return ERR_MEMORY;
  }
  if ((Calc->Kalofornia = StackInit(train_t)) == NULL)
  {
    StackDestroy(Calc->Texas);
    _varsFree();
    return ERR_MEMORY;
  }
  if (!StackPush(Calc->Texas, null))
  {
    StackDestroy(Calc->Texas);
    StackDestroy(Calc->Kalofornia);
    _varsFree();
    return ERR_MEMORY;
  }
  return SUCCESS;
}

static void _calcDestroy( calc_t *Calc )
{
  if (Calc->Texas != NULL)
  {
    StackDestroy(Calc->Texas);
    Calc->Texas = NULL;
  }
  if (Calc->Kalofornia != NULL)
  {
    StackDestroy(Calc->Kalofornia);
    Calc->Kalofornia = NULL;
  }
}

static error_code_t _processFunctionOrOperator( calc_t *Calc )
{
  train_t train = {0};
  train.IsNumber = 0;
  if (isalpha(Calc->NewYork[Calc->I]))
  {
    char *t = &Calc->NewYork[Calc->I];
    int code, a;
    if ((a = _getCode(&t, &code)) == -1)
      return ERR_UNK_WORD;
    if (a == 0)
    {          
      #pragma region VARIABLE
      if (!_isAllowed(Calc->Last, NUMBER))
        return ERR_UNEX_NUM;
      //train.As.Number = s_vars[code].Value;
      train.As.Operand = code;
      train.IsNumber = 1;
      train.IsVar = 1;
      Calc->I = (int)(t - Calc->NewYork);
      if (!StackPush(Calc->Kalofornia, train))
        return ERR_MEMORY;
      Calc->Unary = 0;
      Calc->Last = NUMBER;
      Calc->I++;
      return SUCCESS;
#pragma endregion
    }
#pragma region FUNCTION
    if (!_isAllowed(Calc->Last, FUNCTION))
      return ERR_UNEX_NUM;
    train.As.Operand = (char)code;
    Calc->I = (int)(t - Calc->NewYork);
    Calc->Last = FUNCTION;
    
#pragma endregion 
  }
  else if (Calc->Unary)
  {
#pragma region UNARY OPERAND
    if (Calc->NewYork[Calc->I] == '-')
    {
      train.As.Operand = -'-';
      Calc->Last = UNARY_OPERAND;
    }
    else 
      return ERR_UNEX_OPER;
#pragma endregion 
  }
  else
  {
    train.As.Operand = Calc->NewYork[Calc->I];
    Calc->Last = BINARY_OPERAND;
  }
  while (!StackEmpty(Calc->Texas) && (_isFunction(StackTop(Calc->Texas, train_t).As.Operand)
    ||
    (_isOperand(StackTop(Calc->Texas, train_t).As.Operand) && _getPriority(StackTop(Calc->Texas, train_t).As.Operand) > _getPriority(train.As.Operand))
    ||
    (_isOperand(StackTop(Calc->Texas, train_t).As.Operand) && _getPriority(StackTop(Calc->Texas, train_t).As.Operand) == _getPriority(train.As.Operand) && !_isRightAssotiative(StackTop(Calc->Texas, train_t).As.Operand)))
    && StackTop(Calc->Texas, train_t).As.Operand != '(')
  {
    if (!StackPush(Calc->Kalofornia, StackTop(Calc->Texas, train_t)))
      return ERR_MEMORY;
    StackPop(Calc->Texas);
  }
  if (!StackPush(Calc->Texas, train))
    return ERR_MEMORY;
  Calc->I++;
  Calc->Unary = 1;
  return SUCCESS;
}

static error_code_t _processCloseSkobka( calc_t *Calc )
{  
  if (!_isAllowed(Calc->Last, CLOSE_SKOBKA))
    return ERR_EMPTY;
  while (!StackEmpty(Calc->Texas) && StackTop(Calc->Texas, train_t).As.Operand != '(')
  {
    if (!StackPush(Calc->Kalofornia, StackTop(Calc->Texas, train_t)))
      return ERR_MEMORY;
    StackPop(Calc->Texas);
  }
  if (StackEmpty(Calc->Texas) || StackTop(Calc->Texas, train_t).As.Operand != '(')
    return ERR_CLOSE_BRACKET;
  StackPop(Calc->Texas);
  Calc->I++;
  Calc->Unary = 0; 
  Calc->Last = CLOSE_SKOBKA;
  return SUCCESS;
}

static error_code_t _processOpenSkobka( calc_t *Calc )
{
  train_t train = {0};
  if (!_isAllowed(Calc->Last, OPEN_SKOBKA))
    return ERR_EMPTY;
  train.IsNumber = 0;
  train.As.Operand = Calc->NewYork[Calc->I];
  if (!StackPush(Calc->Texas, train))
    return ERR_MEMORY;
  Calc->I++;
  Calc->Unary = 1;
  Calc->Last = OPEN_SKOBKA;
  return SUCCESS;
}

static error_code_t _processNumber( calc_t *Calc )
{
  train_t train = {0};
  char *end;

  if (!_isAllowed(Calc->Last, NUMBER))
    return ERR_UNEX_NUM;
  train.IsNumber = 1;
  train.As.Number = strtod(Calc->NewYork + Calc->I, &end);
  if (end == Calc->NewYork + Calc->I)
    return ERR_NUM;
  if (!StackPush(Calc->Kalofornia, train))
    return ERR_MEMORY;
  Calc->I = end - Calc->NewYork;
  Calc->Unary = 0;
  Calc->Last = NUMBER;
  return SUCCESS;
}

static error_code_t _calcMakePoskaKurwa( calc_t *Calc )
{
  error_code_t code;
  // Make polska kurwa
  while(Calc->NewYork[Calc->I] != 0)
  {
    if (isspace((unsigned char)Calc->NewYork[Calc->I]))
    {
      Calc->I++;
    }
    else if (Calc->NewYork[Calc->I] == ',' || Calc->NewYork[Calc->I] == ';')
    {
      if (!_isAllowed(Calc->Last, CLOSE_SKOBKA))
        return ERR_UNEX_OPER;
      while (!StackEmpty(Calc->Texas) && StackTop(Calc->Texas, train_t).As.Operand != '(')
      {
        if (!StackPush(Calc->Kalofornia, StackTop(Calc->Texas, train_t)))
          return ERR_MEMORY;
        StackPop(Calc->Texas);
      }
      if (StackEmpty(Calc->Texas) || StackTop(Calc->Texas, train_t).As.Operand != '(')
        return ERR_CLOSE_BRACKET;
      //StackPop(Calc->Texas);
      Calc->I++;
      Calc->Unary = 1;
      Calc->Last = OPEN_SKOBKA;
    }
    else if (isdigit((unsigned char)Calc->NewYork[Calc->I]) || Calc->NewYork[Calc->I] == '.')
    {
      if ((code = _processNumber(Calc)) != SUCCESS)
        return code;
    }
    else if (isalpha((unsigned char)Calc->NewYork[Calc->I]) || 
      _isOperand(Calc->NewYork[Calc->I]))
    { 
      if ((code = _processFunctionOrOperator(Calc)) != SUCCESS)
        return code;
    }
    else if (Calc->NewYork[Calc->I] == '(')
    { 
      if ((code = _processOpenSkobka(Calc)) != SUCCESS)
        return code;
    }
    else if (Calc->NewYork[Calc->I] == ')')
    {                 
      if ((code = _processCloseSkobka(Calc)) != SUCCESS)
        return code;
    }
    else
      return ERR_UNK;
  }
  while (!StackEmpty(Calc->Texas))
  {
     if (StackTop(Calc->Texas, train_t).As.Operand == '(')
       return ERR_NO_CLOSE_BRACKET;
     if (!StackPush(Calc->Kalofornia, StackTop(Calc->Texas, train_t)))
       return ERR_MEMORY;
     StackPop(Calc->Texas);
  }
  StackDestroy(Calc->Texas);
  Calc->Texas = NULL;
  return SUCCESS;
}

static error_code_t _calcCalculatePoskaKurwa( calc_t *Calc, double *Answer ) 
{
  stack_t *numbers = StackInit(train_t);
  if (numbers == NULL)
    return ERR_MEMORY;
  int size = StackSize(Calc->Kalofornia) - 1;
  train_t *polska = StackData(Calc->Kalofornia);
  double a[3], ans;
  for (Calc->I = 0; Calc->I < size; Calc->I++)
  {
    if (polska[Calc->I].IsNumber)
    {
      if (StackPush(numbers, polska[Calc->I]) != 1)
      {
        StackDestroy(numbers);
        return ERR_MEMORY;
      }
    }
    else
    {
      int op = _getArgNum(polska[Calc->I].As.Operand), j, co = 0;
      for (j = 0; j < op; j++)
      {
        if (StackEmpty(numbers))
        {
          StackDestroy(numbers);
          return ERR_NE_ARGS;
        }
        train_t tr = StackTop(numbers, train_t);
        if (tr.IsVar && polska[Calc->I].As.Operand == '=' && j == 0)
          co = tr.As.Operand;
        if (tr.IsVar)
          a[op - 1 - j] = s_vars[tr.As.Operand].Value;
        else
          a[op - 1 - j] = StackTop(numbers, train_t).As.Number;
        StackPop(numbers);
      }
      int (*f)(double*, double*) = _getFunction(polska[Calc->I].As.Operand);
      
      if (f == NULL || f(a, &ans) != 1)
      {
        StackDestroy(numbers);
        return ERR_FUNCION;
      }
      if (polska[Calc->I].As.Operand == '=')
      {
        s_vars[co].Value = ans;
      }
      else
      if (StackPush(numbers, ans) != 1)
      {
        StackDestroy(numbers);
        return ERR_MEMORY;
      }
    }
  }
  if (StackSize(numbers) != 1)
  {
    StackDestroy(numbers);
    return ERR_UNK;
  }
  *Answer = StackTop(numbers, double);
  StackDestroy(numbers);
  return SUCCESS;
}

#pragma endregion 

/* 
 * Main calculator function
 */
int Calculate( char *Expression, double *Answer )
{
  calc_t calc = {0};
  error_code_t code;
  if (!_simpleCheck(Expression))
    return (int)ERR_NO_CLOSE_BRACKET;

  if (!_varsInit())
    return ERR_MEMORY;
  if ((code = _calcInit(&calc, Expression)) != SUCCESS)
  {
    return (int)code;
  }

  if ((code = _calcMakePoskaKurwa(&calc)) != SUCCESS)
  {
    _calcDestroy(&calc); 
    _varsFree();
    return (int)code;
  }

  if ((code = _calcCalculatePoskaKurwa(&calc, Answer)) != SUCCESS)
  {
    _calcDestroy(&calc);
    return (int)code;
  } 
  _varsFree();

  _calcDestroy(&calc);
  return (int)SUCCESS;
}