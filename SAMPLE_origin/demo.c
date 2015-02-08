/**************************************************************************************************
���̃\�t�g�E�F�A�̓t���[�E�F�A�ł��B
���쌠�͍쐬���ł���(��)�k�l�d�q�����L������̂Ƃ��܂��B
���̃\�t�g�E�F�A���g�p�������Ƃɂ��S�Ă̑��Q�ɂ��āA
(��)�k�l�d�q�͈�؂̐ӔC�𕉂�Ȃ����̂Ƃ��܂��B
���̃\�t�g�E�F�A�̈ꕔ�܂��͑S�Ă𖳒f�œ]�ڂ��邱�Ƃ�
�֎~���܂��B�G���Ȃǂ֏Љ�E���^�̏ꍇ�͘A�������肢�v
���܂��B
Copyright (C) Hokuto denshi Co,Ltd. 2008
**************************************************************************************************/
#include "H8_CPU.h"
#include "RTL8019AS.h"
#include <machine.h>

#define		CPUCLK		20000000L
#define		SMPLING		8000
#define		CNT8KHZ		(unsigned short)(CPUCLK/SMPLING)
#define		CNTBASE		(unsigned short)(CPUCLK/40000)

unsigned char	myMacAddress[6];				/* ������MAC�A�h���X */
const unsigned char	myIpAddress[4] = {192,168,0,10};		/* ������IPC�A�h���X */
unsigned char	destMacAddress[6];				/* �����MAC�A�h���X */
#define RD_L()		{ P6DR&=0xef; }				/*P64=L 11101111*/
#define RD_H()		{ P6DR|=0x10; }				/*P64=H 00010000*/

#define WE_L()		{ P6DR&=0xdf; }				/*P65=L 11011111*/
#define WE_H()		{ P6DR|=0x20; }				/*P65=H 00100000*/

#define BUS_IN()	{ P3DDR=0x00; }				/*P37~P30=INPUT*/
#define BUS_OUT()	{ P3DDR=0xff; }				/*P37~P30=OUTPUT*/

/* IP�A�h���X�⍇���p�P�b�g�̍\���� (ARP) */
typedef struct
{	/* Ethernet Header (14 byte)*/
	unsigned char	destMAC[6];				/* �����MAC�A�h���X�i���N�G�X�g���͂��ׂ�1�j */
	unsigned char	srcMAC[6];				/* ������MAC�A�h���X */
	unsigned short	etherType;				/* 0x0806 (ARP)*/
	/* ARP (28 byte)*/
	unsigned short	hardware;				/* 0x0001 (Ethernet10Mbps)*/
	unsigned short	protocol;				/* 0x0800 (IP)*/
	unsigned char	hLength;				/* 0x06 (MAC 6byte)*/
	unsigned char	pLength;				/* 0x04 (IP 4byte)*/
	unsigned short	operation;				/* 0x0001:request, 0x0002:reply*/
	unsigned char	srcMACarp[6];				/* ������MAC�A�h���X */
	unsigned char	srcIP[4];				/* ������IP�A�h���X */
	unsigned char	destMACarp[6];				/* �����MAC�A�h���X�i���N�G�X�g���͂��ׂ�0�j */
	unsigned char	destIP[4];				/* �����IP�A�h���X */
}	PacketARP;

/* ICMP�p�P�b�g�\����(ICMP) */
typedef struct{
	/* Ethernet Header (14 byte)*/
	unsigned char	destMAC[6];				/* �����MAC�A�h���X */
	unsigned char	srcMAC[6];				/* ������MAC�A�h���X */
	unsigned short	etherType;				/* 0x0800 (IP)*/

	/* IP Header (20 byte)*/
	unsigned char	vIP_hLength;				/* 0x45*/
	unsigned char	serviceType;				/* 0x00*/
	unsigned short	lengthIP;				/* 0x00c8�iIP�w�b�_�ȉ������܂��200 byte�j*/
	unsigned short	identification;
	unsigned short	flags_fragmentOffset;			/* 0x4000*/
	unsigned char	ttl;					/* 0x00 (0 hop)*/
	unsigned char	protocol;				/* 0x11 (UDP)*/
	unsigned short	checksumIP;				/* IP�w�b�_�̃`�F�b�N�T�� */
	unsigned char	srcIP[4];				/* ������IP�A�h���X */
	unsigned char	destIP[4];				/* �����IP�A�h���X */
	/* ICMP Header (8 byte)*/

	unsigned char	type;
	unsigned char	code;
	unsigned short	checksum;
	unsigned short	id	;
	unsigned short	sequence;
	
	unsigned char	PingData[1024];				
} PacketICMP;



void Wait_LCD ( unsigned long Interval );
void Init_LCD ( void );
void Clear_LCD ( void );
void Locate_LCD ( const unsigned char X, const unsigned char Y );
void Print_LCD ( const char *String );
void Write_LCD ( const unsigned char RS_Mode , const unsigned char Data );
void InitPort(void);
void LcdOut(unsigned char lcd_data);
void InitRTL8019AS(void);
unsigned char getbus(unsigned char );
void putbus(unsigned char ,unsigned char );
unsigned char databus(int ,unsigned char );
char keyin(void);
#pragma interrupt (Proc8kHz)
void Proc8kHz(void);
void SendArpReply(unsigned char *buff);
void SendPingMessage(unsigned char *buff);
void SendEtherData(unsigned char *buff, unsigned short count);
unsigned short GetChecksumIP(unsigned char *buff);
char ReceiveEtherData(void);
unsigned char CompareIP(unsigned char *pIP1, unsigned char *pIP2);
unsigned char CIP(unsigned char *pIP1, unsigned char *pIP2);


