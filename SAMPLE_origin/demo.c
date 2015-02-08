/**************************************************************************************************
このソフトウェアはフリーウェアです。
著作権は作成元である(株)北斗電子が所有するものとします。
このソフトウェアを使用したことによる全ての損害について、
(株)北斗電子は一切の責任を負わないものとします。
このソフトウェアの一部または全てを無断で転載することを
禁止します。雑誌などへ紹介・収録の場合は連絡をお願い致
します。
Copyright (C) Hokuto denshi Co,Ltd. 2008
**************************************************************************************************/
#include "H8_CPU.h"
#include "RTL8019AS.h"
#include <machine.h>

#define		CPUCLK		20000000L
#define		SMPLING		8000
#define		CNT8KHZ		(unsigned short)(CPUCLK/SMPLING)
#define		CNTBASE		(unsigned short)(CPUCLK/40000)

unsigned char	myMacAddress[6];				/* 自分のMACアドレス */
const unsigned char	myIpAddress[4] = {192,168,0,10};		/* 自分のIPCアドレス */
unsigned char	destMacAddress[6];				/* 相手のMACアドレス */
#define RD_L()		{ P6DR&=0xef; }				/*P64=L 11101111*/
#define RD_H()		{ P6DR|=0x10; }				/*P64=H 00010000*/

#define WE_L()		{ P6DR&=0xdf; }				/*P65=L 11011111*/
#define WE_H()		{ P6DR|=0x20; }				/*P65=H 00100000*/

#define BUS_IN()	{ P3DDR=0x00; }				/*P37~P30=INPUT*/
#define BUS_OUT()	{ P3DDR=0xff; }				/*P37~P30=OUTPUT*/

/* IPアドレス問合せパケットの構造体 (ARP) */
typedef struct
{	/* Ethernet Header (14 byte)*/
	unsigned char	destMAC[6];				/* 相手のMACアドレス（リクエスト時はすべて1） */
	unsigned char	srcMAC[6];				/* 自分のMACアドレス */
	unsigned short	etherType;				/* 0x0806 (ARP)*/
	/* ARP (28 byte)*/
	unsigned short	hardware;				/* 0x0001 (Ethernet10Mbps)*/
	unsigned short	protocol;				/* 0x0800 (IP)*/
	unsigned char	hLength;				/* 0x06 (MAC 6byte)*/
	unsigned char	pLength;				/* 0x04 (IP 4byte)*/
	unsigned short	operation;				/* 0x0001:request, 0x0002:reply*/
	unsigned char	srcMACarp[6];				/* 自分のMACアドレス */
	unsigned char	srcIP[4];				/* 自分のIPアドレス */
	unsigned char	destMACarp[6];				/* 相手のMACアドレス（リクエスト時はすべて0） */
	unsigned char	destIP[4];				/* 相手のIPアドレス */
}	PacketARP;

