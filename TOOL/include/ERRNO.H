/*--------------------------------------------------------------------*/
/* H8S,H8/300 SERIES C Compiler Ver. 1.0                              */
/* Copyright (C) 1994 Hitachi, Ltd.                                   */
/* Licensed Material of Hitachi,Ltd.                                  */
/*--------------------------------------------------------------------*/
/*****************************************************************/
/* SPEC ;                                                        */
/*   NAME = errno :                                             */
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

#ifndef EDIV
#define EDIV    1102
#endif

#ifndef ESTRN
#define ESTRN   1104
#endif

#ifndef PTRERR
#define PTRERR  1106
#endif

#ifndef SEQERR
#define SEQERR  1108
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

#ifndef ENUM
#define ENUM    1208
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

#ifndef NOTOPN
#define NOTOPN  1300
#endif

#ifndef EBADF
#define EBADF   1302
#endif

#ifndef ECSPEC
#define ECSPEC  1304
#endif

#ifndef errno
#define errno _errno
#endif

extern volatile int _errno;
