/*--------------------------------------------------------------------*/
/* H8S,H8/300 SERIES C Compiler Ver. 1.0                              */
/* Copyright (C) 1994 Hitachi, Ltd.                                   */
/* Licensed Material of Hitachi,Ltd.                                  */
/*--------------------------------------------------------------------*/
/**********************************************************************/
/* SPEC;                                                              */
/*  NAME = setjmp.h :                                ;                */
/*                                                                    */
/*  FUNC = this module do the following functions    ;                */
/*                                                                    */
/*  CLAS = UNIT;                                                      */
/*                                                                    */
/* END;                                                               */
/**********************************************************************/
/**********************************************************************/
/* __CPU__ == 0: 300 STACK,  1: 300 REGISTER                          */
/*            2: 300H NORMAL 3: 300H ADVANCED                         */
/*            4: 2600 NORMAL 5: 2600 ADVANCED                         */
/*            6: 2000 NORMAL 7: 2000 ADVANCED                         */
/**********************************************************************/
#if __CPU__==0 || __CPU__==1
typedef int jmp_buf[9];
#else
typedef int jmp_buf[17];
#endif

extern int setjmp(jmp_buf);
extern void longjmp(jmp_buf, int);
extern volatile int _errno;

#ifndef SEQERR
#define SEQERR     1108
#endif
