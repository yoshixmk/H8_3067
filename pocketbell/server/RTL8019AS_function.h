unsigned char NIC_read(unsigned char address)
{
   unsigned char data;
   
   P1DR = address; /* ポート1にレジスタのアドレスをセットする */
   P3DDR = 0x00; /* ポート3を入力ポートとして設定する */
   P6DR &= 0xEF; /* P64に0をセットして，データの読み取りを許可する */
   data = P3DR; /* レジスタのデータを読み取る */
   P6DR |= 0x10; /* P64に1をセットして，データの読み取りを禁止する */
   return data; /* dataを戻り値として，呼び出し元の関数に戻る */
}

void NIC_write(unsigned char address, unsigned char data)
{
   P1DR = address; /* ポート1にレジスタのアドレスをセットする */
   P3DDR = 0xFF; /* ポート3を出力ポートとして設定する */
   P6DR &= 0xDF; /* P65に0をセットして，データの書き込みを許可する */
   P3DR = data; /* レジスタにデータを書き込む */
   P6DR |= 0x20; /* P65に1をセットして，データの書き込みを禁止する */
}

void NIC_init(void)
{
   unsigned short i;
   unsigned char data;
   
   /* ハードウェア・リセット */
   PADR |= 0x80; /* RSTDRVに1を出力 */
   ms_timer(10); /* 10msの時間待ち */
   PADR &= 0x7F; /* RSTDRVに0を出力 */
   ms_timer(10); /* 10msの時間待ち */
   
   /* ソフトウェア・リセット */
   data = NIC_read(RP); /* Reset Portのデータを読み取る */
   NIC_write(RP, data); /* Reset Portにデータを書き込む */
   ms_timer(10); /* 10msの時間待ち */
   
   NIC_write(CR, 0x21); /* レジスタ・ページ0，NICをストップさせる */
   NIC_write(DCR, 0x4A); /* データ形式の設定 */
   NIC_write(RBCR0, 0); /* リモートDMAにおけるデータ・サイズ（下位バイト）のクリア */
   NIC_write(RBCR1, 0); /* リモートDMAにおけるデータ・サイズ（上位バイト）のクリア */
   NIC_write(RCR, 0x20); /* モニタ・モード（受信） */
   NIC_write(TCR, 0x02); /* ループバック・モード（送信） */
   NIC_write(TPSR, 0x40); /* 送信バッファの開始ページ */
   NIC_write(PSTART, 0x46); /* 受信バッファの開始ページ */
   NIC_write(BNRY, 0x46); /* 受信バッファの境界ページ */
   NIC_write(PSTOP, 0x60); /* 受信バッファの終了ページ */
   NIC_write(IMR, 0x00); /* 割り込み禁止 */
   NIC_write(ISR, 0xFF); /* 割り込みフラグのクリア */
   
   NIC_write(RBCR0, 12); /* MACアドレスのサイズ（下位バイト） */
   NIC_write(RBCR1, 0); /* MACアドレスのサイズ（上位バイト） */
   NIC_write(RSAR0, 0x00); /* MACアドレスの先頭アドレス（下位バイト） */
   NIC_write(RSAR1, 0x00); /* MACアドレスの先頭アドレス（上位バイト） */
   NIC_write(CR, 0x0A); /* NICをスタートさせ，リモートDMA読み取りを許可する */
   for (i = 0; i < 6; i += 2)
   {
      src_MAC[i + 1] = NIC_read(RDMAP); /* MACアドレスを読み取る */
      NIC_read(RDMAP); /* ダミー・データを読み取る */
      src_MAC[i] = NIC_read(RDMAP); /* MACアドレスを読み取る */
      NIC_read(RDMAP); /* ダミー・データを読み取る */
   }
   do
   {
      data = NIC_read(ISR); /* ISRを読み取る */
   } while ((data & 0x40) == 0x00); /* リモートDMAが停止するのを待つ */
   
   NIC_write(CR, 0x61); /* レジスタ・ページ1，NICをストップさせる */
   NIC_write(PAR0, src_MAC[0]); /* MACアドレスの設定 */
   NIC_write(PAR1, src_MAC[1]); /* MACアドレスの設定 */
   NIC_write(PAR2, src_MAC[2]); /* MACアドレスの設定 */
   NIC_write(PAR3, src_MAC[3]); /* MACアドレスの設定 */
   NIC_write(PAR4, src_MAC[4]); /* MACアドレスの設定 */
   NIC_write(PAR5, src_MAC[5]); /* MACアドレスの設定 */
   
   NIC_write(CURR, 0x47); /* 受信パケットの開始ページの初期値 */
   
   NIC_write(MAR0, 0); /* マルチキャスト・アドレスの設定 */
   NIC_write(MAR1, 0); /* マルチキャスト・アドレスの設定 */
   NIC_write(MAR2, 0); /* マルチキャスト・アドレスの設定 */
   NIC_write(MAR3, 0); /* マルチキャスト・アドレスの設定 */
   NIC_write(MAR4, 0); /* マルチキャスト・アドレスの設定 */
   NIC_write(MAR5, 0); /* マルチキャスト・アドレスの設定 */
   NIC_write(MAR6, 0); /* マルチキャスト・アドレスの設定 */
   NIC_write(MAR7, 0); /* マルチキャスト・アドレスの設定 */
   
   NIC_write(CR, 0x21); /* レジスタ・ページ0，NICをストップさせる */
   NIC_write(RCR, 0x04); /* ブロードキャスト・パケットに対応する */
   NIC_write(CR, 0x22); /* レジスタ・ページ0，NICをスタートさせる */
   NIC_write(TCR, 0x00); /* 通常送信モード */
   NIC_write(IMR, 0x00); /* 割り込み禁止 */
}

