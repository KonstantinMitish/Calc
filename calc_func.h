// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef CALC_FUNC_H_INCLUDED__
#define CALC_FUNC_H_INCLUDED__
#pragma once

int FuncDummy(double *In, double *Out);

int FuncMinus(double *In, double *Out);
int FuncPlus(double *In, double *Out);
int FuncMultiply(double *In, double *Out);
int FuncDevide(double *In, double *Out);
int FuncUnaryMinus(double *In, double *Out);
int FuncPower(double *In, double *Out);

int FuncSqrt(double *In, double *Out);
int FuncSin(double *In, double *Out);
int FuncCos(double *In, double *Out);
int FuncTg(double *In, double *Out);
int FuncCtg(double *In, double *Out);
int FuncArcsin(double *In, double *Out);
int FuncArccos(double *In, double *Out);
int FuncArctg(double *In, double *Out);
int FuncLn(double *In, double *Out);
int FuncLog(double *In, double *Out);
int FuncFloor(double *In, double *Out);
int FuncCeil(double *In, double *Out);

#endif /* CALC_FUNC_H_INCLUDED__ */