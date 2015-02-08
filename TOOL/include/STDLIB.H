/*--------------------------------------------------------------------*/
/* H8S,H8/300 SERIES C Compiler Ver. 2.0                              */
/* Copyright (C) 1994,1996 Hitachi, Ltd.                              */
/* Licensed Material of Hitachi,Ltd.                                  */
/*--------------------------------------------------------------------*/
/*****************************************************************/
/* SPEC ;                                                        */
/*   NAME = stdlib :                                             */
/*   FUNC =                                                      */
/*          ;                                                    */
/*                                                               */
/*                                                               */
/*                                                               */
/*   CLAS = UNIT ;                                               */
/*   END ;                                                       */
/*****************************************************************/

#include <stddef.h>

typedef struct {int quot; int rem;} div_t ;
typedef struct {long quot; long rem;} ldiv_t ;

#define RAND_MAX  32767
#ifndef ERANGE
#define ERANGE  1100
#endif
#ifndef EDOM
#define EDOM    1101
#endif
#ifndef EDIV
#define EDIV    1102
#endif
#ifndef ESTRN
#define ESTRN   1104
#endif
#ifndef ECBASE
#define ECBASE  1200
#endif
#ifndef ETLN
#define ETLN    1202
#endif
#ifndef EEXP
#define EEXP    1204
#endif
#ifndef EEXPN
#define EEXPN   1206
#endif
#ifndef EFLOATO
#define EFLOATO 1210
#endif
#ifndef EFLOATU
#define EFLOATU 1220
#endif
#ifndef EOVER
#define EOVER   1230
#endif
#ifndef EUNDER
#define EUNDER  1240
#endif

extern volatile int _errno;
extern const double _HUGE_VAL;
#define HUGE_VAL _HUGE_VAL
extern const size_t _sbrk_size;

extern double atof(const char *) ;
extern int atoi(const char *) ;
extern long int atol(const char *) ;
extern double strtod(const char *, char **) ;
extern long int strtol(const char *, char **, int);
extern int rand(void) ;
extern void srand(unsigned int);
extern void *calloc(size_t, size_t);
extern void free(void *) ;
extern void *malloc(size_t);
extern void *realloc(void *, size_t) ;
extern void *bsearch(const void *, const void *, size_t, size_t,
                                  int(*)(const void*,const void*) ) ;
extern void qsort(void *, size_t, size_t,
                                  int(*)(const void*,const void*) );
extern int abs(int);
extern div_t div(int, int);
extern long int labs(long int);
extern ldiv_t ldiv(long int, long int);

