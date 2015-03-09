void ARP_request(unsigned char *packet)
{
   unsigned short i;
   ARP_PACKET *arp_packet;
  
   arp_packet = (ARP_PACKET *)packet; /* packetをARP_PACKET構造体に当てはめる */
   
   for (i = 0; i < 6; i++)
   {
      arp_packet -> eth_dst_MAC[i] = 0xFF; /* ブロードキャスト・アドレス */
   }
   for (i = 0; i < 6; i++)
   {
      arp_packet -> eth_src_MAC[i] = src_MAC[i]; /* 送信元MACアドレス */
   }
   arp_packet -> eth_ethernet_type = 0x0806; /* 上位プロトコルの種類（ARP） */
   arp_packet -> arp_hardware_type = 0x0001; /* ネットワークの物理媒体の種類（イーサネット） */
   arp_packet -> arp_protocol_type = 0x0800; /* 上位プロトコルの種類（IP） */
   arp_packet -> arp_hardware_length = 6; /* ネットワークの物理媒体のアドレス長（MACアドレス） */
   arp_packet -> arp_protocol_length = 4; /* 上位プロトコルのアドレス長（IPv4） */
   arp_packet -> arp_operation = 1; /* ARPの動作（ARPリクエスト） */
   for (i = 0; i < 6; i++)
   {
      arp_packet -> arp_src_MAC[i] = src_MAC[i]; /* 送信元MACアドレス */
   }
   for (i = 0; i < 4; i++)
   {
      arp_packet -> arp_src_IP[i] = src_IP[i]; /* 送信元IPアドレス */
   }
   for (i = 0; i < 6; i++)
   {
      arp_packet -> arp_dst_MAC[i] = 0x00; /* 宛先MACアドレス */
   }
   for (i = 0; i < 4; i++)
   {
      arp_packet -> arp_dst_IP[i] = dst_IP[i]; /* 宛先IPアドレス */
   }

   packet_send(packet, 60); /* パケットのサイズを60byteに指定して送信する */
}

void ARP_reply(unsigned char *packet)
{
   unsigned short i;
   ARP_PACKET *arp_packet;
   
   arp_packet = (ARP_PACKET *)packet; /* packetをARP_PACKET構造体に当てはめる */
   
   for (i = 0; i < 6; i++)
   {
      arp_packet -> eth_dst_MAC[i] = arp_packet -> eth_src_MAC[i];
      arp_packet -> eth_src_MAC[i] = src_MAC[i];
      /* 宛先MACアドレスと送信元MACアドレスを入れ替える */
   }
   arp_packet -> arp_operation = 2; /* ARPの動作（ARPリプライ） */
   for (i = 0; i < 6; i++)
   {
      arp_packet -> arp_dst_MAC[i] = arp_packet -> arp_src_MAC[i];
      arp_packet -> arp_src_MAC[i] = src_MAC[i];
      /* 宛先MACアドレスと送信元MACアドレスを入れ替える */
   }
   for (i = 0; i < 4; i++)
   {
      arp_packet -> arp_dst_IP[i] = arp_packet -> arp_src_IP[i];
      arp_packet -> arp_src_IP[i] = src_IP[i];
      /* 宛先IPアドレスと送信元IPアドレスを入れ替える */
   }
   
   packet_send(packet, 60); /* パケットのサイズを60byteに指定して送信する */
}
