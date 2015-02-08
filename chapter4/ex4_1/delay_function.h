void delay_us(unsigned short us)
{
   unsigned short t;
   
   TSTR |= 0x02; /* STR1を1にしてタイマ1をスタートさせる */
   for (t = 0; t < us; t++)
   {
      while ((TISRA & 0x02) == 0x00) /* IMFA1が1になるのを待つ */
      {
         /* TCNT1とGRA1のコンペア・マッチが起きるとIMFA1は1になる */
      }
      TISRA &= 0xFD; /* IMFA1を0に戻す */
      
      /* 以上の処理をus回繰り返す */
   }
   TSTR &= 0xFD; /* STR1を0にしてタイマ1をストップさせる */
}

void delay_ms(unsigned short ms)
{
   unsigned short t;
   
   TSTR |= 0x04; /* STR2を1にしてタイマ2をスタートさせる */
   for (t = 0; t < ms; t++)
   {
      while ((TISRA & 0x04) == 0x00) /* IMFA2が1になるのを待つ */
      {
         /* TCNT2とGRA2のコンペア・マッチが起きるとIMFA2は1になる */
      }
      TISRA &= 0xFB; /* IMFA2を0に戻す */
      
      /* 以上の処理をms回繰り返す */
   }
   TSTR &= 0xFB; /* STR2を0にしてタイマ2をストップさせる */
}