/*
/////////////////////////////////////////////////////////////////////////////////////////
//���萔�錾
*/
#define VAL_LCD_ROW		16	/* ���ő� */
#define VAL_LCD_COLUMN		2	/* �c�ő� */

#define VAL_RS_LOW		0
#define VAL_RS_HIGH		1

/*
/////////////////////////////////////////////////////////////////////////////////////////
//���^�錾
*/
typedef struct typeLCD
{
	union 
	{
		struct 
		{
			unsigned char X : 4;
			unsigned char Y : 4;
		} BIT;
		unsigned char BYTE;
	} CURSOR;
} TYPE_STRUCT_LCD;

unsigned char	Buffer[512];			/* �����f�[�^��M�p */


/*
/////////////////////////////////////////////////////////////////////////////////////////
//���ϐ��錾
*/
static TYPE_STRUCT_LCD m_LCDInfo;		/* �k�b�c��� */

int main(void){

const unsigned short onnkai[7][3]={{38223,19111,9556},	/*�h*/		/*�b*/
				{34052,17026,8513},	/*��*/		/*�c*/
				{30337,15169,7584},	/*�~*/		/*�d*/
				{28635,14317,7159},	/*�t�@*/	/*�e*/
				{25511,12755,6378},	/*�\*/		/*�f*/
				{22727,11364,5682},	/*��*/		/*�`*/
				{20248,10124,5062}};	/*�V*/		/*�a*/


	char asciicode[]="0123456789ABCDEF";
	int i;
	char keyCode[] = " ";
	char a;
	InitPort();						/* H8�|�[�g�֌W�̏����� */
	Init_LCD();						/* �t���\�����W���[���̏����� */
	InitRTL8019AS();					/* Ethernet�R���g���[���̏����� */
	Locate_LCD (0,0);
	set_imask_ccr( 0 );
/* A/D,D/A���[�v�e�X�g */
	TSTR |= 0x01;						/* �����X�^�[�g */
/* LCD �e�X�g */	
	Print_LCD ( "HOKUTODENSHI" );
/* SW15 ���[�h�ؑւr�v�e�X�g */
	if ((MDCR & 0x2) == 0){
		Locate_LCD (13,0);
		Print_LCD ( "B" );				/* �o�X���[�h */
	}else{
		Locate_LCD (13,0);
		Print_LCD ( "P" );				/* �V���O�����[�h */
	}
		Locate_LCD (13,0);
/* �}�g���b�N�X�L�[ �� BUZZ�e�X�g */
	while(1){
		switch(keyin()){
			case 0x01:
				GRA1 = onnkai[0][0];	/*�h*/
				GRB1 = onnkai[0][1];
				keyCode[0] = '1';		/* �P */
				break;
			case 0x02:
				GRA1 = onnkai[1][0];	/*��*/
				GRB1 = onnkai[1][1];
				keyCode[0] = '2';		/* �Q */
				break;
			case 0x04:
				GRA1 = onnkai[2][0];	/*�~*/
				GRB1 = onnkai[2][1];
				keyCode[0] = '3';		/* �R */
				break;
			case 0x08:
				GRA1 = onnkai[3][0];	/*�t�@*/
				GRB1 = onnkai[3][1];
				keyCode[0] = '4';		/* �S */
				break;
			
			case 0x11:
				GRA1 = onnkai[4][0];	/*�\*/
				GRB1 = onnkai[4][1];
				keyCode[0] = '5';		/* �T */
				break;
			case 0x12:
				GRA1 = onnkai[5][0];	/*��*/
				GRB1 = onnkai[5][1];
				keyCode[0] = '6';		/* �U */
				break;
			case 0x14:
				GRA1 = onnkai[6][0];	/*�V*/
				GRB1 = onnkai[6][1];
				keyCode[0] = '7';		/* �V */
				break;
			case 0x18:
				GRA1 = onnkai[0][1];	/*�h*/
				GRB1 = onnkai[0][2];
				keyCode[0] = '8';		/* �W */
				break;
			case 0x21:
				GRA1 = onnkai[1][1];	/*��*/
				GRB1 = onnkai[1][2];

				keyCode[0] = '9';		/* �X */
				break;
			case 0x22:
				GRA1 = onnkai[2][1];	/*�~*/
				GRB1 = onnkai[2][2];

				keyCode[0] = '.';		/* �D */
				break;
			case 0x24:
				GRA1 = onnkai[3][1];	/*�t�@*/
				GRB1 = onnkai[3][2];

				keyCode[0] = '0';		/* �O */
				break;
			case 0x28:
				GRA1 = onnkai[4][1];	/*�\*/
				GRB1 = onnkai[4][2];

				keyCode[0] = '#';		/* �� */
				break;
			default:
				GRA1 = 0xffff;
				GRB1 = 0xffff;
				keyCode[0] = ' ';		/* ���� */
		}
		Locate_LCD (0,1);
		Print_LCD ( keyCode );
/* ON OFF SW�e�X�g �� D8 D9 LED�e�X�g */
		if (((~P5DR) & 0x2) ==0)	PADR= ((PADR & 0xfb)|0x4);			/* �|�[�g����L�[�̏�Ԃ���� */
		else				PADR= ((PADR & 0xfb));				/* �Ή�����LED��ON�OFF */
		if (((~P5DR) & 0x1) ==0)	PADR= ((PADR & 0xf7)|0x8);			/* �|�[�g����L�[�̏�Ԃ���� */
		else				PADR= ((PADR & 0xf7));				/* �Ή�����LED��ON�OFF */
/* SW15 ID CODE �r�v�e�X�g */
		keyCode[0] = asciicode[((P2DR) & 0x7)];
		Locate_LCD (15,0);
		Print_LCD ( keyCode );
		a = ReceiveEtherData();
		
	}
}



