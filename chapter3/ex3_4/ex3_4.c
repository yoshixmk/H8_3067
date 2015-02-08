#include "H8_register.h" /* H8/3067のレジスタ */
#include "delay_function.h" /* 時間待ちの関数 */
#include "LCD_function.h" /* SC1602Bの関数 */

void main(void)
{
   char str[] = "Hello, world!"; /* 液晶ディスプレイに表示する文字列 */
   
   PBDDR = 0xFF; /* PB0～PB7を出力端子として設定する */
   PBDR = 0xFF; /* PB0～PB7の初期値を1に設定する */
   
   /* タイマ1の設定 */
   TCR1 |= 0x20; /* CCLR1を0，CCLR0を1に設定する */
   GRA1 = 0x0013; /* 1usの設定 (1/20MHz * (0x0013 + 0x0001)) */
   
   /* タイマ2の設定 */
   TCR2 |= 0x23; /* CCLR1を0，CCLR0を1，TPSC2を0，TPSC1を1, TPSC0を1に設定する */
   GRA2 = 0x09C3; /* 1msの設定 (1/20MHz * 8 * (0x09C3 + 0x0001)) */
   
   LCD_init(); /* SC1602Bの初期化 */
   
   while (1) /* 無限ループ */
   {
      LCD_clear(); /* 表示クリア */
      LCD_control(0x80); /* カーソルを0行目の先頭に移動する */
      LCD_print(str); /* 液晶ディスプレイに文字列を表示する */
      delay_ms(1000); /* 1000ms（1秒）だけ時間待ちする */
      
      LCD_clear(); /* 表示クリア */
      LCD_control(0xC0); /* カーソルを1行目の先頭に移動する */
      LCD_print(str); /* 液晶ディスプレイに文字列を表示する */
      delay_ms(1000); /* 1000ms（1秒）だけ時間待ちする */
   }
}
