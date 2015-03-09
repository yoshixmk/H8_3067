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
   sum = ones_complement_sum(packet, 34, 72); /* 1の補数和（pingメッセージ） */
   sum = (~sum) & 0xFFFF; /* 計算結果をNOT演算によって反転する */
   ping_packet -> ping_checksum = (unsigned short)sum; /* 計算結果をセットする */
   
   packet_send(packet, (14 + 20 + 72)); /* パケットを送信する */
}

void ping_request(unsigned char *packet, unsigned char count)
{
   unsigned short i,j=0;
   unsigned long sum;
   PING_PACKET *ping_packet;
   
   ping_packet = (PING_PACKET *)packet; /* packetをPING_PACKET構造体に当てはめる */
   /* イーサネット・ヘッダ（14byte） */
   for (i = 0; i < 6; i++)
   {
      ping_packet -> eth_dst_MAC[i] = dst_MAC[i];
      ping_packet -> eth_src_MAC[i] = src_MAC[i];
      /* 宛先MACアドレスと送信元MACアドレスをセットする */
   }
   ping_packet -> eth_ethernet_type = 0x0800; /* Ether type (IP)をセットする */

   /* IPヘッダ（20byte） */
   ping_packet -> ip_version_length = 0x45; /* Ver=4 Length=20bytes */
   ping_packet -> ip_service_type = 0x00; /* ToS=0(normal) */
   ping_packet -> ip_total_length = 0x003c; /* TotalLength=60bytes */
   ping_packet -> ip_id = 0x0000 + count; /* ID=1,2,3,4 */
   ping_packet -> ip_flags_fragment_offset = 0x00; /* Flags=0(Don't Fragment) */
   ping_packet -> ip_time_to_live = 0x80; /* TTL=128 */
   ping_packet -> ip_protocol = 0x01; /* Protocol=1(ICMP) */
   for (i = 0; i < 4; i++)
   {
      ping_packet -> ip_dst_IP[i] = dst_IP[i];
      ping_packet -> ip_src_IP[i] = src_IP[i];
      /* 宛先IPアドレスと送信元IPアドレスをセットする */
   }
   /* pingメッセージ（40byte） */
   ping_packet -> ping_type = 0x08; /* Type=8(Echo request) */
   ping_packet -> ping_code = 0x00; /* Code=0 */
   ping_packet -> ping_id = 0x0001; /* ID=1 */
   ping_packet -> ping_sequence_number = 0x0000 + count; /* Seq=1,2,3,4 */
   for(i=0;i<32;i++){
      if(i>=23) {
         j=i-23;
      }else{
         j=i;
      }
      ping_packet -> ping_data[i] = 'a'+j;
   }


   
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
