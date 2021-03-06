#include <sci.h>
#include <reg3067.h>
#include <machine.h>
#include <RTL8019AS_register.h>

unsigned char src_MAC[6];

void ms_timer(unsigned short ms)
{
	int j;

	TSTR |= 0x04;
	for(j=0;j<ms;j++){
		while((TISRA & 0x04) != 0x04);
		TISRA &= 0xFB;
	}
	TSTR &= 0xFB;
}

void us_timer(unsigned short us)
{
	int j;

	TSTR |= 0x02;
	for(j=0;j<us;j++){
		while((TISRA & 0x02) != 0x02);
		TISRA &= 0xFD;
	}
	TSTR &= 0xFD;
}

char matrix_key_read(void)
{
	char key;
	unsigned char column, row;

	key = 0;
	column = 0x10;

	while(column <= 0x40)
	{
		P4DR = ~column;

		row =(~P4DR) & 0x0f;

		if(row != 0)
		{
			switch(column | row)
			{
				case 0x11 : key ='1'; break;
				case 0x12 : key ='2'; break;
				case 0x14 : key ='3'; break;
				case 0x18 : key ='4'; break;

				case 0x21 : key ='5'; break;
				case 0x22 : key ='6'; break;
				case 0x24 : key ='7'; break;
				case 0x28 : key ='8'; break;

				case 0x41 : key ='9'; break;
				case 0x42 : key ='.'; break;
				case 0x44 : key ='0'; break;
				case 0x48 : key ='#'; break;

				default : key = 0; break;
			}
		}
		column = column << 1;
	}
	return key;
}



void LCD_write(char data, char RS)
{
	PBDR = data & 0xF0;
	if(RS == 1)
	{
		PBDR |= 0x02;/*RSに1をセットする*/
	}
	else
	{
		PBDR &= 0xFD;/*RSに0をセットする*/
	}

	ms_timer(1);/*40ns以上の時間待ち*/
	PBDR |= 0x01;/*Eに1をセットする*/
	ms_timer(1);/*230ns以上待ち*/
	PBDR &= 0xFE;/*Eに0をセットする*/
}

void LCD_display(char code)
{
	LCD_write(code,1);
	LCD_write(code << 4,1);
	ms_timer(1);/*40us以上待ち*/
}

void LCD_control(char code)
{
	LCD_write(code, 0);
	LCD_write(code << 4, 0);
	ms_timer(1);/*40us以上待ち*/
}

void LCD_init(void)
{
	ms_timer(15);
	LCD_write(0x30,0);
	ms_timer(1);
	LCD_write(0x30,0);
	ms_timer(1);
	LCD_write(0x30,0);
	ms_timer(5);
	LCD_write(0x20,0);
	ms_timer(1);
	LCD_control(0x28);
	LCD_control(0x08);
	LCD_control(0x0C);
	LCD_control(0x06);
	LCD_control(0x01);
}
unsigned char NIC_read(address)
{
	unsigned char data;
	P1DR = address;
	P3DDR = 0x00;
	P6DR &= 0xEF;
	data = P3DR;
	P6DR |= 0x10;
	return data;
}
void NIC_write(unsigned char address, unsigned char data)
{
	P1DR = address;
	P3DDR = 0xFF;
	P6DR &= 0xDF;
	P3DR = data;
	P6DR |= 0x20;
}
void NIC_init(void)
{
	unsigned short i;
	unsigned char data;
	PADR |= 0x80;
	ms_timer(10);
	PADR &= 0x7F;
	ms_timer (10);
	data = NIC_read(RP);
	NIC_write(RP, data);
	ms_timer (10);
	NIC_write(CR, 0x21);
	NIC_write(DCR, 0x4A);
	NIC_write(RBCR0, 0);
	NIC_write(RBCR1, 0);
	NIC_write(RCR, 0x20);
	NIC_write(TCR, 0x02);
	NIC_write(TPSR, 0x40);
	NIC_write(PSTART, 0x46);
	NIC_write(BNRY, 0x46);
	NIC_write(PSTOP, 0x60);
	NIC_write(IMR, 0x00);
	NIC_write(ISR, 0xFF);
	NIC_write(RBCR0, 12);
	NIC_write(RBCR1, 0);
	NIC_write(RSAR0, 0x00);
	NIC_write(RSAR1, 0x00);
	NIC_write(CR, 0x0A);
	for (i = 0; i < 6; i += 2) {
		src_MAC[i + 1] = NIC_read(RDMAP);
		NIC_read(RDMAP);
		src_MAC[i] = NIC_read(RDMAP);
		NIC_read(RDMAP);
	}
	do {
		data = NIC_read(ISR);
	} while ((data & 0x40) == 0x00);
	NIC_write(CR, 0x61);
	NIC_write(PAR0, src_MAC[0]);
	NIC_write(PAR1, src_MAC[1]);
	NIC_write(PAR2, src_MAC[2]);
	NIC_write(PAR3, src_MAC[3]);
	NIC_write(PAR4, src_MAC[4]);
	NIC_write(PAR5, src_MAC[5]);
	NIC_write(CURR, 0x47);
	NIC_write(MAR0, 0);
	NIC_write(MAR1, 0);
	NIC_write(MAR2, 0);
	NIC_write(MAR3, 0);
	NIC_write(MAR4, 0);
	NIC_write(MAR5, 0);
	NIC_write(MAR6, 0);
	NIC_write(MAR7, 0);
	NIC_write(CR, 0x21);
	NIC_write(RCR, 0x04);
	NIC_write(CR, 0x22);
	NIC_write(TCR, 0x00);
	NIC_write(IMR, 0x00);
}