/* ICMPパケット構造体(ICMP) */
typedef struct{
	/* Ethernet Header (14 byte)*/
	unsigned char	destMAC[6];				/* 相手のMACアドレス */
	unsigned char	srcMAC[6];				/* 自分のMACアドレス */
	unsigned short	etherType;				/* 0x0800 (IP)*/

	/* IP Header (20 byte)*/
	unsigned char	vIP_hLength;				/* 0x45*/
	unsigned char	serviceType;				/* 0x00*/
	unsigned short	lengthIP;				/* 0x00c8（IPヘッダ以下音声含んで200 byte）*/
	unsigned short	identification;
	unsigned short	flags_fragmentOffset;			/* 0x4000*/
	unsigned char	ttl;					/* 0x00 (0 hop)*/
	unsigned char	protocol;				/* 0x11 (UDP)*/
	unsigned short	checksumIP;				/* IPヘッダのチェックサム */
	unsigned char	srcIP[4];				/* 自分のIPアドレス */
	unsigned char	destIP[4];				/* 相手のIPアドレス */
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
//■定数宣言
*/
#define VAL_LCD_ROW		16	/* 横最大 */
#define VAL_LCD_COLUMN		2	/* 縦最大 */

#define VAL_RS_LOW		0
#define VAL_RS_HIGH		1

/*
/////////////////////////////////////////////////////////////////////////////////////////
//■型宣言
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

unsigned char	Buffer[512];			/* 音声データ受信用 */


/*
/////////////////////////////////////////////////////////////////////////////////////////
//■変数宣言
*/
static TYPE_STRUCT_LCD m_LCDInfo;		/* ＬＣＤ情報 */

int main(void){

const unsigned short onnkai[7][3]={{38223,19111,9556},	/*ド*/		/*Ｃ*/
				{34052,17026,8513},	/*レ*/		/*Ｄ*/
				{30337,15169,7584},	/*ミ*/		/*Ｅ*/
				{28635,14317,7159},	/*ファ*/	/*Ｆ*/
				{25511,12755,6378},	/*ソ*/		/*Ｇ*/
				{22727,11364,5682},	/*ラ*/		/*Ａ*/
				{20248,10124,5062}};	/*シ*/		/*Ｂ*/


	char asciicode[]="0123456789ABCDEF";
	int i;
	char keyCode[] = " ";
	char a;
	InitPort();						/* H8ポート関係の初期化 */
	Init_LCD();						/* 液晶表示モジュールの初期化 */
	InitRTL8019AS();					/* Ethernetコントローラの初期化 */
	Locate_LCD (0,0);
	set_imask_ccr( 0 );
/* A/D,D/Aループテスト */
	TSTR |= 0x01;						/* 割込スタート */
/* LCD テスト */	
	Print_LCD ( "HOKUTODENSHI" );
/* SW15 モード切替ＳＷテスト */
	if ((MDCR & 0x2) == 0){
		Locate_LCD (13,0);
		Print_LCD ( "B" );				/* バスモード */
	}else{
		Locate_LCD (13,0);
		Print_LCD ( "P" );				/* シングルモード */
	}
		Locate_LCD (13,0);
/* マトリックスキー ＆ BUZZテスト */
	while(1){
		switch(keyin()){
			case 0x01:
				GRA1 = onnkai[0][0];	/*ド*/
				GRB1 = onnkai[0][1];
				keyCode[0] = '1';		/* １ */
				break;
			case 0x02:
				GRA1 = onnkai[1][0];	/*レ*/
				GRB1 = onnkai[1][1];
				keyCode[0] = '2';		/* ２ */
				break;
			case 0x04:
				GRA1 = onnkai[2][0];	/*ミ*/
				GRB1 = onnkai[2][1];
				keyCode[0] = '3';		/* ３ */
				break;
			case 0x08:
				GRA1 = onnkai[3][0];	/*ファ*/
				GRB1 = onnkai[3][1];
				keyCode[0] = '4';		/* ４ */
				break;
			
			case 0x11:
				GRA1 = onnkai[4][0];	/*ソ*/
				GRB1 = onnkai[4][1];
				keyCode[0] = '5';		/* ５ */
				break;
			case 0x12:
				GRA1 = onnkai[5][0];	/*ラ*/
				GRB1 = onnkai[5][1];
				keyCode[0] = '6';		/* ６ */
				break;
			case 0x14:
				GRA1 = onnkai[6][0];	/*シ*/
				GRB1 = onnkai[6][1];
				keyCode[0] = '7';		/* ７ */
				break;
			case 0x18:
				GRA1 = onnkai[0][1];	/*ド*/
				GRB1 = onnkai[0][2];
				keyCode[0] = '8';		/* ８ */
				break;
			case 0x21:
				GRA1 = onnkai[1][1];	/*レ*/
				GRB1 = onnkai[1][2];

				keyCode[0] = '9';		/* ９ */
				break;
			case 0x22:
				GRA1 = onnkai[2][1];	/*ミ*/
				GRB1 = onnkai[2][2];

				keyCode[0] = '.';		/* ． */
				break;
			case 0x24:
				GRA1 = onnkai[3][1];	/*ファ*/
				GRB1 = onnkai[3][2];

				keyCode[0] = '0';		/* ０ */
				break;
			case 0x28:
				GRA1 = onnkai[4][1];	/*ソ*/
				GRB1 = onnkai[4][2];

				keyCode[0] = '#';		/* ＃ */
				break;
			default:
				GRA1 = 0xffff;
				GRB1 = 0xffff;
				keyCode[0] = ' ';		/* 無効 */
		}
		Locate_LCD (0,1);
		Print_LCD ( keyCode );
/* ON OFF SWテスト ＆ D8 D9 LEDテスト */
		if (((~P5DR) & 0x2) ==0)	PADR= ((PADR & 0xfb)|0x4);			/* ポートからキーの状態を入力 */
		else				PADR= ((PADR & 0xfb));				/* 対応するLEDをON･OFF */
		if (((~P5DR) & 0x1) ==0)	PADR= ((PADR & 0xf7)|0x8);			/* ポートからキーの状態を入力 */
		else				PADR= ((PADR & 0xf7));				/* 対応するLEDをON･OFF */
/* SW15 ID CODE ＳＷテスト */
		keyCode[0] = asciicode[((P2DR) & 0x7)];
		Locate_LCD (15,0);
		Print_LCD ( keyCode );
		a = ReceiveEtherData();
		
	}
}



/***********************************************************
**  初期化に関する処理                                    **
***********************************************************/  

/* H8ポート関係の初期化 */
void InitPort(void){

	/* port 1 （アドレスバス） */
	P1DDR = 0xff;
	P1DR = 0xff;
	/* port 2 （アドレスバス）*/
	P2DDR = 0x00;
	P2PCR = 0xff;
	/* port 3 （使用しないので出力設定）*/
	P3DDR = 0xff;
	P3DR = 0xff;
	/* port 4 （上位4bitはキースキャン用入力，下位4bitはキースキャン用出力）*/
	P4DDR = 0xf0;
	P4PCR = 0x0f;
	P4DR = 0xff;
	/* port 5 (4 bits)（下位2bitはキースキャン用プルアップ付き入力）*/
	P5DDR = 0xfc;
	P5PCR = 0x03;
	P5DR = 0xff;
	/* port 6 （使用しないので出力設定）*/
	P6DDR = 0xff;
	P6DR = 0xff;
	/* port 7 (入力専用) */
	/* AD，DAと兼用．使用していない端子は外部でプルダウンする */
	/* port 8 (5 bits) （使用しないので出力設定）*/
	P8DDR = 0xff;
	P8DR = 0xff;
	/* port 9 (6 bits) */
	P9DDR = 0x13;	/* bit 5,3,2 are input for IRQ5, RXDs */
	P9DR = 0xff;
	/* port A （上位2bitはLED点灯用出力，他は使用しないので出力設定）*/
	PADDR = 0xff;
	PADR = 0xff;
	/* port B （下位2bit，上位4bitはLCD制御用出力，他は使用しないので出力設定）*/
	PBDDR = 0xff;
	PBDR = 0x00;
	/* D/A変換 */
	DACR = 0x40;						/* DA ch0 を有効 */
	/* A/D変換 */
	ADCSR = 0x00;						/* AD ch0を指定 */
	/* バス制御関係 */
	WCRH= 0x00;						/* アクセス時のウェイトは0 */
	WCRL= 0x00;
	/* 16bitタイマ関係 */
	TSTR = 0xf8;						/* タイマ停止  */
	TSNC = 0x00;						/* シンクロなし  */
	TMDR = 0x02;						/* チャネル1はPWM モード その他通常動作  */
	/* CH0（割込用）*/
	TCR0 = 0xA0;						/* IMFA0でクリア，分周なし  */
	TIOR0 = 0x00;						/* 一致チェック，端子出力なし  */
	TCNT0 = 0x0000;						/* カウンタクリア  */
	GRA0 = CNT8KHZ;						/* カウント数 for 8kHz */
	TISRA = 0x10;						/* IMFA0一致で割込  */
	/* CH1（BUZZ用）*/
	TCR1 = 0xA2;						/* IMFA1でクリア，４分周クロック  */
	TIOR1 = 0x00;						/* 一致チェック，端子出力なし  */
	GRA1 = 0xffff;
	GRB1 = 0xffff;
	TSTR |= 0x02;						/* ch2カウンタスタート  */

}

/*
///////////////////////////////////////////////////////////////////////////////
//■Init_LCD
//・ＬＣＤ初期化
//-----------------------------------------------------------------------------
//return
//-----------------------------------------------------------------------------
//param
//-----------------------------------------------------------------------------
*/
void Init_LCD ( void )
{
	/* 変数初期化 */
	memset ( &m_LCDInfo, 0, sizeof ( TYPE_STRUCT_LCD ) );
	
	PBDR = 0x00;
	
	Wait_LCD ( 20 );
	
	PBDR = PBDR & ~0x02;	/* RS Low Set */

	/* LCD初期化1回目 */
	PBDR = PBDR | 0x01;			/* E High Set */
	PBDR = ( PBDR & 0x0f ) | 0x30;	/*初期化信号送信1回目 */
	Wait_LCD ( 1 );
	PBDR = PBDR & ~0x01;	/* E Low Set */
	Wait_LCD ( 1 );
	
	Wait_LCD ( 5 );
	
	
	/* LCD初期化2回目 */
	PBDR = PBDR | 0x01;	/* E High Set */
	PBDR = ( PBDR & 0x0f ) | 0x30;	/*初期化信号送信2回目 */
	Wait_LCD ( 1 );
	PBDR = PBDR & ~0x01;	/* E Low Set */
	Wait_LCD ( 1 );
	
	
	/* LCD初期化3回目 */
	PBDR = PBDR | 0x01;	/* E High Set */
	PBDR = ( PBDR & 0x0f ) | 0x30;	/*初期化信号送信3回目 */
	Wait_LCD ( 1 );
	PBDR = PBDR & ~0x01;	/* E Low Set */
	Wait_LCD ( 1 );
	
	
	/* LCD初期化4回目 */
	PBDR = PBDR | 0x01;	/* E High Set */
	PBDR = ( PBDR & 0x0f ) | 0x20;	/*初期化信号送信4回目 */
	Wait_LCD ( 1 );
	PBDR = PBDR & ~0x01;	/* E Low Set */
	Wait_LCD ( 1 );
	
	/* ファンクションセット：４ビットバス、２行、５×７ドット */
	Write_LCD ( VAL_RS_LOW , 0x28 );
	
	/* エントリーモード：カーソルインクリメント、カーソル移動 */
	Write_LCD ( VAL_RS_LOW , 0x06 );

	/* カーソル/表示シフト：カーソル右シフト */
	Write_LCD ( VAL_RS_LOW , 0x10 );

	/* 表示ON/OFFコントロール：ディスプレイ表示、カーソル表示、ブリンク表示 */
	Write_LCD ( VAL_RS_LOW , 0x0c );

	/* 表示クリア */
	Clear_LCD ();
	
	Wait_LCD ( 5 );
	
}

/*
///////////////////////////////////////////////////////////////////////////////
//■Clear_LCD
//　ＬＣＤ制御：表示クリア
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
//■Locate_LCD
//　ＬＣＤ制御：カーソル移動
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
	
	/* カーソル保存 */
	m_LCDInfo.CURSOR.BIT.X = X;
	m_LCDInfo.CURSOR.BIT.Y = Y;
	
}

