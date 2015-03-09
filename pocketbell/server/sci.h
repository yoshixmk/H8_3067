/*******************************************************
 *  SCI1制御用ヘッダファイル (H8/3067 20MHz)           *
 *   通信条件　                                        *
 *     データビット  ：8                               *
 *     パリティ      ：なし                            *
 *     ストップビット：1                               *
 *     ボーレート    ：9600bps                         *
 *   PCとのシリアル通信用関数                          *
 *    void sci_open()           <SCI初期化>            *
 *    char sci_getc(void)       <1バイトデータ受信>    *
 *    char *sci_gets(char *str) <文字列受信>           *
 *    void sci_putc(char ch)    <1バイトデータ送信>    *
 *    void sci_puts(char *str)  <文字列送信>           *
 *******************************************************/

/* SCI1 */
typedef volatile struct {
	unsigned char SMR; /* シリアルモード       */
	unsigned char BRR; /* ビットレート         */
	unsigned char SCR; /* シリアルコントロール */
	unsigned char TDR; /* トランスミットデータ */
	unsigned char SSR; /* シリアルステータス   */
	unsigned char RDR; /* レシーブデータ       */
	unsigned char SCMR;/* スマートカードモード */
} H8_3069_SCI1;

/*　SCIアドレス　*/
#define SCI ((H8_3069_SCI1 *)0xFFFB8)

#define SMR_DATA8 0
#define SMR_DATA7 0x40
#define SMR_NOPARITY 0
#define SMR_STOP1 0
#define SMR_STOP2 8

#define SCR_TIE 0x80
#define SCR_RIE 0x40
#define SCR_TE 0x20
#define SCR_RE 0x10
#define SCR_TEIE 0x04

#define SSR_TDRE 0x80
#define SSR_RDRF 0x40
#define SSR_ORER 0x20
#define SSR_FER 0x10
#define SSR_PER 0x08
#define SSR_TEND 0x04

void sci_open(void);
void sci_close(void);
char sci_getc(void);
char *sci_gets(char *);
void sci_putc(char);
void sci_puts(char*);

void wait(long);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +　SCIの初期設定を行う。
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sci_open()
{
	int n;
	SCI->SCR = 0;
	SCI->SMR = 0;

	SCI->SMR = SMR_DATA8 + SMR_NOPARITY + SMR_STOP1;
	SCI->BRR = 64;	/*  9600bps */
/*	SCI->BRR = 32;  /* 19200bps */
/*	SCI->BRR = 15;	/* 38400bps */

	for (n = 0; n < 250; n++) ; /*time wait*/

	SCI->SCR = SCR_TE + SCR_RE;	/* 送受信許可 */
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +　SCIを閉じる。
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sci_close()
{
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +　1バイトのデータを受信する。
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char sci_getc(void)
{
	char ch;
	while ((SCI->SSR & (SSR_ORER + SSR_PER + SSR_FER + SSR_RDRF)) == 0) ;
	if (SCI->SSR & SSR_RDRF) {
		ch = SCI->RDR;
	}
	else {
		ch = -1;
		SCI->SSR = 0;
	}
	return ch;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +　文字列を受信する。
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char *sci_gets(char *str)
{
	char *p, ch;

	p = str;
	for (;;) {
		ch = sci_getc();
		if (ch == '\r') break;
		*p++ = ch;
	}
	*p = '\0';
	return str;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +　1バイトのデータを送信する。
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sci_putc(char ch)
{
	while (!(SCI->SSR & SSR_TDRE)) ;
	SCI->TDR = ch;
	SCI->SSR &= 0x7F; /*TDREを0クリア*/
	while (!(SCI->SSR & SSR_TEND)) ;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +　文字列を送信する。
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sci_puts(char *str)
{
	char	*p;
	for (p = str; *p != '\0'; p++) sci_putc(*p);
}
