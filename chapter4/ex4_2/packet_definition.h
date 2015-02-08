typedef struct
{
   /* イーサネット・ヘッダ（14byte） */
   unsigned char eth_dst_MAC[6];
   unsigned char eth_src_MAC[6];
   unsigned short eth_ethernet_type;
   
   /* ARPメッセージ（28byte） */
   unsigned short arp_hardware_type;
   unsigned short arp_protocol_type;
   unsigned char arp_hardware_length;
   unsigned char arp_protocol_length;
   unsigned short arp_operation;
   unsigned char arp_src_MAC[6];
   unsigned char arp_src_IP[4];
   unsigned char arp_dst_MAC[6];
   unsigned char arp_dst_IP[4];
} ARP_PACKET;