/*
///////////////////////////////////////////////////////////////////////////////
//■Print_LCD
//　ＬＣＤ制御：文字列表示
//-----------------------------------------------------------------------------
//return
//-----------------------------------------------------------------------------
//param
//-----------------------------------------------------------------------------
*/
void Print_LCD ( const char *String )
{
	/* 変数宣言 */
	unsigned char i, j;
	
	
	/* 文字列表示ループ */
	for ( i = 0; String[ i ] != '\0'; ++i )
	{
		/* 文字描画 */
		if (
			( String[ i ] >= 0x20 && String[ i ] <= 0x7f ) ||
			( String[ i ] >= 0xa1 && String[ i ] <= 0xfd )
		)
		{/* ＬＣＤ文字コードの範囲内の場合 */
			/* 文字描画 */
			Write_LCD ( VAL_RS_HIGH, String[ i ] );
			Wait_LCD ( 1 );
			
			/* カーソル移動 */
			if ( m_LCDInfo.CURSOR.BIT.X == VAL_LCD_ROW  )
			{/* 16行をオーバーする場合 */
				m_LCDInfo.CURSOR.BIT.X = 0;
				m_LCDInfo.CURSOR.BIT.Y = ( m_LCDInfo.CURSOR.BIT.Y == 0 ? 1 : 0 );
				Locate_LCD ( m_LCDInfo.CURSOR.BIT.X, m_LCDInfo.CURSOR.BIT.Y );
				for ( j = 0; j < VAL_LCD_ROW; ++j )
					Write_LCD ( VAL_RS_HIGH, ' ' );
				Locate_LCD ( m_LCDInfo.CURSOR.BIT.X, m_LCDInfo.CURSOR.BIT.Y );
			}
			else
			{/* 16行以内の場合 */
				++m_LCDInfo.CURSOR.BIT.X;
				Locate_LCD ( m_LCDInfo.CURSOR.BIT.X, m_LCDInfo.CURSOR.BIT.Y );
			}/* if ( m_LCDInfo.CURSOR.BIT.X + 1 <= 0x0f ) */
		}
		else if ( String[ i ] == '\n' )
		{/* 改行コードの場合 */
			m_LCDInfo.CURSOR.BIT.X = 0;
			m_LCDInfo.CURSOR.BIT.Y = ( m_LCDInfo.CURSOR.BIT.Y == 0 ? 1 : 0 );
			Locate_LCD ( m_LCDInfo.CURSOR.BIT.X, m_LCDInfo.CURSOR.BIT.Y );
			for ( j = 0; j < VAL_LCD_ROW; ++j )
					Write_LCD ( VAL_RS_HIGH, ' ' );
			Locate_LCD ( m_LCDInfo.CURSOR.BIT.X, m_LCDInfo.CURSOR.BIT.Y );
		}
		else
		{/* ＬＣＤ文字コードの範囲外の場合 */
			
		}
	}
	
	
}