/***********************************************************
**  �������Ɋւ��鏈��                                    **
***********************************************************/  

/* H8�|�[�g�֌W�̏����� */
void InitPort(void){

	/* port 1 �i�A�h���X�o�X�j */
	P1DDR = 0xff;
	P1DR = 0xff;
	/* port 2 �i�A�h���X�o�X�j*/
	P2DDR = 0x00;
	P2PCR = 0xff;
	/* port 3 �i�g�p���Ȃ��̂ŏo�͐ݒ�j*/
	P3DDR = 0xff;
	P3DR = 0xff;
	/* port 4 �i���4bit�̓L�[�X�L�����p���́C����4bit�̓L�[�X�L�����p�o�́j*/
	P4DDR = 0xf0;
	P4PCR = 0x0f;
	P4DR = 0xff;
	/* port 5 (4 bits)�i����2bit�̓L�[�X�L�����p�v���A�b�v�t�����́j*/
	P5DDR = 0xfc;
	P5PCR = 0x03;
	P5DR = 0xff;
	/* port 6 �i�g�p���Ȃ��̂ŏo�͐ݒ�j*/
	P6DDR = 0xff;
	P6DR = 0xff;
	/* port 7 (���͐�p) */
	/* AD�CDA�ƌ��p�D�g�p���Ă��Ȃ��[�q�͊O���Ńv���_�E������ */
	/* port 8 (5 bits) �i�g�p���Ȃ��̂ŏo�͐ݒ�j*/
	P8DDR = 0xff;
	P8DR = 0xff;
	/* port 9 (6 bits) */
	P9DDR = 0x13;	/* bit 5,3,2 are input for IRQ5, RXDs */
	P9DR = 0xff;
	/* port A �i���2bit��LED�_���p�o�́C���͎g�p���Ȃ��̂ŏo�͐ݒ�j*/
	PADDR = 0xff;
	PADR = 0xff;
	/* port B �i����2bit�C���4bit��LCD����p�o�́C���͎g�p���Ȃ��̂ŏo�͐ݒ�j*/
	PBDDR = 0xff;
	PBDR = 0x00;
	/* D/A�ϊ� */
	DACR = 0x40;						/* DA ch0 ��L�� */
	/* A/D�ϊ� */
	ADCSR = 0x00;						/* AD ch0���w�� */
	/* �o�X����֌W */
	WCRH= 0x00;						/* �A�N�Z�X���̃E�F�C�g��0 */
	WCRL= 0x00;
	/* 16bit�^�C�}�֌W */
	TSTR = 0xf8;						/* �^�C�}��~  */
	TSNC = 0x00;						/* �V���N���Ȃ�  */
	TMDR = 0x02;						/* �`���l��1��PWM ���[�h ���̑��ʏ퓮��  */
	/* CH0�i�����p�j*/
	TCR0 = 0xA0;						/* IMFA0�ŃN���A�C�����Ȃ�  */
	TIOR0 = 0x00;						/* ��v�`�F�b�N�C�[�q�o�͂Ȃ�  */
	TCNT0 = 0x0000;						/* �J�E���^�N���A  */
	GRA0 = CNT8KHZ;						/* �J�E���g�� for 8kHz */
	TISRA = 0x10;						/* IMFA0��v�Ŋ���  */
	/* CH1�iBUZZ�p�j*/
	TCR1 = 0xA2;						/* IMFA1�ŃN���A�C�S�����N���b�N  */
	TIOR1 = 0x00;						/* ��v�`�F�b�N�C�[�q�o�͂Ȃ�  */
	GRA1 = 0xffff;
	GRB1 = 0xffff;
	TSTR |= 0x02;						/* ch2�J�E���^�X�^�[�g  */

}

