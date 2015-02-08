void text_packet_send(unsigned char *packet)
{
   unsigned short i;
   unsigned long sum;
   TEXT_PACKET *text_packet;
   unsigned char header[12];
   PSEUDO_HEADER *pseudo_header;
   
   text_packet = (TEXT_PACKET *)packet; /* packetをTEXT_PACKET構造体に当てはめる */
   
   for (i = 0; i < 6; i++)
   {
      text_packet -> eth_dst_MAC[i] = dst_MAC[i]; /* 宛先MACアドレス */
   }
   for (i = 0; i < 6; i++)
   {
      text_packet -> eth_src_MAC[i] = src_MAC[i]; /* 送信元MACアドレス */
   }
   text_packet -> eth_ethernet_type = 0x0800; /* 上位プロトコルの種類（IP） */
   text_packet -> ip_version_length = 0x45; /* IPのバージョンとIPヘッダの長さ */
   text_packet -> ip_service_type = 0; /* パケットの優先度 */
   text_packet -> ip_total_length = 20 + 8 + 16; /* IPデータグラムの長さ */
   text_packet -> ip_id = src_ip_id; /* パケットの識別子 */
   text_packet -> ip_flags_fragment_offset = 0x4000; /* パケットの分割の有無と本来のデータ位置 */
   text_packet -> ip_time_to_live = 128; /* ルータの中継回数の上限 */
   text_packet -> ip_protocol = 17; /* 上位プロトコルの種類（UDP） */
   text_packet -> ip_checksum = 0x0000; /* チェックサムの初期値 */
   for (i = 0; i < 4; i++)
   {
      text_packet -> ip_src_IP[i] = src_IP[i]; /* 送信元IPアドレス */
   }
   for (i = 0; i < 4; i++)
   {
      text_packet -> ip_dst_IP[i] = dst_IP[i]; /* 宛先IPアドレス */
   }
   text_packet -> udp_src_port = src_port; /* 送信元ポート番号 */
   text_packet -> udp_dst_port = dst_port; /* 宛先ポート番号 */
   text_packet -> udp_length = 8 + 16; /* UDPセグメントの長さ */
   text_packet -> udp_checksum = 0x0000; /* チェックサムの初期値 */
   for (i = 0; i < 16; i++)
   {
      text_packet -> text_data[i] = text_buffer[i];
      /* text_bufferに格納されたテキスト・データをパケットに搭載する */
   }
   
   /* 送信側におけるIPヘッダのチェックサムの計算 */
   sum = ones_complement_sum(packet, 14, 20); /* 1の補数和（IPヘッダ） */
   sum = (~sum) & 0xFFFF; /* 計算結果をNOT演算によって反転する */
   text_packet -> ip_checksum = (unsigned short)sum; /* 計算結果をセットする */
   
   /* 擬似ヘッダ */
   pseudo_header = (PSEUDO_HEADER *)header; /* headerをPSEUDO_HEADER構造体に当てはめる */
   for (i = 0; i < 4; i++)
   {
      pseudo_header -> ip_src_IP[i] = text_packet -> ip_src_IP[i]; /* 送信元IPアドレス */
      pseudo_header -> ip_dst_IP[i] = text_packet -> ip_dst_IP[i]; /* 宛先IPアドレス */
   }
   pseudo_header -> zero_padding = 0; /* ゼロ・パディング */
   pseudo_header -> ip_protocol = text_packet -> ip_protocol; /* 上位プロトコルの種類（UDP） */
   pseudo_header -> udp_length = text_packet -> udp_length; /* UDPセグメントの長さ */
   
   /* 送信側におけるUDPヘッダのチェックサムの計算 */
   sum = ones_complement_sum(header, 0, 12); /* 1の補数和（擬似ヘッダ） */
   sum += ones_complement_sum(packet, 34, (8 + 16)); /* 1の補数和（UDPセグメント） */
   sum = (sum & 0xFFFF) + (sum >> 16); /* 1の補数和 */
   sum = (~sum) & 0xFFFF; /* 計算結果をNOT演算によって反転する */
   if (sum == 0x0000)
   {
      sum = 0xFFFF; /* 計算結果が0x0000となってしまった場合は0xFFFFにする */
   }
   text_packet -> udp_checksum = (unsigned short)sum; /* 計算結果をセットする */
   
   packet_send(packet, 60); /* パケットのサイズを60byteに指定して送信する */
}

unsigned char text_packet_error_check(unsigned char *packet)
{
   unsigned short i;
   unsigned long sum;
   TEXT_PACKET *text_packet;
   unsigned char header[12];
   PSEUDO_HEADER *pseudo_header;
   
   text_packet = (TEXT_PACKET *)packet; /* packetをTEXT_PACKET構造体に当てはめる */
   
   /* 受信側におけるIPヘッダのチェックサムの計算 */
   sum = ones_complement_sum(packet, 14, 20); /* 1の補数和（IPヘッダ） */
   sum = (~sum) & 0xFFFF; /* 計算結果をNOT演算によって反転する */
   if (sum != 0x0000)
   {
      return 1; /* チェックサムが0x0000にならない場合はエラーが生じたとみなされる */
   }
   
   if (text_packet -> udp_checksum != 0x0000) /* UDPヘッダのチェックサムが0x0000ではないとき */
   {
      /* 擬似ヘッダ */
      pseudo_header = (PSEUDO_HEADER *)header; /* headerをPSEUDO_HEADER構造体に当てはめる */
      for (i = 0; i < 4; i++)
      {
         pseudo_header -> ip_src_IP[i] = text_packet -> ip_src_IP[i]; /* 送信元IPアドレス */
         pseudo_header -> ip_dst_IP[i] = text_packet -> ip_dst_IP[i]; /* 宛先IPアドレス */
      }
      pseudo_header -> zero_padding = 0; /* ゼロ・パディング */
      pseudo_header -> ip_protocol = text_packet -> ip_protocol; /* 上位プロトコルの種類（UDP） */
      pseudo_header -> udp_length = text_packet -> udp_length; /* UDPセグメントの長さ */
      
      /* 受信側におけるUDPヘッダのチェックサムの計算 */
      sum = ones_complement_sum(header, 0, 12); /* 1の補数和（擬似ヘッダ） */
      sum += ones_complement_sum(packet, 34, (8 + 16)); /* 1の補数和（UDPセグメント） */
      sum = (sum & 0xFFFF) + (sum >> 16); /* 1の補数和 */
      sum = (~sum) & 0xFFFF; /* 計算結果をNOT演算によって反転する */
      if (sum != 0x0000)
      {
         return 1; /* チェックサムが0x0000にならない場合はエラーが生じたとみなされる */
      }
   }
   
   return 0; /* エラーがなければ，0を戻り値として，呼び出し元の関数に戻る */
}
