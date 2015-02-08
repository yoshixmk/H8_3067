#include "H8_register.h" /* H8/3067のレジスタ */
#include "delay_function.h" /* 時間待ちの関数 */
#include "LCD_function.h" /* SC1602Bの関数 */
#include "matrix_key_function.h" /* マトリクス・キーの関数 */

void main(void)
{
   char key, previous_key;
   unsigned char cursor, line;
   
   P4DDR = 0xF0; /* P40～P43を入力端子，P44～P47を出力端子として設定する */
   P4PCR = 0x0F; /* P40～P43のプルアップ回路を有効にする */
   
   PBDDR = 0xFF; /* PB0～PB7を出力端子として設定する */
   PBDR = 0xFF; /* PB0～PB7の初期値を1に設定する */
   
   /* タイマ1の設定 */
   TCR1 |= 0x20; /* CCLR1を0，CCLR0を1に設定する */
   GRA1 = 0x0013; /* 1usの設定 (1/20MHz * (0x0013 + 0x0001)) */
   
   /* タイマ2の設定 */
   TCR2 |= 0x23; /* CCLR1を0，CCLR0を1，TPSC2を0，TPSC1を1, TPSC0を1に設定する */
   GRA2 = 0x09C3; /* 1msの設定 (1/20MHz * 8 * (0x09C3 + 0x0001)) */
   
   LCD_init(); /* SC1602Bの初期化 */
   
   LCD_clear(); /* 表示クリア */
   LCD_control(0x80); /* カーソルを0行目の先頭に移動する */
   
   cursor = 0; /* cursorの初期値 */
   line = 0; /* lineの初期値 */
   previous_key = 0; /* previous_keyの初期値 */
   
   while (1) /* 無限ループ */
   {
      while (cursor < 16) /* cursorが16よりも小さいとき */
      {
         delay_ms(20);
         /* 20msの時間待ちをはさむことで，チャタリングによるスイッチの誤検出を回避する */
         
         key = matrix_key_read(); /* 現在のキーをチェックする */
         
         if ((key != 0) && (key != previous_key))
         /* 現在のキーが0ではなく，直前のキーとは異なるとき */
         {
            if ((cursor == 0) && (line == 0)) /* cursorが0，lineが0のとき */
            {
               LCD_clear(); /* 表示クリア */
               LCD_control(0x80); /* カーソルを0行目の先頭に移動する */
            }
            else if ((cursor == 0) && (line == 1)) /* cursorが0，lineが1のとき */
            {
               LCD_control(0xC0); /* カーソルを1行目の先頭に移動する */
            }
            LCD_display(key); /* 液晶ディスプレイに現在のキーを表示する */
            cursor++; /* cursorをインクリメントする */
         }
         previous_key = key;
         /* 現在のキーを，次のチェック時刻における直前のキーとして定義する */
      }
      cursor = 0; /* cursorを0に戻す */
      line ^= 1; /* XOR演算によって，lineが0のときは1，lineが1のときは0にする */
   }
}
