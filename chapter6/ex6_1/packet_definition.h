typedef struct
{
   /* �C�[�T�l�b�g�E�w�b�_�i14byte�j */
   unsigned char eth_dst_MAC[6];
   unsigned char eth_src_MAC[6];
   unsigned short eth_ethernet_type;
   
   /* ARP���b�Z�[�W�i28byte�j */
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

typedef struct
{
   /* �C�[�T�l�b�g�E�w�b�_�i14byte�j */
   unsigned char eth_dst_MAC[6];
   unsigned char eth_src_MAC[6];
   unsigned short eth_ethernet_type;
   
   /* IP�w�b�_�i20byte�j */
   unsigned char ip_version_length;
   unsigned char ip_service_type;
   unsigned short ip_total_length;
   unsigned short ip_id;
   unsigned short ip_flags_fragment_offset;
   unsigned char ip_time_to_live;
   unsigned char ip_protocol;
   unsigned short ip_checksum;
   unsigned char ip_src_IP[4];
   unsigned char ip_dst_IP[4];
   
   /* UDP�w�b�_�i8byte�j */
   unsigned short udp_src_port;
   unsigned short udp_dst_port;
   unsigned short udp_length;
   unsigned short udp_checksum;
   
   /* �Đ��䃁�b�Z�[�W�i2byte�j */
   unsigned short call_type;
} CALL_PACKET;

typedef struct
{
   /* �C�[�T�l�b�g�E�w�b�_�i14byte�j */
   unsigned char eth_dst_MAC[6];
   unsigned char eth_src_MAC[6];
   unsigned short eth_ethernet_type;
   
   /* IP�w�b�_�i20byte�j */
   unsigned char ip_version_length;
   unsigned char ip_service_type;
   unsigned short ip_total_length;
   unsigned short ip_id;
   unsigned short ip_flags_fragment_offset;
   unsigned char ip_time_to_live;
   unsigned char ip_protocol;
   unsigned short ip_checksum;
   unsigned char ip_src_IP[4];
   unsigned char ip_dst_IP[4];
   
   /* UDP�w�b�_�i8byte�j */
   unsigned short udp_src_port;
   unsigned short udp_dst_port;
   unsigned short udp_length;
   unsigned short udp_checksum;
   
   /* RTP�w�b�_�i12byte�j */
   unsigned char rtp_v_p_x_cc;
   unsigned char rtp_m_payload_type;
   unsigned short rtp_sequence_number;
   unsigned long rtp_time_stamp;
   unsigned long rtp_ssrc;
   
   /* �����f�[�^�i160byte�j */
   unsigned char speech_data[160];
} SPEECH_PACKET;

typedef struct
{
   /* �[���w�b�_�i12byte�j */
   unsigned char ip_src_IP[4];
   unsigned char ip_dst_IP[4];
   unsigned char zero_padding;
   unsigned char ip_protocol;
   unsigned short udp_length;
} PSEUDO_HEADER;
