unsigned char src_IP[4]; /* 送信元IPアドレス */
unsigned char src_MAC[6]; /* 送信元MACアドレス */
unsigned char dst_IP[4]; /* 宛先IPアドレス */
unsigned char dst_MAC[6]; /* 宛先MACアドレス */
unsigned char packet[256]; /* 送受信パケット用 */

#include <string.h>
#include "H8_register.h"
#include "H8_register_function.h"
#include "RTL8019AS_register.h"
#include "timer_function.h"
#include "RTL8019AS_function.h"
#include "lcd_function.h"
#include "matrix_key_function.h"
#include "utility_function.h"
#include "global_variables.h"
#include "ARP_function.h"
#include "ping_function.h"

void main(void)
{
	ARP_PACKET *arp_packet; /* ARPパケット構造体 */
	UDP_PACKET *udp_packet; /* pingパケット構造体 */
	unsigned char packet_type; /* パケットタイプ判定用 */
	char lcd_data[25];
	int i,j,k;
	/* 送信元IPアドレス  10.1.68.160*/
	src_IP[0]=10;
	src_IP[1]=1;
	src_IP[2]=69;
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
			udp_packet = (UDP_PACKET *)packet; /* packetをUDP_PACKET構造体に当てはめる */
			if ((arp_packet -> eth_ethernet_type == 0x0806) && 
				/* ARPのパケットのとき */
				(IP_compare(arp_packet -> arp_dst_IP, src_IP) == 0) && 
				/* パケットに記述されている宛先IPアドレスが送信元IPアドレスに一致したとき */
				(arp_packet -> arp_operation == 1)
				/* ARP Requestのとき */
				)
			{
				ARP_reply(packet);
			}
			if ((udp_packet -> eth_ethernet_type == 0x0800) && 
				/* IPのパケットのとき */
				(IP_compare(udp_packet -> ip_dst_IP, src_IP) == 0) && 
				/* パケットに記述されている宛先IPアドレスが送信元IPアドレスに一致したとき */
				(udp_packet -> ip_protocol == 17) && 
				/* ICMPのパケットのとき */
				(udp_packet -> udp_dst_port == 30000)
				/* UDPポートが12345のとき */
				)
			{
				LCD_control(0x80);
				for(i=0;i<25;i++)
				{
					if(i<8)lcd_data[i] = packet[42+i];
					else lcd_data[i] = ' ';
				}
                LCD_print(lcd_data);
                LCD_control(0x80);
                ms_timer(1500);
			}
		}
	}
}
