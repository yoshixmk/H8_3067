void ping_reply(unsigned char *packet)
{
   unsigned short i;
   unsigned long sum;
   PING_PACKET *ping_packet;
   
   ping_packet = (PING_PACKET *)packet; /* packetをPING_PACKET構造体に当てはめる */
   
   for (i = 0; i < 6; i++)
   {
      ping_packet -> eth_dst_MAC[i] = ping_packet -> eth_src_MAC[i];
      ping_packet -> eth_src_MAC[i] = src_MAC[i];
      /* 宛先MACアドレスと送信元MACアドレスを入れ替える */
   }
   for (i = 0; i < 4; i++)
   {
      ping_packet -> ip_dst_IP[i] = ping_packet -> ip_src_IP[i];
      ping_packet -> ip_src_IP[i] = src_IP[i];
      /* 宛先IPアドレスと送信元IPアドレスを入れ替える */
   }
   ping_packet -> ping_type = 0; /* メッセージの種類（pingリプライ） */
   
   /* 送信側におけるIPヘッダのチェックサムの計算 */
   ping_packet -> ip_checksum = 0x0000; /* チェックサムの初期値を0x0000にする */
   sum = ones_complement_sum(packet, 14, 20); /* 1の補数和（IPヘッダ） */
   sum = (~sum) & 0xFFFF; /* 計算結果をNOT演算によって反転する */
   ping_packet -> ip_checksum = (unsigned short)sum; /* 計算結果をセットする */
   
   /* 送信側におけるpingメッセージのチェックサムの計算 */
   ping_packet -> ping_checksum = 0x0000; /* チェックサムの初期値を0x0000にする */
   sum = ones_complement_sum(packet, 34, 40); /* 1の補数和（pingメッセージ） */
   sum = (~sum) & 0xFFFF; /* 計算結果をNOT演算によって反転する */
   ping_packet -> ping_checksum = (unsigned short)sum; /* 計算結果をセットする */
   
   packet_send(packet, (14 + 20 + 40)); /* パケットを送信する */
}
