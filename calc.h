// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef CALC_H_INCLUDED__
#define CALC_H_INCLUDED__
#pragma once

char *GetStrError( int Code );

int Calculate( char *Expression, double *Answer );

#endif /* CALC_H_INCLUDED__ */
