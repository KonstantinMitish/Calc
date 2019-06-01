// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "calc_func.h"
#include <math.h>

#define PI 3.14159265358979323846

int FuncDummy(double *In, double *Out)
{
  *Out = *In;
  return 1;
}

int FuncMinus(double *In, double *Out)
{
  *Out = In[0] - In[1];
  return 1;
}

int FuncPlus(double *In, double *Out)
{
  *Out = In[0] + In[1];
  return 1; 
}

int FuncMultiply(double *In, double *Out)
{
  *Out = In[0] * In[1];
  return 1;
}

int FuncDevide(double *In, double *Out)
{
  if (In[1] == 0)
    return 0;
  *Out = In[0] / In[1];
  return 1;
}

int FuncLog(double *In, double *Out)
{
  if (In[1] <= 0)
    return 0;
  if (In[0] <= 0 || In[0] == 1)
    return 0;
  *Out = log(In[1]) / log(In[0]);
  return 1;
} 

int FuncUnaryMinus(double *In, double *Out)
{
  *Out = -In[0];
  return 1;
}

int FuncPower(double *In, double *Out)
{
  *Out = pow(In[0], In[1]);
  return 1;
}

int FuncSqrt(double *In, double *Out)
{
  if (*In < 0)
    return 0;
  *Out = sqrt(*In);
  return 1; 
}

int FuncSin(double *In, double *Out)
{
  *Out = sin(*In);
  return 1;
}

int FuncCos(double *In, double *Out)
{
  *Out = cos(*In);
  return 1;
}

int FuncTg(double *In, double *Out)
{
  *Out = tan(*In);
  return 1;
}

int FuncCtg(double *In, double *Out)
{
  double t = tan(*In);
  if (t == 0)
    return 0;
  *Out = 1 / t;
  return 1;
}

int FuncArcsin(double *In, double *Out)
{
  if (*In < -1 || *In > 1)
    return 0;
  *Out = asin(*In);
  return 1;
}

int FuncArccos(double *In, double *Out)
{
  if (*In < -1 || *In > 1)
    return 0;
  *Out = acos(*In);
  return 1;
}

int FuncArctg(double *In, double *Out)
{
  *Out = atan(*In);
  return 1;
}

int FuncLn(double *In, double *Out)
{
  if (*In <= 0)
    return 0;
  *Out = log(*In);
  return 1;
}

int FuncFloor(double *In, double *Out)
{
  *Out = floor(*In);
  return 1;
}

int FuncCeil(double *In, double *Out)
{
  *Out = ceil(*In);
  return 1;
}