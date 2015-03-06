void LCD_write(char data, char RS)
{
   /* SC1602Bの端子: DB7, DB6, DB5, DB4,   x,   x,  RS,   E */
   /* H8/3067の端子: PB7, PB6, PB5, PB4, PB3, PB2, PB1, PB0 */
   
   PBDR = data & 0xF0; /* DB4～DB7にdataの上位4bit，RSとEに0をセットする */
   
   if (RS == 1) /* 文字コードの場合 */
   {
      PBDR |= 0x02; /* RSに1をセットする */
   }
   else /* 制御コードの場合 */
   {
      PBDR &= 0xF0; /* RSに0をセットする */
   }
   
   delay_us(1); /* 1us（40ns以上）の時間待ち */
   PBDR |= 0x01; /* Eに1をセットする */
   delay_us(1); /* 1us（230ns以上）の時間待ち */
   PBDR &= 0xFE; /* Eに0をセットする */
}

void LCD_display(char code)
{
   LCD_write(code, 1); /* 文字コードの上位4bitを転送する */
   LCD_write(code << 4, 1); /* 文字コードの下位4bitを転送する */
   delay_us(50); /* 50us（40us以上）の時間待ち */
}

void LCD_control(char code)
{
   LCD_write(code, 0); /* 制御コードの上位4bitを転送する */
   LCD_write(code << 4, 0); /* 制御コードの下位4bitを転送する */
   delay_us(50); /* 50us（40us以上）の時間待ち */
}

void LCD_clear(void)
{
   LCD_write(0x01, 0); /* 0x01の上位4bitを転送する */
   LCD_write(0x01 << 4, 0); /* 0x01の下位4bitを転送する */
   delay_ms(2); /* 2ms（1.64ms以上）の時間待ち */
}

void LCD_init(void)
{
   delay_ms(20); /* 20ms（15ms以上）の時間待ち */
   LCD_write(0x30, 0); /* 8bitモード設定 */
   delay_ms(5); /* 5ms（4.1ms以上）の時間待ち */
   LCD_write(0x30, 0); /* 8bitモード設定 */
   delay_ms(5); /* 5ms（100us以上）の時間待ち */
   LCD_write(0x30, 0); /* 8bitモード設定 */
   delay_ms(5); /* 5ms（4.1ms以上）の時間待ち */
   LCD_write(0x20, 0); /* 4bitモード設定 */
   delay_ms(5); /* 5ms（40us以上）の時間待ち */
   LCD_control(0x28); /* 4bitモードで2行表示に設定する */
   LCD_control(0x08); /* 文字表示をオフにする */
   LCD_control(0x0C); /* 文字表示をオンにする */
   LCD_control(0x06); /* カーソル移動を右方向に設定する */
}

void LCD_print(char *str)
{
   while (*str != 0x00) /* ナル文字（0x00）が現れるまで */
   {
      LCD_display(*str); /* 液晶ディスプレイに文字を表示する */
      str++;
   }
}
