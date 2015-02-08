/*--------------------------------------------------------------------*/
/* H8S,H8/300 SERIES C Compiler Ver. 1.0                              */
/* Copyright (C) 1994 Hitachi, Ltd.                                   */
/* Licensed Material of Hitachi,Ltd.                                  */
/*--------------------------------------------------------------------*/
/*****************************************************************/
/* SPEC ;                                                        */
/*   NAME = math :                                               */
/*   FUNC =                                                      */
/*          ;                                                    */
/*                                                               */
/*                                                               */
/*                                                               */
/*   CLAS = UNIT ;                                               */
/*   END ;                                                       */
/*****************************************************************/

#ifndef ERANGE
#define ERANGE  1100
#endif
#ifndef EDOM
#define EDOM    1101
#endif
#ifndef ENUM
#define ENUM    1208
#endif
extern volatile int _errno;
extern const double _HUGE_VAL;
#define HUGE_VAL _HUGE_VAL

extern double frexp(double, int *);
extern double ldexp(double, int );
extern double modf(double, double *);
extern double ceil(double);
extern double fabs(double);
extern double floor(double);
extern double fmod(double, double);
