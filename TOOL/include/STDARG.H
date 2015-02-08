/*--------------------------------------------------------------------*/
/* H8S,H8/300 SERIES C Compiler Ver. 1.0                              */
/* Copyright (C) 1994 Hitachi, Ltd.                                   */
/* Licensed Material of Hitachi,Ltd.                                  */
/*--------------------------------------------------------------------*/
/*****************************************************************/
/* SPEC ;                                                        */
/*   NAME = stdarg :                                             */
/*   FUNC = this module do the following finctions ;             */
/*                                                               */
/*         (1) initialize parameter pointer                      */
/*         (2) set parameter pointer to next parameter           */
/*                                                               */
/*                                                               */
/*   END ;                                                       */
/*****************************************************************/

typedef char *va_list ;

extern void va_end(va_list);

#define va_start(ap,param) ((void)((ap)=(int)\
                                       ((char *) &(param)+sizeof(param))%2u?\
                                        (char *) &(param)+sizeof(param)+1u:\
                                        (char *) &(param)+sizeof(param)))

#define va_arg(ap,type) (*((ap)=((int)((ap)+sizeof(type))%2u?\
                                      (ap)+sizeof(type)+1u:\
                                      (ap)+sizeof(type)),\
                               (type *)((int)((ap)-sizeof(type))%2u?\
                                              (ap)-sizeof(type)-1u:\
                                              (ap)-sizeof(type))))

#define va_end(ap)