/*
///////////////////////////////////////////////////////////////////////////////
//��Init_LCD
//�E�k�b�c������
//-----------------------------------------------------------------------------
//return
//-----------------------------------------------------------------------------
//param
//-----------------------------------------------------------------------------
*/
void Init_LCD ( void )
{
	/* �ϐ������� */
	memset ( &m_LCDInfo, 0, sizeof ( TYPE_STRUCT_LCD ) );
	
	PBDR = 0x00;
	
	Wait_LCD ( 20 );
	
	PBDR = PBDR & ~0x02;	/* RS Low Set */

	/* LCD������1��� */
	PBDR = PBDR | 0x01;			/* E High Set */
	PBDR = ( PBDR & 0x0f ) | 0x30;	/*�������M�����M1��� */
	Wait_LCD ( 1 );
	PBDR = PBDR & ~0x01;	/* E Low Set */
	Wait_LCD ( 1 );
	
	Wait_LCD ( 5 );
	
	
	/* LCD������2��� */
	PBDR = PBDR | 0x01;	/* E High Set */
	PBDR = ( PBDR & 0x0f ) | 0x30;	/*�������M�����M2��� */
	Wait_LCD ( 1 );
	PBDR = PBDR & ~0x01;	/* E Low Set */
	Wait_LCD ( 1 );
	
	
	/* LCD������3��� */
	PBDR = PBDR | 0x01;	/* E High Set */
	PBDR = ( PBDR & 0x0f ) | 0x30;	/*�������M�����M3��� */
	Wait_LCD ( 1 );
	PBDR = PBDR & ~0x01;	/* E Low Set */
	Wait_LCD ( 1 );
	
	
	/* LCD������4��� */
	PBDR = PBDR | 0x01;	/* E High Set */
	PBDR = ( PBDR & 0x0f ) | 0x20;	/*�������M�����M4��� */
	Wait_LCD ( 1 );
	PBDR = PBDR & ~0x01;	/* E Low Set */
	Wait_LCD ( 1 );
	
	/* �t�@���N�V�����Z�b�g�F�S�r�b�g�o�X�A�Q�s�A�T�~�V�h�b�g */
	Write_LCD ( VAL_RS_LOW , 0x28 );
	
	/* �G���g���[���[�h�F�J�[�\���C���N�������g�A�J�[�\���ړ� */
	Write_LCD ( VAL_RS_LOW , 0x06 );

	/* �J�[�\��/�\���V�t�g�F�J�[�\���E�V�t�g */
	Write_LCD ( VAL_RS_LOW , 0x10 );

	/* �\��ON/OFF�R���g���[���F�f�B�X�v���C�\���A�J�[�\���\���A�u�����N�\�� */
	Write_LCD ( VAL_RS_LOW , 0x0c );

	/* �\���N���A */
	Clear_LCD ();
	
	Wait_LCD ( 5 );
	
}

/*
///////////////////////////////////////////////////////////////////////////////
//��Clear_LCD
//�@�k�b�c����F�\���N���A
//-----------------------------------------------------------------------------
//return
//-----------------------------------------------------------------------------
//param
//-----------------------------------------------------------------------------
*/
void Clear_LCD ( void )
{
	PBDR = PBDR & ~0x02;	/* RS Low Set */
	Write_LCD ( VAL_RS_LOW , 0x01 );
	Wait_LCD ( 2 );
	Locate_LCD ( 0 , 0 );
}


/*
///////////////////////////////////////////////////////////////////////////////
//��Locate_LCD
//�@�k�b�c����F�J�[�\���ړ�
//-----------------------------------------------------------------------------
//return
//-----------------------------------------------------------------------------
//param
//-----------------------------------------------------------------------------
*/
void Locate_LCD ( const unsigned char X, const unsigned char Y )
{
	unsigned char i;
	i = Y * 0x40 + X;
	i = i | 0x80;
	
	Write_LCD( VAL_RS_LOW , i );
	Wait_LCD ( 1 );
	
	/* �J�[�\���ۑ� */
	m_LCDInfo.CURSOR.BIT.X = X;
	m_LCDInfo.CURSOR.BIT.Y = Y;
	
}

/*
///////////////////////////////////////////////////////////////////////////////
//��Print_LCD
//�@�k�b�c����F������\��
//-----------------------------------------------------------------------------
//return
//-----------------------------------------------------------------------------
//param
//-----------------------------------------------------------------------------
*/
void Print_LCD ( const char *String )
{
	/* �ϐ��錾 */
	unsigned char i, j;
	
	
	/* ������\�����[�v */
	for ( i = 0; String[ i ] != '\0'; ++i )
	{
		/* �����`�� */
		if (
			( String[ i ] >= 0x20 && String[ i ] <= 0x7f ) ||
			( String[ i ] >= 0xa1 && String[ i ] <= 0xfd )
		)
		{/* �k�b�c�����R�[�h�͈͓̔��̏ꍇ */
			/* �����`�� */
			Write_LCD ( VAL_RS_HIGH, String[ i ] );
			Wait_LCD ( 1 );
			
			/* �J�[�\���ړ� */
			if ( m_LCDInfo.CURSOR.BIT.X == VAL_LCD_ROW  )
			{/* 16�s���I�[�o�[����ꍇ */
				m_LCDInfo.CURSOR.BIT.X = 0;
				m_LCDInfo.CURSOR.BIT.Y = ( m_LCDInfo.CURSOR.BIT.Y == 0 ? 1 : 0 );
				Locate_LCD ( m_LCDInfo.CURSOR.BIT.X, m_LCDInfo.CURSOR.BIT.Y );
				for ( j = 0; j < VAL_LCD_ROW; ++j )
					Write_LCD ( VAL_RS_HIGH, ' ' );
				Locate_LCD ( m_LCDInfo.CURSOR.BIT.X, m_LCDInfo.CURSOR.BIT.Y );
			}
			else
			{/* 16�s�ȓ��̏ꍇ */
				++m_LCDInfo.CURSOR.BIT.X;
				Locate_LCD ( m_LCDInfo.CURSOR.BIT.X, m_LCDInfo.CURSOR.BIT.Y );
			}/* if ( m_LCDInfo.CURSOR.BIT.X + 1 <= 0x0f ) */
		}
		else if ( String[ i ] == '\n' )
		{/* ���s�R�[�h�̏ꍇ */
			m_LCDInfo.CURSOR.BIT.X = 0;
			m_LCDInfo.CURSOR.BIT.Y = ( m_LCDInfo.CURSOR.BIT.Y == 0 ? 1 : 0 );
			Locate_LCD ( m_LCDInfo.CURSOR.BIT.X, m_LCDInfo.CURSOR.BIT.Y );
			for ( j = 0; j < VAL_LCD_ROW; ++j )
					Write_LCD ( VAL_RS_HIGH, ' ' );
			Locate_LCD ( m_LCDInfo.CURSOR.BIT.X, m_LCDInfo.CURSOR.BIT.Y );
		}
		else
		{/* �k�b�c�����R�[�h�͈̔͊O�̏ꍇ */
			
		}
	}
	
	
}

