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

	/* pingメッセージ（40byte） */
	unsigned char ping_type;
	unsigned char ping_code;
	unsigned short ping_checksum;
	unsigned short ping_id;
	unsigned short ping_sequence_number;
	unsigned char ping_data[32];/*データサイズが32byteの場合*/
} PING_PACKET;

typedef struct
{
	unsigned char eth_dst_MAC[6];
	unsigned char eth_src_MAC[6];
	unsigned short eth_ethernet_type;

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

	unsigned short udp_src_port;
	unsigned short udp_dst_port;
	unsigned short udp_length;
	unsigned short udp_checksum;

	/* テキスト・データ(16byte) */
	char text_data[16];
} UDP_PACKET;

#include "H8_register.h"
#include "H8_register_function.h"
#include "RTL8019AS_register.h"
#include "timer_function.h"
#include "RTL8019AS_function.h"
#include "lcd_function.h"
#include "matrix_key_function.h"
#include "utility_function.h"
#include "ARP_function.h"
#include "ping_function.h"

void main(void)
{
	ARP_PACKET *arp_packet; /* ARPパケット構造体 */
	PING_PACKET *ping_packet; /* pingパケット構造体 */
	UDP_PACKET *udp_packet;
	unsigned char packet_type; /* パケットタイプ判定用 */

	src_IP[0]=10;
	src_IP[1]=1;
	src_IP[2]=68;
	src_IP[3]=139;

	H8_register_init(); /* H8レジスタ初期化・設定 */

	LCD_init(); /* LCD初期化 */

	NIC_init(); /* NIC初期化 */
	LCD_control(0x01);/*LCD clear*/
	LCD_control(0x80); /* カーソルを1行目の先頭に移動する */
	ms_timer(1000); /* Wait */

	while(1){
		packet_type = 0;

		if (packet_receive(packet) != 1) /* パケットを受信したとき */
		{
			arp_packet = (ARP_PACKET *)packet; /* packetをARP_PACKET構造体に当てはめる */
			if ((arp_packet -> eth_ethernet_type == 0x0806) && 
				/* ARPのパケットのとき */
				(IP_compare(arp_packet -> arp_dst_IP, src_IP) == 0) && 
				/* パケットに記述されている宛先IPアドレスが送信元IPアドレスに一致したとき */
				(arp_packet -> arp_operation == 1)
				/* ARP Requestのとき */
				)
			{
				packet_type = 'a'; /* packet_typeをaにする */
			}
			ping_packet = (PING_PACKET *)packet; /* packetをPING_PACKET構造体に当てはめる */
			if ((ping_packet -> eth_ethernet_type == 0x0800) && 
				/* IPのパケットのとき */
				(IP_compare(ping_packet -> ip_dst_IP, src_IP) == 0) && 
				/* パケットに記述されている宛先IPアドレスが送信元IPアドレスに一致したとき */
				(ping_packet -> ip_protocol == 1) && 
				/* ICMPのパケットのとき */
				(ping_packet -> ping_type == 8)
				/* pingリクエストのとき */
				)
			{
				packet_type = 'p'; /* packet_typeを'p'にする */
			}
			udp_packet =  (UDP_PACKET *)packet;
            if((udp_packet -> eth_ethernet_type == 0x0800) && (IP_compare(udp_packet -> ip_dst_IP, src_IP) == 0) && (udp_packet -> udp_dst_port == 30000)){
                packet_type = 'u';
                UDP_text_receive(packet);
            }

			switch (packet_type) /* packet_typeにしたがって処理を行う */
			{
			case 'a': /* packet_typeが'a'のとき */
				ARP_reply(packet); /* ARPリプライ */
				break;
			case 'p': /* packet_typeが'p'のとき */
				LCD_display('!'); /* 液晶ディスプレイにpacket_typeを表示する */
				ping_reply(packet); /* pingリプライ */
				break;
			case 'u':
			    /*LCD_control(0xc0);*/
			    LCD_print(udp_packet -> text_data);
			    break;
			}
		}
	}
}