void packet_send(unsigned char *packet, unsigned short size)
{
   unsigned short i;
   unsigned char data;
   unsigned char size_H, size_L;
   
   size_L = (unsigned char)(size & 0x00FF); /* 送信パケットのサイズ（下位バイト） */
   size_H = (unsigned char)(size >> 8); /* 送信パケットのサイズ（上位バイト） */
   
   NIC_write(CR, 0x22); /* レジスタ・ページ0 */
   NIC_write(RBCR0, size_L); /* 送信パケットのサイズ（下位バイト） */
   NIC_write(RBCR1, size_H); /* 送信パケットのサイズ（上位バイト） */
   NIC_write(RSAR0, 0x00); /* 送信パケットの先頭アドレス（下位バイト） */
   NIC_write(RSAR1, 0x40); /* 送信パケットの先頭アドレス（上位バイト） */
   NIC_write(CR, 0x12); /* リモートDMA書き込みを許可する */
   for (i = 0; i < size; i++)
   {
      NIC_write(RDMAP, packet[i]); /* 送信バッファにパケットのデータを書き込む */
   }
   do
   {
      data = NIC_read(ISR); /* ISRを読み取る */
   } while ((data & 0x40) == 0x00); /* リモートDMAが停止するのを待つ */
   
   NIC_write(CR, 0x22); /* レジスタ・ページ0 */
   NIC_write(TBCR0, size_L); /* 送信パケットのサイズ（下位バイト） */
   NIC_write(TBCR1, size_H); /* 送信パケットのサイズ（上位バイト） */
   NIC_write(TPSR, 0x40); /* 送信バッファの開始ページ */
   NIC_write(CR, 0x26); /* パケットを送信する */
   do
   {
      data = NIC_read(CR); /* CRを読み取る */
   } while ((data & 0x04) == 0x04); /* パケットの送信が完了するのを待つ */
}

unsigned char packet_receive(unsigned char *packet)
{
   unsigned short i;
   unsigned short size;
   unsigned char data;
   unsigned char size_H, size_L;
   unsigned char boundary_page, start_page, current_page;
   unsigned char header[4];
   
   NIC_write(CR, 0x22); /* レジスタ・ページ0 */
   boundary_page = NIC_read(BNRY); /* BNRYを読み取る */
   NIC_write(CR, 0x62); /* レジスタ・ページ1 */
   current_page = NIC_read(CURR); /* CURRを読み取る */
   
   if (current_page < boundary_page)
   {
      current_page += (0x60 - 0x46); /* 受信バッファがリング・バッファであることを考慮する */
   }
   if (current_page == boundary_page + 1) /* 受信パケットの判定 */
   {
      return 1; /* 受信パケットなし */
   }
   
   start_page = boundary_page + 1; /* 受信パケットの開始ページ */
   if (start_page == 0x60)
   {
      start_page = 0x46; /* 受信バッファがリング・バッファであることを考慮する */
   }
   
   NIC_write(CR, 0x22); /* レジスタ・ページ0 */
   NIC_write(RBCR0, 4); /* フレーム管理ヘッダのサイズ（下位バイト） */
   NIC_write(RBCR1, 0); /* フレーム管理ヘッダのサイズ（上位バイト） */
   NIC_write(RSAR0, 0x00); /* フレーム管理ヘッダの先頭アドレス（下位バイト） */
   NIC_write(RSAR1, start_page); /* フレーム管理ヘッダの先頭アドレス（上位バイト） */
   NIC_write(CR, 0x0A); /* リモートDMA読み取りを許可する */
   for (i = 0; i < 4; i++)
   {
      header[i] = NIC_read(RDMAP); /* フレーム管理ヘッダを読み取る */
   }
   do
   {
      data = NIC_read(ISR); /* ISRを読み取る */
   } while ((data & 0x40) == 0x00); /* リモートDMAが停止するのを待つ */
   
   NIC_write(CR, 0x22); /* レジスタ・ページ0 */
   size_L = header[2]; /* 受信パケットのサイズ（下位バイト） */
   size_H = header[3]; /* 受信パケットのサイズ（上位バイト） */
   size = ((unsigned short)size_H << 8) + (unsigned short)size_L; /* 受信パケットのサイズ */
   NIC_write(RBCR0, size_L); /* 受信パケットのサイズ（下位バイト） */
   NIC_write(RBCR1, size_H); /* 受信パケットのサイズ（上位バイト） */
   NIC_write(RSAR0, 0x00); /* 受信パケットの先頭アドレス（下位バイト） */
   NIC_write(RSAR1, start_page); /* 受信パケットの先頭アドレス（上位バイト） */
   NIC_write(CR, 0x0A); /* リモートDMA読み取りを許可する */
   for (i = 0; i < 4; i++)
   {
      NIC_read(RDMAP); /* ダミー・データを読み取る */
   }
   for (i = 0; i < (size - 4); i++)
   {
      packet[i] = NIC_read(RDMAP); /* 受信バッファからパケットのデータを読み取る */
      
      if (i >= 256) /* パケットのサイズが256byteよりも大きいとき */
      {
         NIC_read(RDMAP); /* ダミー・データを読み取る */
      }
   }
   do
   {
      data = NIC_read(ISR); /* ISRを読み取る */
   } while ((data & 0x40) == 0x00); /* リモートDMAが停止するのを待つ */
   
   NIC_write(CR, 0x22); /* レジスタ・ページ0 */
   boundary_page = current_page - 1;
   if (boundary_page >= 0x60)
   {
      boundary_page -= (0x60 - 0x46); /* 受信バッファがリング・バッファであることを考慮する */
   }
   NIC_write(BNRY, boundary_page); /* BNRYを更新する */
   
   return 0;
}
