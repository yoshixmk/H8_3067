unsigned char src_IP[4]; /* ���M��IP�A�h���X */
unsigned char src_MAC[6]; /* ���M��MAC�A�h���X */
unsigned char dst_IP[4]; /* ����IP�A�h���X */
unsigned char dst_MAC[6]; /* ����MAC�A�h���X */
unsigned char packet[256]; /* ����M�p�P�b�g�p */
typedef struct {
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

	/* ping���b�Z�[�W�i14+20+72byte�j */
	unsigned char ping_type;
	unsigned char ping_code;
	unsigned short ping_checksum;
	unsigned short ping_id;
	unsigned short ping_sequence_number;
	unsigned char ping_data[64];
} PING_PACKET;

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

	/* UDP�w�b�_�i8byte+�C�Ӄf�[�^�j */
	unsigned short udp_src_port;
	unsigned short udp_dst_port;
	unsigned short udp_length;
	unsigned short udp_checksum;
	unsigned char udp_data[7];
} UDP_PACKET;