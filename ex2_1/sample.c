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
	char cursor_colm=0;
	char cursor_line=0;
	char key;
	PADDR = 0xFF;
	PADR = 0xFF;
	P4DDR = 0xF0;
	P4PCR = 0x0F;
	PBDDR = 0xF3;

	TCR2 = 0xA3;
	GRA2 = 2499;
	TISRA |= 0x40;
	PADR = 0xFB;
	TSTR |= 0x04;

	LCD_init();

	while(1)
	{
		while(cursor_colm<16){
			if((cursor_colm==0)&&(cursor_line==0)){
				LCD_control(0x01);
				LCD_control(0x00);
			}
			if((cursor_colm==0)&&(cursor_line==1)){
				LCD_control(0xC0);
			}
			key=matrix_key_read();
			if(key>='0' && key<='9'){
				LCD_display(key);
				cursor_colm++;
				ms_timer(300);
			}
			if(key=='#'){
				LCD_control(0x01);
				LCD_control(0x00);
				ms_timer(300);
			}
		}
		cursor_colm=0;
		cursor_line^=1;
	}
}
