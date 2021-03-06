/* I/Oポートのレジスタ */
#define P5DDR  (*(volatile unsigned char  *)0xEE004)
#define PADDR  (*(volatile unsigned char  *)0xEE009)
#define P5DR   (*(volatile unsigned char  *)0xFFFD4)
#define PADR   (*(volatile unsigned char  *)0xFFFD9)
#define P5PCR  (*(volatile unsigned char  *)0xEE03F)

void main(void)
{
   P5DDR = 0xF0; /* P50〜P53を入力端子として設定する */
   P5PCR = 0xFF; /* P50〜P53のプルアップ回路を有効にする */
   
   PADDR = 0xFF; /* PA0〜PA7を出力端子として設定する */
   PADR = 0xFF; /* PA0〜PA7の初期値を1に設定する */
   
   while (1) /* 無限ループ */
   {
      if (P5DR == 0xFD) /* ONスイッチだけを押したとき */
      {
         PADR = 0xFB; /* LED(D8)だけが発光する */
      }
      else if (P5DR == 0xFE) /* OFFスイッチだけを押したとき */
      {
         PADR = 0xF7; /* LED(D9)だけが発光する */
      }
      else /* それ以外のとき */
      {
         PADR = 0xFF; /* どちらのLEDも発光しない */
      }
   }
}