/*
///////////////////////////////////////////////////////////////////////////////
//��Write_LCD
//�@�k�b�c����F��������
//-----------------------------------------------------------------------------
//return
//-----------------------------------------------------------------------------
//param
//-----------------------------------------------------------------------------
*/
void Write_LCD (
	const unsigned char RS_Mode ,
	const unsigned char Data
)
{
	/* 4�r�b�g���̏ꍇ */
	/* RS Setting */
	if ( RS_Mode == VAL_RS_LOW )
	{
		PBDR = PBDR & ~0x02;	/* RS Low Set */
	}
	else if ( RS_Mode == VAL_RS_HIGH )
	{
		PBDR = PBDR | 0x02 ;	/* RS High Set */
	}
	
	/* ���4�r�b�g */
	
	PBDR = PBDR | 0x01;	/* E High Set */
	
	PBDR = ( PBDR & 0x0f ) | ( Data & 0xf0 );	/* ���4�r�b�g�������� */
	Wait_LCD ( 1 );
	PBDR = PBDR & ~0x01;	/* E Low Set */
	Wait_LCD ( 1 );
	
	/* ����4�r�b�g */
	PBDR = PBDR | 0x01;	/* E High Set */

	PBDR = ( PBDR & 0x0f ) | ( ( Data << 4 ) & 0xf0 );
	Wait_LCD ( 1 );
	PBDR = PBDR & ~0x01;	/* E Low Set */
	Wait_LCD ( 1 );
	
}

/*
///////////////////////////////////////////////////////////////////////////////
//��Wait_LCD
//�@�k�b�c�ҋ@
//-----------------------------------------------------------------------------
//return
//-----------------------------------------------------------------------------
//param
//-----------------------------------------------------------------------------
*/
void Wait_LCD ( unsigned long Interval )
{
	volatile unsigned long i;
	for ( i = 0; i < Interval * 100; ++i );
}

/*
///////////////////////////////////////////////////////////////////////////////
//��InitRTL8019AS
//�@�l�b�g���[�N�R���g���[������F������
//-----------------------------------------------------------------------------
//return
//-----------------------------------------------------------------------------
//param
//-----------------------------------------------------------------------------
*/
void InitRTL8019AS(void){

	unsigned char	i, status;

	PADR= PADR | 0x80;						/* �n�[�h���Z�b�g */
	Wait_LCD(1000);							/* ���Z�b�g�҂��iLCD���[�v���p�j */
	PADR= (PADR & 0x7f);						/* �n�[�h���Z�b�g */
	putbus(ResetPort,0xff);						/* ���Z�b�g */
	Wait_LCD(1000);							/* ���Z�b�g�҂��iLCD���[�v���p�j */

	putbus(Command_R,0x21);						/* �y�[�W0�CremoteDMA��~�C����OFF */
	putbus(DC_R,0x4a);						/* bos:1  big endan */
	putbus(RBC_R1,0);						/* counter H byte=0 */
	putbus(RBC_R0,0);						/* counter L byte=0 */
	putbus(IM_R,0);							/* mask all IRQ */
	putbus(IS_R,0xff);						/* set to clear irq */
	putbus(RC_R,0x20);						/* rcr.MON=1 */
	putbus(TC_R,0x02);						/* tcr.LB0=1 */
	putbus(TPS_R,0x40);						/* area 40-45 */
	putbus(PSTART_R,PSTART);					/* area	46-5f */
	putbus(PSTOP_R,PSTOP);						/* stop page 0x60 */
	putbus(BNRY_R,PSTART);						/* boundry=pstart */
	putbus(Command_R,0x61);						/* rd2,stp=1, 1page */
	putbus(CURR_R,PSTART+1);					/* current=boundry+1 */
	/* ���g��MAC�̐ݒ�iEEPROM����ǂݏo���ă��W�X�^�ɐݒ�j*/
	putbus(Command_R,0x21);						/* �y�[�W0�CremoteDMA��~�C����OFF */
	putbus(RSA_R0,0);						/* �X�^�[�g�A�h���X�̐ݒ� */
	putbus(RSA_R1,0);
	putbus(RBC_R0,12);						/* �]���o�C�g���̐ݒ� */
	putbus(RBC_R1,0);
	putbus(Command_R,0x0a);						/* �y�[�W0�CremoteDMA�ǂݏo���C����ON */
	for(i=0; i<6; i+=2){
		myMacAddress[i+1] = getbus(DataPort);
		status= getbus(DataPort);				/* Dummy Read*/
		myMacAddress[i] = getbus(DataPort);			/* �G���f�B�A���ϊ��ǉ� */
		status= getbus(DataPort);
	}
	do{
		status = getbus(IS_R);
	}	while(!(status & 0x40));				/* remoteDMA��~�܂ő҂� */
	putbus(Command_R,0x61);
	putbus(PA_R0,myMacAddress[0]);					/* MAC�A�h���X�̐ݒ� */
	putbus(PA_R1,myMacAddress[1]);
	putbus(PA_R2,myMacAddress[2]);
	putbus(PA_R3,myMacAddress[3]);
	putbus(PA_R4,myMacAddress[4]);
	putbus(PA_R5,myMacAddress[5]);

	/* �}���`�L���X�g�A�h���X�̐ݒ� */
	putbus(MA_R0,0);
	putbus(MA_R1,0);
	putbus(MA_R2,0);
	putbus(MA_R3,0);
	putbus(MA_R4,0);
	putbus(MA_R5,0);
	putbus(MA_R6,0);
	putbus(MA_R7,0);

	/* �ʏ퓮��ֈڍs */
	putbus(Command_R,0x21);						/* rd2,stp=1, 0page */
	putbus(RC_R,0x04);						/* rcr.ab=1 */
	putbus(TC_R,0);							/* clear tcr */
	putbus(IM_R,0);							/* disable interrupt */
	putbus(Command_R,0x22);
}