void MAC_to_str(unsigned char *MAC, char *str)
{
	unsigned short i, n;
	unsigned char nibble;
	n = 0;
	for(i = 0; i < 6; i++)
	{
		nibble = (MAC[i] & 0xF0) >> 4;
		if (nibble < 10)
		{
			str[n] = '0' + nibble;
		}
		else
		{
			str[n] = 'A' + nibble - 10;
		}
		n++;
		nibble = MAC[i] & 0x0F;
		if (nibble < 10)
		{
			str[n] = '0' + nibble;
		}
		else
		{
			str[n] = 'A' + nibble - 10;
		}
		n++;
	}
	str[n] = 0x00;
}

void LCD_print(char *str)
{
	for( ; *str!= '\0'; str++)
	{
		LCD_display(*str);
	}
}
void buzzer(void) /*using while()*/
{
	char key;
	key=matrix_key_read();
	PADR |= 0x10;
	switch(key){
		case '1' :
			us_timer(956);
			PADR &= 0xEF;
			us_timer(956); break;
		case '2' :
			us_timer(852);
			PADR &= 0xEF;
			us_timer(852); break;
		case '3' :
			us_timer(759);
			PADR &= 0xEF;
			us_timer(759); break;
		case '4' :
			us_timer(716);
			PADR &= 0xEF;
			us_timer(716); break;
		case '5' :
			us_timer(638);
			PADR &= 0xEF;
			us_timer(638); break;
		case '6' :
			us_timer(568);
			PADR &= 0xEF;
			us_timer(568); break;
		case '7' :
			us_timer(506);
			PADR &= 0xEF;
			us_timer(506); break;
		case '8' :
			us_timer(478);
			PADR &= 0xEF;
			us_timer(478); break;
		case '9' :
			us_timer(423);
			PADR &= 0xEF;
			us_timer(423); break;
		case '0' :
			us_timer(379);
			PADR &= 0xEF;
			us_timer(379); break;
		default : key = 0; break;
	}
}
void main(void)
{
	char str[13];
	P1DDR |= 0x1F;
	P6DDR |= 0x30;
	PADDR |= 0x80;
	P4DDR = 0xF0;
	P4PCR = 0x0F;
	PBDDR = 0xF3;

	TCR1 = 0xA0;
	TCR2 = 0xA3;
	GRA1 = 0x13;
	GRA2 = 2499;
	TISRA |= 0x40;
	PADR = 0xFB;
	TSTR |= 0x04;

	NIC_init();
	LCD_init();
	MAC_to_str(src_MAC, str);
	LCD_print(str);

	while(1);
}
