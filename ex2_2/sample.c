#include <sci.h>
#include <reg3067.h>
#include <machine.h>

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
}

void main(void)
{
	char key;
	PADDR = 0xFF;
	P4DDR = 0xF0;
	P4PCR = 0x0F;
	PBDDR = 0xF3;

	TCR1 = 0xA0;
	TCR2 = 0xA3;
	GRA1 = 0x13;

	LCD_init();

	while(1){
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
}