/*
///////////////////////////////////////////////////////////////////////////////
//��getbus
//�@�l�b�g���[�N�R���g���[������F������
//-----------------------------------------------------------------------------
//return databus
//-----------------------------------------------------------------------------
//param
//-----------------------------------------------------------------------------
*/
unsigned char getbus(unsigned char addr){

	unsigned char buf;
	int i;

	if ((MDCR & 0x2) == 0){
		pNIC= RTL8019;
		buf = pNIC[addr];
		return(buf);
	}else{
		P1DR = addr;						/* �A�h���X�o�� */
		BUS_IN();						/* �o�X����͂� */
		RD_L();
	
		buf =  databus(0,buf);
		RD_H();
		return(buf);
	}
}

/*
///////////////////////////////////////////////////////////////////////////////
//��InitRTL8019AS
//�@�l�b�g���[�N�R���g���[������F������
//-----------------------------------------------------------------------------
//return
//-----------------------------------------------------------------------------
//param
//-----------------------------------------------------------------------------
*/
void putbus(unsigned char addr,unsigned char data){

	int i;

	if ((MDCR & 0x2) == 0){
		pNIC= RTL8019;
		pNIC[addr]= data;
	}else{
		P1DR = addr;						/* �A�h���X�o�� */
		WE_L();							/* �C�l�[�u�� */
		BUS_OUT();						/* �o�X���o�͂� */
	
		data=databus(10,data);					/* �f�[�^�o�� */
		WE_H();							/* �f�B�Z�[�u�� */
		BUS_IN();						/* �o�X����͂� */
	}
}

/*
///////////////////////////////////////////////////////////////////////////////
//��InitRTL8019AS
//�@�l�b�g���[�N�R���g���[������F������
//-----------------------------------------------------------------------------
//return
//-----------------------------------------------------------------------------
//param
//-----------------------------------------------------------------------------
*/
unsigned char databus(int b,unsigned char dataa){

	if (b > 1) {
		P3DR=dataa;
	}else{
		dataa=P3DR;
	}
	return(dataa);
}

char keyin(void){

char i,data;
	for(i=0;i<3;i++){
		P4DR = ~(0x10 << i);
		data = (~P4DR & 0xf);
		if(data != 0){
			data += (i<<4);
			break;
		}
	}
	P4DR = 0xf0;
	return(data);

}

