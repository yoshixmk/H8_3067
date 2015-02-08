/***********************************************************
**  RTL8019AS用レジスタアクセス  アドレス定義ファイル     **
***********************************************************/  

/* ベースアドレス*/
/*#define		RTL8019		(*(volatile unsigned char *)0x200000)*/
#define		RTL8019		(volatile unsigned char *)0x200000
volatile unsigned char	*pNIC;
/* 内部レジスタ相対アドレス*/
#define		Command_R	0x00
#define		DC_R		0x0e
#define		TC_R		0x0d
#define		TS_R		0x04
#define		TPS_R		0x04
#define		TBC_R0		0x05
#define		TBC_R1		0x06
#define		PSTART_R	0x01
#define		PSTOP_R		0x02
#define		CURR_R		0x07
#define		BNRY_R		0x03
#define		RC_R		0x0c
#define		RS_R		0x0c
#define		RSA_R0		0x08
#define		RSA_R1		0x09
#define		RBC_R0		0x0a
#define		RBC_R1		0x0b
#define		IM_R		0x0f
#define		IS_R		0x07
#define		PA_R0		0x01
#define		PA_R1		0x02
#define		PA_R2		0x03
#define		PA_R3		0x04
#define		PA_R4		0x05
#define		PA_R5		0x06
#define		MA_R0		0x08
#define		MA_R1		0x09
#define		MA_R2		0x0a
#define		MA_R3		0x0b
#define		MA_R4		0x0c
#define		MA_R5		0x0d
#define		MA_R6		0x0e
#define		MA_R7		0x0f
#define		DataPort	0x10
#define		ResetPort	0x18

#define		PSTART		0x46
#define		PSTOP		0x60
