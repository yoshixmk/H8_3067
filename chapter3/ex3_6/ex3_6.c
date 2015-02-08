#include "H8_register.h" /* H8/3067のレジスタ */
#include <machine.h> /* set_imask_ccr関数 */

/* sampling_8khz関数は，タイマ割り込みが発生したときに呼び出される関数 */
#pragma interrupt(sampling_8khz)
void sampling_8khz(void)
{
   unsigned char speech;
   
   TISRA &= 0xFE; /* IMFA0を0に戻す */
   
   ADCSR |= 0x20; /* ADSTを1にしてA-D変換をスタートさせる */
   while ((ADCSR & 0x80) == 0x00) /* ADFが1になるのを待つ */
   {
      /* A-D変換が完了するとADFは1になる */
   }
   ADCSR &= 0x7F; /* ADFを0に戻す */
   
   speech = (unsigned char)(ADDRA >> 8); /* ADDRAの上位8bitをspeechに格納する */
   
   DADR0 = speech; /* speechをDA0に出力する */
}

void main(void)
{
   set_imask_ccr(0); /* H8/3067を割り込みが受け付けられる状態にする */
   
   /* A-Dコンバータの設定 */
   ADCSR |= 0x00; /* CH2を0, CH1を0, CH0を0に設定し，AN0をアナログ信号の入力端子に設定する */
   
   /* D-Aコンバータの設定 */
   DACR |= 0x40; /* DAOE0を1に設定し，DA0をアナログ信号の出力端子に設定する */
   
   /* タイマ0の設定 */
   TCR0 |= 0x20; /* CCLR1を0，CCLR0を1に設定する */
   GRA0 = 0x09C3; /* 125usの設定 (1/20MHz * (0x09C3 +0x0001)) */
   TISRA |= 0x10; /* IMIEA0を1に設定し，タイマ割り込みを有効にする */
   
   TSTR |= 0x01; /* STR0を1にしてタイマ0をスタートさせる */
   
   while (1) /* 無限ループ */
   {
      /* 125usごとにタイマ割り込みが発生する */
   }
}