/***********************************************************
**  �����v���O�����i8kHz�����œ��삵�܂��j                **
***********************************************************/  
void Proc8kHz(void){
	TISRA &= 0xfe;							/* �^�C�}�����t���O�̃��Z�b�g */
	DADR0=ADDRAH;
	ADCSR = 0x20;

}
/* Ethernet�R���g���[�������M�p�P�b�g��ǂݏo�� */
char ReceiveEtherData(void){

	unsigned char	boundry, current, addH, nextP, nextEP, countL, countH, status;
	volatile unsigned short	i;
	unsigned short	count;
	char asciicode[]="0123456789ABCDEF ";
	char Code[] = "     ";
	char Code2[] = "   ";

	/* ��M�p�P�b�g�̗L�����`�F�b�N���� */
	putbus(Command_R,0x22);						/* �y�[�W0�CremoteDMA��~�C����ON */
	boundry = getbus(BNRY_R);
	putbus(Command_R,0x62);						/* �y�[�W1�CremoteDMA��~�C����ON */
	current = getbus(CURR_R);
	if(current < boundry){
		current += PSTOP - PSTART;
	}
	if(current == boundry+1){
		return 0;						/* �p�P�b�g�Ȃ��Ȃ�C�I��� */
	}
	boundry++;							/* �ǂݏo���y�[�W��addH�ɐݒ� */
	if(boundry==PSTOP){
		addH= PSTART;
	}
	else{
		addH = boundry;
	}
	/* �ŏ���4byte����]���o�C�g����ǂݏo�� */
	putbus(Command_R,0x22);						/* �y�[�W0�CremoteDMA��~�C����ON */
	putbus(RSA_R0,0);						/* �X�^�[�g�A�h���X�ݒ� */
	putbus(RSA_R1,addH);
	putbus(RBC_R0,4);						/* �]���o�C�g���ݒ� */
	putbus(RBC_R1,0);
	putbus(Command_R,0x0a);						/* �y�[�W0�CremoteDMA�Ǐo�C����ON */
	status = getbus(DataPort);
	for(i=0; i<2; i++);						/* waiting for rDMA internal process */
	nextP = getbus(DataPort);
	for(i=0; i<2; i++);						/* waiting for rDMA internal process */
	countL = getbus(DataPort);
	for(i=0; i<2; i++);						/* waiting for rDMA internal process */
	countH = getbus(DataPort);
	do{
		status = getbus(IS_R);
	}	while(!(status & 0x40));				/* remoteDMA��~�܂ő҂� */
	putbus(Command_R,0x22);						/* �y�[�W0�CremoteDMA��~�C����ON */
	/* ��M�p�P�b�g���̃`�F�b�N */
	nextEP= addH + countH + 1;					/* ���̃y�[�W�ʒu���v�Z */
	if(nextEP>=PSTOP){
	}
	if(nextP != nextEP){						/* �ǂݏo�����y�[�W�ƌv�Z�l�������Ă��邩 */
		putbus(BNRY_R,addH);
		nextEP= nextEP - PSTOP + PSTART;
		return 0;						/* �s���ȃp�P�b�g�̓y�[�W��i�߂ďI��� */
	}
	/* �擾�����o�C�g���ŉ��߂Ď�M�p�P�b�g��ǂݏo�� */
	putbus(RSA_R0,0);
	putbus(RSA_R1,addH);
	putbus(RBC_R0,countL);
	putbus(RBC_R1,countH);
	count = (unsigned short)countH*256+(unsigned short)countL;
	putbus(Command_R,0x0a);						/* �y�[�W0�CremoteDMA�Ǐo�C����ON */
	for(i=0; i<512; i++)	Buffer[i]=0;
	for(i=0; i<count; i++){
		if(i<4){
			Buffer[0]= getbus(DataPort);			/* ��ǂ� */
		}
		else if(i>512){
			Buffer[511]= getbus(DataPort);			/* �傫�����镔���͋�ǂ� */
		}else{
			Buffer[i-4]= getbus(DataPort);
		}
	}
	do{
		status = getbus(IS_R);
	}	while(!(status & 0x40));				/* remoteDMA��~�܂ő҂� */
	putbus(Command_R,0x22);						/* �y�[�W0�CremoteDMA��~�C����ON */
	boundry= addH + countH;
	if(boundry>=PSTOP){						/* ���̃p�P�b�g�ǂݏo���ʒu�ݒ� */
		putbus(BNRY_R,boundry - PSTOP + PSTART);
	}
	else{
		putbus(BNRY_R,boundry);
	}
	/* �p�P�b�g�̎�ʃ`�F�b�N */
		Locate_LCD (12,0);
		Print_LCD ( " " );
	if((unsigned short)(((PacketICMP *)Buffer)->etherType)==(unsigned short)0x0806){				/* ARP�̏ꍇ */
		if(CompareIP((((PacketARP *)Buffer)->destIP), myIpAddress)==0){	/* ����`�F�b�N */
			if(((PacketARP *)Buffer)->operation==0x0001){		/* ���N�G�X�g�̏ꍇ */
				SendArpReply(Buffer);			/* ���X�|���X�̑��M */
				return 0;
			}
		}
		else{							/* ���̑���ARP�p�P�b�g�͖��� */
			return 0;
		}
	}else if((unsigned short)(((PacketICMP *)Buffer)->etherType)==(unsigned short)0x0800){			/* IP�p�P�b�g��M�̎� */
		if(CompareIP((((PacketICMP *)Buffer)->destIP), myIpAddress) == 0){	/* ����`�F�b�N */
			switch(((PacketICMP *)Buffer)->protocol){			/* �v���g�R������ */
				case 0x01:		/* ICMP */
					Locate_LCD (13,0);
					Print_LCD ( "d" );
					if(((PacketICMP *)Buffer)->type==0x08){	/* �v�����L�鎞���� */
						SendPingMessage(Buffer);
						return 0;
					}
					break;
				case 0x06:		/* TCP */
					break;
				case 0x11:		/* UDP */
					break;
				default:
					break;
			}


		}

	}
	return 0;
}


/* IP�A�h���X�̈�v�`�F�b�N */
unsigned char CompareIP(unsigned char *pIP1, unsigned char *pIP2){

	unsigned char	i;
	
	for(i=0; i<4; i++){
		if(pIP1[i] != pIP2[i]){
			return (1);
		}
	}
	return(0);
}

/* IP�A�h���X�̈�v�`�F�b�N */
unsigned char CIP(unsigned char *pIP1, unsigned char *pIP2){

	unsigned char	i;
	char asciicode[]="0123456789ABCDEF ";
	char Code[] = "  .";
	
	Locate_LCD(0,0);
	for(i=0; i<4; i++){
		Code[0] = asciicode[((pIP1[i]>>4) & 0xf)];
		Code[1] = asciicode[((pIP1[i]   ) & 0xf)];
		Print_LCD ( Code );
	}
	return(0);
}