/*
///////////////////////////////////////////////////////////////////////////////
//■Write_LCD
//　ＬＣＤ制御：書き込み
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
	/* 4ビット長の場合 */
	/* RS Setting */
	if ( RS_Mode == VAL_RS_LOW )
	{
		PBDR = PBDR & ~0x02;	/* RS Low Set */
	}
	else if ( RS_Mode == VAL_RS_HIGH )
	{
		PBDR = PBDR | 0x02 ;	/* RS High Set */
	}
	
	/* 上位4ビット */
	
	PBDR = PBDR | 0x01;	/* E High Set */
	
	PBDR = ( PBDR & 0x0f ) | ( Data & 0xf0 );	/* 上位4ビット書き込み */
	Wait_LCD ( 1 );
	PBDR = PBDR & ~0x01;	/* E Low Set */
	Wait_LCD ( 1 );
	
	/* 下位4ビット */
	PBDR = PBDR | 0x01;	/* E High Set */

	PBDR = ( PBDR & 0x0f ) | ( ( Data << 4 ) & 0xf0 );
	Wait_LCD ( 1 );
	PBDR = PBDR & ~0x01;	/* E Low Set */
	Wait_LCD ( 1 );
	
}

/*
///////////////////////////////////////////////////////////////////////////////
//■Wait_LCD
//　ＬＣＤ待機
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
//■InitRTL8019AS
//　ネットワークコントローラ制御：初期化
//-----------------------------------------------------------------------------
//return
//-----------------------------------------------------------------------------
//param
//-----------------------------------------------------------------------------
*/
void InitRTL8019AS(void){

	unsigned char	i, status;

	PADR= PADR | 0x80;						/* ハードリセット */
	Wait_LCD(1000);							/* リセット待ち（LCDループ流用） */
	PADR= (PADR & 0x7f);						/* ハードリセット */
	putbus(ResetPort,0xff);						/* リセット */
	Wait_LCD(1000);							/* リセット待ち（LCDループ流用） */

	putbus(Command_R,0x21);						/* ページ0，remoteDMA停止，動作OFF */
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
	/* 自身のMACの設定（EEPROMから読み出してレジスタに設定）*/
	putbus(Command_R,0x21);						/* ページ0，remoteDMA停止，動作OFF */
	putbus(RSA_R0,0);						/* スタートアドレスの設定 */
	putbus(RSA_R1,0);
	putbus(RBC_R0,12);						/* 転送バイト数の設定 */
	putbus(RBC_R1,0);
	putbus(Command_R,0x0a);						/* ページ0，remoteDMA読み出し，動作ON */
	for(i=0; i<6; i+=2){
		myMacAddress[i+1] = getbus(DataPort);
		status= getbus(DataPort);				/* Dummy Read*/
		myMacAddress[i] = getbus(DataPort);			/* エンディアン変換追加 */
		status= getbus(DataPort);
	}
	do{
		status = getbus(IS_R);
	}	while(!(status & 0x40));				/* remoteDMA停止まで待つ */
	putbus(Command_R,0x61);
	putbus(PA_R0,myMacAddress[0]);					/* MACアドレスの設定 */
	putbus(PA_R1,myMacAddress[1]);
	putbus(PA_R2,myMacAddress[2]);
	putbus(PA_R3,myMacAddress[3]);
	putbus(PA_R4,myMacAddress[4]);
	putbus(PA_R5,myMacAddress[5]);

	/* マルチキャストアドレスの設定 */
	putbus(MA_R0,0);
	putbus(MA_R1,0);
	putbus(MA_R2,0);
	putbus(MA_R3,0);
	putbus(MA_R4,0);
	putbus(MA_R5,0);
	putbus(MA_R6,0);
	putbus(MA_R7,0);

	/* 通常動作へ移行 */
	putbus(Command_R,0x21);						/* rd2,stp=1, 0page */
	putbus(RC_R,0x04);						/* rcr.ab=1 */
	putbus(TC_R,0);							/* clear tcr */
	putbus(IM_R,0);							/* disable interrupt */
	putbus(Command_R,0x22);
}

