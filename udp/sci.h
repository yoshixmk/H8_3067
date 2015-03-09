/*******************************************************
 *  SCI1����p�w�b�_�t�@�C�� (H8/3067 20MHz)           *
 *   �ʐM�����@                                        *
 *     �f�[�^�r�b�g  �F8                               *
 *     �p���e�B      �F�Ȃ�                            *
 *     �X�g�b�v�r�b�g�F1                               *
 *     �{�[���[�g    �F9600bps                         *
 *   PC�Ƃ̃V���A���ʐM�p�֐�                          *
 *    void sci_open()           <SCI������>            *
 *    char sci_getc(void)       <1�o�C�g�f�[�^��M>    *
 *    char *sci_gets(char *str) <�������M>           *
 *    void sci_putc(char ch)    <1�o�C�g�f�[�^���M>    *
 *    void sci_puts(char *str)  <�����񑗐M>           *
 *******************************************************/

/* SCI1 */
typedef volatile struct {
	unsigned char SMR; /* �V���A�����[�h       */
	unsigned char BRR; /* �r�b�g���[�g         */
	unsigned char SCR; /* �V���A���R���g���[�� */
	unsigned char TDR; /* �g�����X�~�b�g�f�[�^ */
	unsigned char SSR; /* �V���A���X�e�[�^�X   */
	unsigned char RDR; /* ���V�[�u�f�[�^       */
	unsigned char SCMR;/* �X�}�[�g�J�[�h���[�h */
} H8_3069_SCI1;

/*�@SCI�A�h���X�@*/
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
 +�@SCI�̏����ݒ���s���B
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

	SCI->SCR = SCR_TE + SCR_RE;	/* ����M���� */
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +�@SCI�����B
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sci_close()
{
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +�@1�o�C�g�̃f�[�^����M����B
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
 +�@���������M����B
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
 +�@1�o�C�g�̃f�[�^�𑗐M����B
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sci_putc(char ch)
{
	while (!(SCI->SSR & SSR_TDRE)) ;
	SCI->TDR = ch;
	SCI->SSR &= 0x7F; /*TDRE��0�N���A*/
	while (!(SCI->SSR & SSR_TEND)) ;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +�@������𑗐M����B
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void sci_puts(char *str)
{
	char	*p;
	for (p = str; *p != '\0'; p++) sci_putc(*p);
}