/* MAC�A�h���X�̖₢���킹�ɓ����� */
void SendArpReply(unsigned char *buff){

	unsigned short	i;
	
	for(i=0; i<6; i++){						/* MAC�A�h���X�̐ݒ� */
		((PacketARP *)buff)->destMAC[i]= ((PacketARP *)buff)->srcMAC[i];
		((PacketARP *)buff)->destMACarp[i]= ((PacketARP *)buff)->srcMAC[i];
		((PacketARP *)buff)->srcMAC[i]= myMacAddress[i];
		((PacketARP *)buff)->srcMACarp[i]= myMacAddress[i];
	}
	for(i=0; i<4; i++){						/* IP�A�h���X�̐ݒ� */
		((PacketARP *)buff)->destIP[i]= ((PacketARP *)buff)->srcIP[i];
		((PacketARP *)buff)->srcIP[i]= myIpAddress[i];
	}
	((PacketARP *)buff)->operation= 0x0002;				/* ARP���� */
	SendEtherData(buff, 60);
}


void SendPingMessage(unsigned char *buff){

	unsigned char	i;
	unsigned short	l;
	unsigned long	sum;
	
	
	((PacketICMP *)buff)->type = 0x00;				/* �t�B�[���h�^�C�v */
	((PacketICMP *)buff)->code = 0x00;				/* �R�[�h�B�v���A��������00h�ƂȂ� */
	((PacketICMP *)buff)->checksum = 0x0000;			/* �`�F�b�N�T��(��Ōv�Z) */
	for(i=0; i<6; i++){						/* MAC�A�h���X�̐ݒ� */
		((PacketICMP *)buff)->destMAC[i]= ((PacketICMP *)buff)->srcMAC[i];
		((PacketICMP *)buff)->srcMAC[i]= myMacAddress[i];
	}
	for(i=0; i<4; i++){						/* IP�A�h���X�̐ݒ� */
		((PacketICMP *)buff)->destIP[i]= ((PacketICMP *)buff)->srcIP[i];
		((PacketICMP *)buff)->srcIP[i]= myIpAddress[i];
	}
	((PacketICMP *)buff)->identification= TCNT2;
	((PacketICMP *)buff)->checksumIP= 0;
	((PacketICMP *)buff)->checksumIP= GetChecksumIP(buff);		/* IP�`�F�b�N�T���̌v�Z */


	sum=0;
	for(l=34;(l+1)<((((PacketICMP *)buff)->lengthIP)+14);l+=2){
		sum += (((unsigned short)buff[l] << 8) + (unsigned short)buff[l+1]);
	}
	if( (l+1) == ((((PacketICMP *)buff)->lengthIP+14)) ){
		sum += ((unsigned short)buff[l] << 8);
	}
	sum = (sum & 0xffff) + (sum >> 16);
	sum = (sum & 0xffff) + (sum >> 16);

	((PacketICMP *)buff)->checksum = (unsigned short)~sum;
	SendEtherData(buff, (((PacketICMP *)buff)->lengthIP+14));

}

/* Ethernet�R���g���[���փp�P�b�g����������ő��M���� */
void SendEtherData(unsigned char *buff, unsigned short count){

	volatile unsigned short	i;
	unsigned char	status, countH, countL;

	countH = (unsigned char)(count >> 8);				/* �]���f�[�^�����o�C�g�ɕ��� */
	countL = (unsigned char)(count & 0x00ff);
	putbus(Command_R,0x22);						/* �y�[�W0�CremoteDMA��~�C����ON */
	do{
		status = getbus(IS_R);
	}	while(!(status & 0x40));				/* remoteDMA��~�܂ő҂� */
	/* �����[�gDMA�̐ݒ� */
	putbus(RSA_R0,0x0);						/* �X�^�[�g�A�h���X�ݒ� */
	putbus(RSA_R1,0x40);
	putbus(RBC_R0,countL);						/* �]���o�C�g���ݒ� */
	putbus(RBC_R1,countH);
	putbus(Command_R,0x12);						/* �y�[�W0�CremoteDMA�����C����ON */
	for(i= 0; i<(unsigned short)count; i++){
		putbus(DataPort,buff[i]);
	}
	do{
		status = getbus(IS_R);
	}	while(!(status & 0x40));				/* remoteDMA�����܂ő҂� */
	putbus(Command_R,0x22);						/* �y�[�W0�CremoteDMA��~�C����ON */
	putbus(TPS_R,0x40);						/* ���M�y�[�W�ݒ� */
	putbus(TBC_R0,countL);						/* ���M�o�C�g���ݒ� */
	putbus(TBC_R1,countH);
	putbus(Command_R,0x26);						/* �y�[�W0�CremoteDMA��~�C�p�P�b�g���M�C����ON */
}


/* IP�w�b�_�̃`�F�b�N�T���v�Z */
unsigned short GetChecksumIP(unsigned char *buff){

	unsigned long	sum;
	unsigned char	i;

	sum= 0;
	for(i=14; i<34; i+=2){
		sum += (((unsigned short)buff[i] << 8) + (unsigned short)buff[i+1]);
		sum = (sum & 0x0000ffff)+((sum & 0xffff0000)>>16);
	}
	if ((sum & 0xffff0000) != 0)	sum = (sum & 0x0000ffff)+((sum & 0xffff0000)>>16);
	return ((unsigned short)~sum);
}
