void H8_register_init(void)
{
   /* RTL8019ASのアドレス・バス */
   P1DDR = 0xFF; /* P10〜P17を出力端子として設定する */
   P1DR = 0xFF; /* P10〜P17の初期値を1に設定する */
   
   /* ディップ・スイッチ */
   P2DDR = 0x00; /* P20〜P27を入力端子として設定する */
   P2PCR = 0xFF; /* P20〜P27のプルアップ回路を有効にする */
   
   /* RTL8019ASのデータ・バス */
   P3DDR = 0xFF; /* P30〜P37を出力端子として設定する */
   P3DR = 0xFF; /* P30〜P37の初期値を1に設定する */
   
   /* マトリクス・キー */
   P4DDR = 0xF0; /* P40〜P43を入力端子，P44〜P47を出力端子として設定する */
   P4PCR = 0x0F; /* P40〜P43のプルアップ回路を有効にする */
   
   /* ONスイッチ，OFFスイッチ */
   P5DDR = 0xF0; /* P50〜P53を入力端子として設定する */
   P5PCR = 0xFF; /* P50〜P53のプルアップ回路を有効にする */
   
   /* RTL8019ASの制御バス */
   P6DDR = 0xFF; /* P60〜P67を出力端子として設定する */
   P6DR = 0xFF; /* P60〜P67の初期値を1に設定する */
   
   /* LED，ブザー，RTL8019ASのRSTDRV */
   PADDR = 0xFF; /* PA0〜PA7を出力端子として設定する */
   PADR = 0xFF; /* PA0〜PA7の初期値を1に設定する */
   
   /* SC1602B */
   PBDDR = 0xFF; /* PB0〜PB7を出力端子として設定する */
   PBDR = 0xFF; /* PB0〜PB7の初期値を1に設定する */
   
   /* A-Dコンバータの設定 */
   ADCSR |= 0x00; /* CH2を0, CH1を0, CH0を0に設定し，AN0をアナログ信号の入力端子に設定する */
   
   /* D-Aコンバータの設定 */
   DACR |= 0x40; /* DAOE0を1に設定し，DA0をアナログ信号の出力端子に設定する */
   
   /* タイマ0の設定 */
   TCR0 |= 0x20; /* CCLR1を0，CCLR0を1に設定する */
   GRA0 = 0x09C3; /* 125usの設定 (1/20MHz * (0x09C3 +0x0001)) */
   
   /* タイマ1の設定 */
   TCR1 |= 0x20; /* CCLR1を0，CCLR0を1に設定する */
   GRA1 = 0x0013; /* 1usの設定 (1/20MHz * (0x0013 + 0x0001)) */
   
   /* タイマ2の設定 */
   TCR2 |= 0x23; /* CCLR1を0，CCLR0を1，TPSC2を0，TPSC1を1, TPSC0を1に設定する */
   GRA2 = 0x09C3; /* 1msの設定 (1/20MHz * 8 * (0x09C3 + 0x0001)) */
}
