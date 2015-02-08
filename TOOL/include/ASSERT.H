/*--------------------------------------------------------------------*/
/* H8S,H8/300 SERIES C Compiler Ver. 1.0                              */
/* Copyright (C) 1994 Hitachi, Ltd.                                   */
/* Licensed Material of Hitachi,Ltd.                                  */
/*--------------------------------------------------------------------*/
/**********************************************************************/
/* SPEC;                                                              */
/*  NAME = assert.h : debug macro define          ;                   */
/*                                                                    */
/*  FUNC = this module do the following functions;                    */
/*         (1) if NDEBUG defined assert(x) = (void)0;                 */
/*  CLAS = UNIT;                                                      */
/*                                                                    */
/* END;                                                               */
/**********************************************************************/
#include <stdio.h>
#if defined(NDEBUG)
#define       assert(x) ((void)0)
#else
extern void abort(void);
#define       assert(x) ((x)?(void)0: \
              (fprintf(stderr,"ASSERTION FAILED: "#x" FILE %s,LINE %d \n", \
              __FILE__,__LINE__),abort(),(void)0))
#endif
