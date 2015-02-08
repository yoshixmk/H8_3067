#include "H8_register.h" /* H8/3067のレジスタ */
#include "delay_function.h" /* 時間待ちの関数 */

void buzzer(unsigned short us)
{
   PADR = 0xFF; /* PA4を1にする */
   delay_us(us); /* PA4を1にしたまま，us単位で時間待ちする */
   PADR = 0xEF; /* PA4を0にする */
   delay_us(us); /* PA4を0にしたまま，us単位で時間待ちする */
}

void main(void)
{
   P5DDR = 0xF0; /* P50～P53を入力端子として設定する */
   P5PCR = 0xFF; /* P50～P53のプルアップ回路を有効にする */
   
   PADDR = 0xFF; /* PA0～PA7を出力端子として設定する */
   PADR = 0xFF; /* PA0～PA7の初期値を1に設定する */
   
   /* タイマ1の設定 */
   TCR1 |= 0x20; /* CCLR1を0，CCLR0を1に設定する */
   GRA1 = 0x0013; /* 1usの設定 (1/20MHz * (0x0013 + 0x0001)) */
   
   /* タイマ2の設定 */
   TCR2 |= 0x23; /* CCLR1を0，CCLR0を1，TPSC2を0，TPSC1を1, TPSC0を1に設定する */
   GRA2 = 0x09C3; /* 1msの設定 (1/20MHz * 8 * (0x09C3 + 0x0001)) */
   
   while (1) /* 無限ループ */
   {
      if (P5DR == 0xFD) /* ONスイッチだけを押したとき */
      {
         buzzer(500); /* 周期が1000us（= 500us * 2）となる波形パターンを出力する */
      }
      else if (P5DR == 0xFE) /* OFFスイッチだけを押したとき */
      {
         buzzer(1000); /* 周期が2000us（= 1000us * 2）となる波形パターンを出力する */
      }
   }
}
