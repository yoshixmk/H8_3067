/*--------------------------------------------------------------------*/
/* H8S,H8/300 SERIES C Compiler Ver. 2.0                              */
/* Copyright (C) 1994,1996 Hitachi, Ltd.                              */
/* Licensed Material of Hitachi,Ltd.                                  */
/*--------------------------------------------------------------------*/
/*****************************************************************/
/* SPEC ;                                                        */
/*   NAME = machine.h :                                          */
/*   FUNC =                                                      */
/*          ;                                                    */
/*   CLAS = UNIT ;                                               */
/*   END ;                                                       */
/*****************************************************************/
#define set_imask_ccr _builtin_set_imask_ccr
#define get_imask_ccr _builtin_get_imask_ccr
#define set_ccr       _builtin_set_ccr
#define get_ccr       _builtin_get_ccr
#define and_ccr       _builtin_and_ccr
#define or_ccr        _builtin_or_ccr
#define xor_ccr       _builtin_xor_ccr
#define set_imask_exr _builtin_set_imask_exr
#define get_imask_exr _builtin_get_imask_exr
#define set_exr       _builtin_set_exr
#define get_exr       _builtin_get_exr
#define and_exr       _builtin_and_exr
#define or_exr        _builtin_or_exr
#define xor_exr       _builtin_xor_exr
#define trapa         _builtin_trapa
#define sleep         _builtin_sleep
#define movfpe        _builtin_movfpe
#define movtpe        _builtin_movtpe
#define tas           _builtin_tas
#define eepmov        _builtin_eepmov
#define mac           _builtin_mac
#define macl          _builtin_macl
#define rotlc         _builtin_rotlc
#define rotlw         _builtin_rotlw
#define rotll         _builtin_rotll
#define rotrc         _builtin_rotrc
#define rotrw         _builtin_rotrw
#define rotrl         _builtin_rotrl
#define ovfaddc       _builtin_ovfaddc
#define ovfaddw       _builtin_ovfaddw
#define ovfaddl       _builtin_ovfaddl
#define ovfsubc       _builtin_ovfsubc
#define ovfsubw       _builtin_ovfsubw
#define ovfsubl       _builtin_ovfsubl
#define ovfshalc      _builtin_ovfshalc
#define ovfshalw      _builtin_ovfshalw
#define ovfshall      _builtin_ovfshall
#define ovfnegc       _builtin_ovfnegc
#define ovfnegw       _builtin_ovfnegw
#define ovfnegl       _builtin_ovfnegl
#define dadd          _builtin_dadd
#define dsub          _builtin_dsub
#define nop           _builtin_nop


extern void          set_imask_ccr(unsigned char);
extern unsigned char get_imask_ccr(void);
extern void          set_ccr(unsigned char);
extern unsigned char get_ccr(void);
extern void          and_ccr(unsigned char);
extern void          or_ccr(unsigned char);
extern void          xor_ccr(unsigned char);
extern void          set_imask_exr(unsigned char);
extern unsigned char get_imask_exr(void);
extern void          set_exr(unsigned char);
extern unsigned char get_exr(void);
extern void          and_exr(unsigned char);
extern void          or_exr(unsigned char);
extern void          xor_exr(unsigned char);
extern void          trapa(unsigned int);
extern void          sleep(void);
extern void          movfpe(char *,char);
extern void          movtpe(char ,char *);
extern void          tas(char *);
#if __CPU__==0 || __CPU__==1
extern void          eepmov(char *,char *,unsigned char);
#else
extern void          eepmov(char *,char *,unsigned int);
#endif
extern long          mac(long,int *,int *,unsigned long);
extern long          macl(long,int *,int *,unsigned long,unsigned long);
extern char          rotlc(int,char);
extern int           rotlw(int,int);
extern long          rotll(int,long);
extern char          rotrc(int,char);
extern int           rotrw(int,int);
extern long          rotrl(int,long);
extern int           ovfaddc(char,char,char *);
extern int           ovfaddw(int,int,int *);
extern int           ovfaddl(long,long,long *);
extern int           ovfsubc(char,char,char *);
extern int           ovfsubw(int,int,int *);
extern int           ovfsubl(long,long,long *);
extern int           ovfshalc(char,char *);
extern int           ovfshalw(int,int *);
extern int           ovfshall(long,long *);
extern int           ovfnegc(char,char *);
extern int           ovfnegw(int,int *);
extern int           ovfnegl(long,long *);
extern void          dadd(unsigned char,char *,char *,char *);
extern void          dsub(unsigned char,char *,char *,char *);
extern void          nop(void);