/*
///////////////////////////////////////////////////////////////////////////////
//■getbus
//　ネットワークコントローラ制御：初期化
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
		P1DR = addr;						/* アドレス出力 */
		BUS_IN();						/* バスを入力に */
		RD_L();
	
		buf =  databus(0,buf);
		RD_H();
		return(buf);
	}
}

/*
///////////////////////////////////////////////////////////////////////////////
//■InitRTL8019AS
//　ネットワークコントローラ制御：初期化
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
		P1DR = addr;						/* アドレス出力 */
		WE_L();							/* イネーブル */
		BUS_OUT();						/* バスを出力に */
	
		data=databus(10,data);					/* データ出力 */
		WE_H();							/* ディセーブル */
		BUS_IN();						/* バスを入力に */
	}
}

/*
///////////////////////////////////////////////////////////////////////////////
//■InitRTL8019AS
//　ネットワークコントローラ制御：初期化
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
**  割込プログラム（8kHz周期で動作します）                **
***********************************************************/  
void Proc8kHz(void){
	TISRA &= 0xfe;							/* タイマ割込フラグのリセット */
	DADR0=ADDRAH;
	ADCSR = 0x20;

}
/* Ethernetコントローラから受信パケットを読み出す */
char ReceiveEtherData(void){

	unsigned char	boundry, current, addH, nextP, nextEP, countL, countH, status;
	volatile unsigned short	i;
	unsigned short	count;
	char asciicode[]="0123456789ABCDEF ";
	char Code[] = "     ";
	char Code2[] = "   ";

	/* 受信パケットの有無をチェックする */
	putbus(Command_R,0x22);						/* ページ0，remoteDMA停止，動作ON */
	boundry = getbus(BNRY_R);
	putbus(Command_R,0x62);						/* ページ1，remoteDMA停止，動作ON */
	current = getbus(CURR_R);
	if(current < boundry){
		current += PSTOP - PSTART;
	}
	if(current == boundry+1){
		return 0;						/* パケットなしなら，終わり */
	}
	boundry++;							/* 読み出しページをaddHに設定 */
	if(boundry==PSTOP){
		addH= PSTART;
	}
	else{
		addH = boundry;
	}
	/* 最初の4byteから転送バイト数を読み出す */
	putbus(Command_R,0x22);						/* ページ0，remoteDMA停止，動作ON */
	putbus(RSA_R0,0);						/* スタートアドレス設定 */
	putbus(RSA_R1,addH);
	putbus(RBC_R0,4);						/* 転送バイト数設定 */
	putbus(RBC_R1,0);
	putbus(Command_R,0x0a);						/* ページ0，remoteDMA読出，動作ON */
	status = getbus(DataPort);
	for(i=0; i<2; i++);						/* waiting for rDMA internal process */
	nextP = getbus(DataPort);
	for(i=0; i<2; i++);						/* waiting for rDMA internal process */
	countL = getbus(DataPort);
	for(i=0; i<2; i++);						/* waiting for rDMA internal process */
	countH = getbus(DataPort);
	do{
		status = getbus(IS_R);
	}	while(!(status & 0x40));				/* remoteDMA停止まで待つ */
	putbus(Command_R,0x22);						/* ページ0，remoteDMA停止，動作ON */
	/* 受信パケット情報のチェック */
	nextEP= addH + countH + 1;					/* 次のページ位置を計算 */
	if(nextEP>=PSTOP){
	}
	if(nextP != nextEP){						/* 読み出したページと計算値が合っているか */
		putbus(BNRY_R,addH);
		nextEP= nextEP - PSTOP + PSTART;
		return 0;						/* 不正なパケットはページを進めて終わり */
	}
	/* 取得したバイト数で改めて受信パケットを読み出す */
	putbus(RSA_R0,0);
	putbus(RSA_R1,addH);
	putbus(RBC_R0,countL);
	putbus(RBC_R1,countH);
	count = (unsigned short)countH*256+(unsigned short)countL;
	putbus(Command_R,0x0a);						/* ページ0，remoteDMA読出，動作ON */
	for(i=0; i<512; i++)	Buffer[i]=0;
	for(i=0; i<count; i++){
		if(i<4){
			Buffer[0]= getbus(DataPort);			/* 空読み */
		}
		else if(i>512){
			Buffer[511]= getbus(DataPort);			/* 大きすぎる部分は空読み */
		}else{
			Buffer[i-4]= getbus(DataPort);
		}
	}
	do{
		status = getbus(IS_R);
	}	while(!(status & 0x40));				/* remoteDMA停止まで待つ */
	putbus(Command_R,0x22);						/* ページ0，remoteDMA停止，動作ON */
	boundry= addH + countH;
	if(boundry>=PSTOP){						/* 次のパケット読み出し位置設定 */
		putbus(BNRY_R,boundry - PSTOP + PSTART);
	}
	else{
		putbus(BNRY_R,boundry);
	}
	/* パケットの種別チェック */
		Locate_LCD (12,0);
		Print_LCD ( " " );
	if((unsigned short)(((PacketICMP *)Buffer)->etherType)==(unsigned short)0x0806){				/* ARPの場合 */
		if(CompareIP((((PacketARP *)Buffer)->destIP), myIpAddress)==0){	/* 宛先チェック */
			if(((PacketARP *)Buffer)->operation==0x0001){		/* リクエストの場合 */
				SendArpReply(Buffer);			/* レスポンスの送信 */
				return 0;
			}
		}
		else{							/* その他のARPパケットは無視 */
			return 0;
		}
	}else if((unsigned short)(((PacketICMP *)Buffer)->etherType)==(unsigned short)0x0800){			/* IPパケット受信の時 */
		if(CompareIP((((PacketICMP *)Buffer)->destIP), myIpAddress) == 0){	/* 宛先チェック */
			switch(((PacketICMP *)Buffer)->protocol){			/* プロトコル識別 */
				case 0x01:		/* ICMP */
					Locate_LCD (13,0);
					Print_LCD ( "d" );
					if(((PacketICMP *)Buffer)->type==0x08){	/* 要求が有る時だけ */
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


/* IPアドレスの一致チェック */
unsigned char CompareIP(unsigned char *pIP1, unsigned char *pIP2){

	unsigned char	i;
	
	for(i=0; i<4; i++){
		if(pIP1[i] != pIP2[i]){
			return (1);
		}
	}
	return(0);
}

/* IPアドレスの一致チェック */
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




/* MACアドレスの問い合わせに答える */
void SendArpReply(unsigned char *buff){

	unsigned short	i;
	
	for(i=0; i<6; i++){						/* MACアドレスの設定 */
		((PacketARP *)buff)->destMAC[i]= ((PacketARP *)buff)->srcMAC[i];
		((PacketARP *)buff)->destMACarp[i]= ((PacketARP *)buff)->srcMAC[i];
		((PacketARP *)buff)->srcMAC[i]= myMacAddress[i];
		((PacketARP *)buff)->srcMACarp[i]= myMacAddress[i];
	}
	for(i=0; i<4; i++){						/* IPアドレスの設定 */
		((PacketARP *)buff)->destIP[i]= ((PacketARP *)buff)->srcIP[i];
		((PacketARP *)buff)->srcIP[i]= myIpAddress[i];
	}
	((PacketARP *)buff)->operation= 0x0002;				/* ARP応答 */
	SendEtherData(buff, 60);
}


void SendPingMessage(unsigned char *buff){

	unsigned char	i;
	unsigned short	l;
	unsigned long	sum;
	
	
	((PacketICMP *)buff)->type = 0x00;				/* フィールドタイプ */
	((PacketICMP *)buff)->code = 0x00;				/* コード。要求、応答時は00hとなる */
	((PacketICMP *)buff)->checksum = 0x0000;			/* チェックサム(後で計算) */
	for(i=0; i<6; i++){						/* MACアドレスの設定 */
		((PacketICMP *)buff)->destMAC[i]= ((PacketICMP *)buff)->srcMAC[i];
		((PacketICMP *)buff)->srcMAC[i]= myMacAddress[i];
	}
	for(i=0; i<4; i++){						/* IPアドレスの設定 */
		((PacketICMP *)buff)->destIP[i]= ((PacketICMP *)buff)->srcIP[i];
		((PacketICMP *)buff)->srcIP[i]= myIpAddress[i];
	}
	((PacketICMP *)buff)->identification= TCNT2;
	((PacketICMP *)buff)->checksumIP= 0;
	((PacketICMP *)buff)->checksumIP= GetChecksumIP(buff);		/* IPチェックサムの計算 */


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

/* Ethernetコントローラへパケットを書き込んで送信する */
void SendEtherData(unsigned char *buff, unsigned short count){

	volatile unsigned short	i;
	unsigned char	status, countH, countL;

	countH = (unsigned char)(count >> 8);				/* 転送データ数をバイトに分解 */
	countL = (unsigned char)(count & 0x00ff);
	putbus(Command_R,0x22);						/* ページ0，remoteDMA停止，動作ON */
	do{
		status = getbus(IS_R);
	}	while(!(status & 0x40));				/* remoteDMA停止まで待つ */
	/* リモートDMAの設定 */
	putbus(RSA_R0,0x0);						/* スタートアドレス設定 */
	putbus(RSA_R1,0x40);
	putbus(RBC_R0,countL);						/* 転送バイト数設定 */
	putbus(RBC_R1,countH);
	putbus(Command_R,0x12);						/* ページ0，remoteDMA書込，動作ON */
	for(i= 0; i<(unsigned short)count; i++){
		putbus(DataPort,buff[i]);
	}
	do{
		status = getbus(IS_R);
	}	while(!(status & 0x40));				/* remoteDMA完了まで待つ */
	putbus(Command_R,0x22);						/* ページ0，remoteDMA停止，動作ON */
	putbus(TPS_R,0x40);						/* 送信ページ設定 */
	putbus(TBC_R0,countL);						/* 送信バイト数設定 */
	putbus(TBC_R1,countH);
	putbus(Command_R,0x26);						/* ページ0，remoteDMA停止，パケット送信，動作ON */
}


/* IPヘッダのチェックサム計算 */
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
