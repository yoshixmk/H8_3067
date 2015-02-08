/*--------------------------------------------------------------------*/
/* H8S,H8/300 SERIES C Compiler Ver. 1.0                              */
/* Copyright (C) 1994 Hitachi, Ltd.                                   */
/* Licensed Material of Hitachi,Ltd.                                  */
/*--------------------------------------------------------------------*/
/**********************************************************************/
/* SPEC;                                                              */
/*  NAME = stddef :                                                   */
/*                                                                    */
/*  FUNC = this module do the following functions                     */
/*                                                                    */
/*  CLAS = UNIT;                                                      */
/*                                                                    */
/* END;                                                               */
/**********************************************************************/
/**********************************************************************/
/* __CPU__ == 0: 300 STACK   1: 300 REGISTER                          */
/*            2: 300H NORMAL 3: 300H ADVANCED                         */
/*            4: 2600 NORMAL 5: 2600 ADVANCED                         */
/*            6: 2000 NORMAL 7: 2000 ADVANCED                         */
/**********************************************************************/
#if __CPU__ == 3 || __CPU__ == 5 || __CPU__ == 7
#ifndef  _PTRDIFF_T
#define  _PTRDIFF_T
typedef  long           ptrdiff_t;
#endif
#ifndef  _SIZE_T
#define  _SIZE_T
typedef  unsigned long  size_t;
#endif
#else
#ifndef  _PTRDIFF_T
#define  _PTRDIFF_T
typedef  int            ptrdiff_t;
#endif
#ifndef  _SIZE_T
#define  _SIZE_T
typedef  unsigned int   size_t;
#endif
#endif

#ifndef  NULL
#define  NULL           ((void *)0)
#endif

#ifndef  offsetof
#define  offsetof(type,id) ((char *)(&(((type *)NULL)->id))  -    \
                            (char *)((type *)NULL))
#endif

#ifndef  errno
#define  errno          _errno
#endif
