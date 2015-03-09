unsigned char src_IP[4]; /* 送信元IPアドレス */
unsigned char src_MAC[6]; /* 送信元MACアドレス */
unsigned char dst_IP[4]; /* 宛先IPアドレス */
unsigned char dst_MAC[6]; /* 宛先MACアドレス */
unsigned char packet[256]; /* 送受信パケット用 */
typedef struct {
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

typedef struct
{
	/* イーサネット・ヘッダ（14byte） */
	unsigned char eth_dst_MAC[6];
	unsigned char eth_src_MAC[6];
	unsigned short eth_ethernet_type;

	/* IPヘッダ（20byte） */
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

	/* pingメッセージ（14+20+72byte） */
	unsigned char ping_type;
	unsigned char ping_code;
	unsigned short ping_checksum;
	unsigned short ping_id;
	unsigned short ping_sequence_number;
	unsigned char ping_data[64];
} PING_PACKET;

typedef struct
{
	/* イーサネット・ヘッダ（14byte） */
	unsigned char eth_dst_MAC[6];
	unsigned char eth_src_MAC[6];
	unsigned short eth_ethernet_type;

	/* IPヘッダ（20byte） */
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

	/* UDPヘッダ（8byte+任意データ） */
	unsigned short udp_src_port;
	unsigned short udp_dst_port;
	unsigned short udp_length;
	unsigned short udp_checksum;
	unsigned char udp_data[7];
} UDP_PACKET;